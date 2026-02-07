#ifndef WALIA_TOOLING_ORACLE_RUNNER_H
#define WALIA_TOOLING_ORACLE_RUNNER_H

#include "core/common.h"
#include "tooling/tooling_oracle_lexer.h"

// ==========================================
// ORACLE RUNNER API
// ==========================================

/**
 * @brief Orchestrates the execution of all registered documentation snippets.
 * 
 * Logic:
 * 1. For each snippet, wrap the body in an anonymous function.
 * 2. JIT compile the snippet to Walia Bytecode.
 * 3. Run the bytecode through the Ghost Executioner (Simulator).
 * 4. Report failures to the Diagnostic Ledger.
 */
void walia_oracle_run_suite();

/**
 * @brief Analyzes a specific documentation snippet.
 */
bool walia_oracle_verify_snippet(WldDocSnippet* snippet);

#endif // WALIA_TOOLING_ORACLE_RUNNER_H
