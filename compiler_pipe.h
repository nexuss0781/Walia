#ifndef WALIA_COMPILER_PIPE_H
#define WALIA_COMPILER_PIPE_H

#include "compiler.h"
#include "ast.h"

// ==========================================
// FUSION JIT API
// ==========================================

/**
 * @brief Compiles a pipeline chain into a fused execution loop.
 * 
 * Logic:
 * 1. Flattens the recursive PipeExpr tree into a linear sequence of stages.
 * 2. Emits code to initialize the Sovereign Stream source.
 * 3. Generates a tight OP_LOOP that executes every transformation in-place.
 * 
 * @param expr The root PipeExpr node.
 * @param targetReg The register where the final stream/result should reside.
 */
void walia_compiler_fuse_pipeline(PipeExpr* expr, int targetReg);

#endif // WALIA_COMPILER_PIPE_H