#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_sqe_nosql.h"
#include "../core/opcode.h"
#include "../core/memory.h" 
#include "../core/object.h" 
#include "../core/chunk.h"  

// ==========================================
// INTERNAL JIT EMITTERS
// ==========================================

static void emitOp(Chunk* chunk, int op, int a, int b, int c) {
    writeChunk(chunk, PACK_INSTRUCTION(op, a, b, c), 0);
}

static void emitOpBC(Chunk* chunk, int op, int a, int bc) {
    writeChunk(chunk, PACK_INSTRUCTION(op, a, (bc >> 8) & 0xFF, bc & 0xFF), 0);
}

/**
 * @brief Emits a jump instruction with a placeholder offset.
 */
static int emitJump(Chunk* chunk, uint8_t op, int a) {
    emitOpBC(chunk, op, a, 0xffff);
    return chunk->count - 1;
}

/**
 * @brief Patches a previously emitted jump with the correct relative offset.
 */
static void patchJump(Chunk* chunk, int offset) {
    // Calculate distance from jump instruction to next instruction
    int jump = chunk->count - offset - 1;

    if (jump > 65535) {
        fprintf(stderr, "WaliaDB SQE: JIT Jump offset too large.\n");
        return;
    }

    Instruction inst = chunk->code[offset];
    chunk->code[offset] = PACK_INSTRUCTION(GET_OP(inst), GET_A(inst), 
                                          (jump >> 8) & 0xFF, jump & 0xFF);
}

// ==========================================
// ASYMMETRIC DISTANCE JIT (Neural Core)
// ==========================================

static void compile_neural_loop(Chunk* chunk, ObjVector* query, WldDistanceMetric metric) {
    int queryIdx = addConstant(chunk, OBJ_VAL(query));
    int metricIdx = addConstant(chunk, NUMBER_VAL((double)metric));

    int loopStart = chunk->count;

    // Load context and perform SIMD match
    emitOpBC(chunk, OP_LOAD_CONST, 2, queryIdx);
    emitOp(chunk, OP_DB_SIMD_FILTER_GT, 3, 2, metricIdx);

    // Loop back to process trillions of neurons
    emitOpBC(chunk, OP_LOOP, 0, chunk->count - loopStart);
}

// ==========================================
// DYNAMIC NOSQL FILTER JIT
// ==========================================

ObjFunction* db_sqe_nosql_filter_jit(const char* fieldName, Value targetValue) {
    ObjFunction* func = newFunction();
    Chunk* chunk = &func->chunk;

    // Logic: R2 = GetField(R1, fieldName); if (R2 != targetValue) skip;
    int fieldNameIdx = addConstant(chunk, OBJ_VAL(copyString(fieldName, (int)strlen(fieldName))));
    int targetValIdx = addConstant(chunk, targetValue);

    emitOpBC(chunk, OP_LOAD_CONST, 4, fieldNameIdx);
    emitOpBC(chunk, OP_LOAD_CONST, 5, targetValIdx);
    
    // Perform dynamic attribute lookup
    emitOp(chunk, OP_GET_GLOBAL, 2, 1, 4); 
    emitOp(chunk, OP_EQUAL, 3, 2, 5);

    // Fixed: emitJump and patchJump now defined locally
    int skipJump = emitJump(chunk, OP_JUMP_IF_FALSE, 3);
    
    // Result Collection (Placeholder for worker results)
    emitOp(chunk, OP_PRINT, 1, 0, 0); 
    
    patchJump(chunk, skipJump);
    emitOp(chunk, OP_RETURN, 0, 0, 0);

    return func;
}

// ==========================================
// PUBLIC SQE NOSQL API
// ==========================================

WldNeuralPlan* db_sqe_vector_jit(ObjVector* query, WldDistanceMetric metric) {
    WldNeuralPlan* plan = ALLOCATE(WldNeuralPlan, 1);
    plan->obj.type = OBJ_CODE_BLOB;
    plan->obj.isMarked = false;
    plan->obj.next = NULL;

    memset(plan->queryHash, 0, 32); 

    ObjFunction* func = newFunction();
    compile_neural_loop(&func->chunk, query, metric);
    
    plan->jitFunc = func;
    
    printf(">> WaliaDB SQE: JIT Compiled Asymmetric Neural Loop.\n");
    return plan;
}
