#ifndef WALIA_ANALYSIS_H
#define WALIA_ANALYSIS_H

#include "core/ast.h"
#include "core/common.h"
#include "core/compiler.h" // Includes SovereignCompiler definition

// ==========================================
// SEMANTIC ANALYZER API
// ==========================================

/**
 * @brief Analyzes the AST for scope and validation.
 */
bool analyze(Stmt** statements, int count);

/**
 * @brief Resolves a local variable within a specific compiler context.
 */
int resolveLocal(SovereignCompiler* compiler, Token name);

/**
 * @brief Resolves an upvalue (closure variable) within a compiler chain.
 */
int resolveUpvalue(SovereignCompiler* compiler, Token name);

#endif // WALIA_ANALYSIS_H
