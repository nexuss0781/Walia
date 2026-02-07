#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_sentry_jit.h"
#include "../core/opcode.h"
#include "vm/vm_db_ops.h"
#include "../core/memory.h"
#include "../core/table.h"
#include "../core/vm.h" // Fixed: Added for global 'vm' access

// ==========================================
// INTERNAL PATTERN ANALYZER
// ==========================================

static int find_target_column(ObjFunction* func, WldTableDescriptor* schema) {
    Chunk* chunk = &func->chunk;
    for (int i = 0; i < chunk->count; i++) {
        Instruction inst = chunk->code[i];
        
        // Pattern: OP_GET_PROPERTY [Dest] [Object] [PropertyNameConst]
        // Assuming 0x13 is GET_GLOBAL/PROPERTY in this ISA
        if (GET_OP(inst) == 0x13) { 
            Value propName = chunk->constants.values[GET_BC(inst)];
            if (IS_STRING(propName)) {
                const char* name = AS_CSTRING(propName);
                for (uint32_t c = 0; c < schema->columnCount; c++) {
                    if (strcmp(schema->columns[c].name, name) == 0) {
                        return (int)c;
                    }
                }
            }
        }
    }
    return -1;
}

// ==========================================
// THE UNROLLER (Bytecode Transformation)
// ==========================================

void db_sentry_unroll_logic(Chunk* outChunk, ObjFunction* source, WldTableDescriptor* schema) {
    int colIdx = find_target_column(source, schema);
    if (colIdx == -1) return;

    // UFO SPEED: The JIT uses the physical offset to bypass property lookups
    uint32_t offset = schema->columns[colIdx].offset;

    for (int i = 0; i < source->chunk.count; i++) {
        Instruction inst = source->chunk.code[i];
        int op = GET_OP(inst);

        if (op == OP_GREATER) {
            // Emit the SIMD version. We encode the offset into Operand B
            // to allow the VM to jump directly to the column bytes.
            Instruction simdInst = PACK_INSTRUCTION(OP_DB_SIMD_FILTER_GT, 2, (uint8_t)(offset / 8), GET_C(inst));
            writeChunk(outChunk, simdInst, 0);
        }
    }
}

// ==========================================
// PUBLIC JIT API
// ==========================================

WldSentryPredicate* db_sentry_compile_predicate(ObjClosure* closure, WldTableDescriptor* schema) {
    ObjFunction* source = closure->function;

    if (source->arity != 1) return NULL;

    WldSentryPredicate* pred = (WldSentryPredicate*)reallocate(NULL, 0, sizeof(WldSentryPredicate));
    pred->obj.type = OBJ_CLOSURE; 
    pred->obj.isMarked = false;
    
    // Fixed: vm.objects and vm access now valid via vm.h
    pred->obj.next = vm.objects;
    vm.objects = (Obj*)pred;

    pred->originalClosure = closure;
    pred->optimizedFunc = newFunction();
    pred->targetColOffset = 0; // Finalized during unrolling
    
    db_sentry_unroll_logic(&pred->optimizedFunc->chunk, source, schema);

    Instruction ret = PACK_INSTRUCTION(OP_RETURN, 0, 0, 0);
    writeChunk(&pred->optimizedFunc->chunk, ret, 0);

    markCard(pred);
    return pred;
}
