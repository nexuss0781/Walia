#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // Added for memcmp
#include "compiler.h"  // Added to fix currentChunk, emitOp, etc.
#include "compiler_pipe.h"
#include "opcode.h"

// ==========================================
// FUSION ENGINE LOGIC
// ==========================================

/**
 * @brief Linearizes the left-recursive PipeExpr tree.
 */
static void flatten_pipe_chain(Expr* expr, Expr** stages, int* count) {
    if (expr->type != EXPR_PIPE) {
        stages[(*count)++] = expr;
        return;
    }
    
    PipeExpr* p = (PipeExpr*)expr;
    flatten_pipe_chain(p->left, stages, count);
    stages[(*count)++] = p->right;
}

void walia_compiler_fuse_pipeline(PipeExpr* expr, int targetReg) {
    int line = expr->obj.line;
    
    // 1. STAGE EXTRACTION
    Expr* stages[32]; 
    int stageCount = 0;
    // (flatten_pipe_chain is static helper in this file)
    flatten_pipe_chain((Expr*)expr, stages, &stageCount);

    // 2. PARALLEL MODIFIER DETECTION
    int shardCount = 1;
    int startOffset = 1;

    if (stageCount > 1 && stages[1]->type == EXPR_CALL) {
        CallExpr* call = (CallExpr*)stages[1];
        if (call->callee->type == EXPR_VARIABLE) {
            Token name = ((VariableExpr*)call->callee)->name;
            if (name.length == 8 && memcmp(name.start, "parallel", 8) == 0) {
                if (call->argCount > 0 && call->arguments[0]->type == EXPR_LITERAL) {
                    shardCount = (int)AS_NUMBER(((LiteralExpr*)call->arguments[0])->value);
                }
                startOffset = 2; 
            }
        }
    }

    // 3. SOURCE & STREAM INITIALIZATION
    // Fixed: Now calling the public compileExpr function
    int sourceReg = compileExpr(stages[0], -1);
    
    // OP_ITER_INIT A=Dest, B=Source, C=ShardCount
    emitOp(OP_ITER_INIT, targetReg, sourceReg, shardCount, line);

    // 4. FUSED LOOP GENERATION
    // Fixed: Now correctly resolving currentChunk() as a pointer
    int loopStart = currentChunk()->count;
    
    // FETCH (Parallel Work-Stealing)
    int exitJump = emitJump(OP_ITER_NEXT, line);
    currentChunk()->code[exitJump] |= (0 << 8);        
    currentChunk()->code[exitJump] |= (targetReg << 16); 

    // 5. TRANSFORMATION PIPELINE (Register Folding)
    for (int i = startOffset; i < stageCount; i++) {
        int callee = compileExpr(stages[i], -1);
        emitOp(OP_MOVE, 1, 0, 0, line); 
        emitOp(OP_CALL, 0, callee, 1, line);
        freeReg(callee);
    }

    // 6. LOOP CLOSURE
    emitOpBC(OP_LOOP, 0, currentChunk()->count - loopStart, line);
    patchJump(exitJump);

    freeReg(sourceReg);
}
