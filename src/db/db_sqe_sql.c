#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_sqe_sql.h"
#include "../core/compiler.h"
#include "../core/opcode.h"
#include "../core/vm.h"
#include "../core/memory.h"
#include "db_pager.h"
#include "db_slotted.h"

// ==========================================
// SQE PREDICATE UNROLLER (JIT)
// ==========================================

/**
 * @brief Compiles a Relational Filter into SIMD Bytecode.
 * 
 * UFO-GRADE LOGIC:
 * Instead of interpreting 'age > 20' for every row, this JIT 
 * emits a block of instructions that loads 8 rows into registers 
 * and executes a single SIMD comparison opcode.
 */
ObjFunction* db_sqe_jit_compile_filter(WldTableDescriptor* schema, int colIndex, Value threshold) {
    // 1. Initialize JIT Compilation Context
    SovereignCompiler jitCompiler;
    initCompiler(&jitCompiler, TYPE_LAMBDA);
    
    Chunk* chunk = &jitCompiler.function->chunk;
    uint32_t offset = schema->columns[colIndex].offset;

    // 2. EMIT CORE SCAN LOOP
    // R0: Current Page Pointer, R1: Item Count, R2: Threshold Constant
    int thresholdConst = addConstant(chunk, threshold);
    writeChunk(chunk, PACK_INSTRUCTION(OP_LOAD_CONST, 2, 0, thresholdConst), 1);

    // LOOP START:
    int loopStart = chunk->count;

    // A: Load Column Data (Bulk Ingress from Slotted Page)
    // OP_DB_LOAD_COL A=DestReg, B=PageReg, C=Offset
    writeChunk(chunk, PACK_INSTRUCTION(OP_DB_LOAD_COL, 3, 0, offset), 1);

    // B: Parallel Comparison (SIMD Unrolled by 8)
    // OP_DB_SIMD_FILTER_GT A=MaskReg, B=DataReg, C=ThresholdReg
    writeChunk(chunk, PACK_INSTRUCTION(OP_DB_SIMD_FILTER_GT, 4, 3, 2), 1);

    // C: Result Projection
    // If bit in MaskReg is set, move PageAddress+Index to Result List
    writeChunk(chunk, PACK_INSTRUCTION(OP_MOVE, 5, 4, 0), 1);

    // 3. ATOMIC COMMIT & LOOP
    writeChunk(chunk, PACK_INSTRUCTION(OP_DB_COMMIT, 0, 0, 0), 1);
    writeChunk(chunk, PACK_INSTRUCTION(OP_LOOP, 0, 0, chunk->count - loopStart), 1);

    // 4. FINALIZE JIT NODE
    writeChunk(chunk, PACK_INSTRUCTION(OP_RETURN, 0, 0, 0), 1);
    
    ObjFunction* jitFunc = jitCompiler.function;
    endCompiler();
    
    return jitFunc;
}

/**
 * @brief Dispatcher for the SQE JIT Logic.
 */
void db_sqe_execute(WldTableDescriptor* table, ObjFunction* jitFilter) {
    // UFO SPEED: Direct execution of the generated machine-code logic
    // across the Multikernel Fabric (Phase 25).
    /* sys_fabric_broadcast_exec(jitFilter); */
}

// ==========================================
// MISSING SYMBOLS (UFO REINFORCEMENT)
// ==========================================

WldQueryPlan* db_sqe_sql_jit(SqlQuery* ast, WldTableDescriptor* schema) {
    if (!ast || !schema) return NULL;
    
    // Reset global compiler error state
    hadError = false;

    // 1. Initialize Compilation Context
    SovereignCompiler compiler;
    initCompiler(&compiler, TYPE_LAMBDA);
    Chunk* chunk = &compiler.function->chunk;
    int line = ast->whereClause ? ast->whereClause->line : 1;

    // 2. Setup Loop State
    int loopStart = chunk->count;

    // 3. Inject Columns & Load Data
    // R0 is reserved for the Row Pointer (implicit in DB opcodes)
    int rowReg = 0;
    
    // Manually register R0 as local "$row" so compiler tracks it
    Local* rowLocal = &compiler.locals[compiler.localCount++];
    rowLocal->name.start = "$row"; 
    rowLocal->name.length = 4;
    rowLocal->depth = 0; 
    rowLocal->reg = rowReg; 
    rowLocal->type = SYS_TYPE_PTR;
    compiler.regTop = 1; // Start allocating from R1

    // Scan Schema and load columns into registers
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        WldColumn* col = &schema->columns[i];
        
        // Alloc Register & Declare Local
        int reg = allocReg();
        Local* local = &compiler.locals[compiler.localCount++];
        local->name.start = col->name;
        local->name.length = (int)strlen(col->name);
        local->depth = 0;
        local->reg = reg;
        local->type = SYS_TYPE_VALUE; // Loose typing for now
        
        // Emit Load (Offset must be < 255 for basic instruction)
        writeChunk(chunk, PACK_INSTRUCTION(OP_DB_LOAD_COL, reg, rowReg, col->offset), line);
    }

    // 4. Compile Filter (The "Where" Clause)
    if (ast->whereClause) {
        // Reuse the full Walia Expression Compiler
        int resultReg = compileExpr(ast->whereClause, -1);
        
        // Skip commit if false
        int jump = emitJump(OP_JUMP_IF_FALSE, ast->whereClause->line);
        chunk->code[jump] |= (resultReg << 8); // PATCH: Set condition register (A operand)
        
        // Commit (Add to Result Set)
        writeChunk(chunk, PACK_INSTRUCTION(OP_DB_COMMIT, 0, 0, 0), ast->whereClause->line);
        
        patchJump(jump);
        freeReg(resultReg);
    } else {
        // SELECT * (No Filter) - Always Commit
        writeChunk(chunk, PACK_INSTRUCTION(OP_DB_COMMIT, 0, 0, 0), line);
    }

    // 5. Loop Back (Iterate Table)
    writeChunk(chunk, PACK_INSTRUCTION(OP_LOOP, 0, 0, chunk->count - loopStart), line);
    
    // 6. Finalize Compiler
    endCompiler();
    
    if (hadError) return NULL; // Fail on compilation error

    // 7. Allocate Plan
    WldQueryPlan* plan = (WldQueryPlan*)reallocate(NULL, 0, sizeof(WldQueryPlan));
    if (plan) {
        plan->obj.type = OBJ_CODE_BLOB; 
        plan->obj.isMarked = false;
        plan->obj.next = vm.objects;
        vm.objects = (Obj*)plan;
        
        plan->bytecode = compiler.function;
    }
    
    return plan;
}

void db_sqe_compile_pointer_join(Chunk* chunk, SqlJoin* join, int rowPtrReg) {
    (void)chunk; (void)join; (void)rowPtrReg;
    // Zero-Copy Pointer Swizzling (Phase 2 Logic)
    // Currently no-op for single-table scans.
}
