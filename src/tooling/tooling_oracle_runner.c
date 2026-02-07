#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling/tooling_oracle_runner.h"
#include "tooling/tooling_simulator.h"
#include "tooling/tooling_ledger.h"
#include "core/compiler.h"
#include "core/parser.h"

// ==========================================
// ANONYMOUS JIT SPLICING
// ==========================================

/**
 * @brief Compiles a raw string snippet into a callable ObjFunction.
 */
static ObjFunction* compile_snippet(const char* body) {
    int stmtCount = 0;
    Stmt** statements = parse(body, &stmtCount);

    if (hadError || statements == NULL) {
        return NULL; // Snippet itself has syntax errors
    }

    Chunk chunk;
    initChunk(&chunk);
    if (!compile(statements, stmtCount, &chunk)) {
        freeChunk(&chunk);
        return NULL;
    }

    ObjFunction* func = newFunction();
    func->chunk = chunk;
    func->name = copyString("oracle_test", 11);

    // Cleanup temporary AST
    for (int i = 0; i < stmtCount; i++) freeStmt(statements[i]);
    free(statements);

    return func;
}

// ==========================================
// VERIFICATION ENGINE
// ==========================================

bool walia_oracle_verify_snippet(WldDocSnippet* snippet) {
    printf(">> Walia Oracle: Verifying example for '%s'...\n", snippet->targetSymbol);

    // 1. JIT Compile the Documentation Example
    ObjFunction* testFunc = compile_snippet(snippet->body);
    if (!testFunc) {
        walia_ledger_log(FAULT_CRITICAL, "Documentation syntax error", snippet->targetSymbol, 
                         "manual", snippet->lineStart, NULL, CURE_NONE);
        return false;
    }

    // 2. Ghost Execution (Simulator)
    // We run the test in the sandbox to prevent side effects
    WaliaSimReport report = walia_simulate_logic(testFunc);

    // 3. Truth Enforcement
    if (report.status != SIM_SUCCESS) {
        char reason[512];
        snprintf(reason, 512, "Documentation truth-violation: %s (Sim Error: %d)", 
                 snippet->description, report.status);
        
        // LOG TO LEDGER: This blocks the build
        walia_ledger_log(FAULT_CRITICAL, reason, snippet->targetSymbol, 
                         "manual", snippet->lineStart, NULL, CURE_NONE);
        
        printf("%sFAIL | Documentation example for '%s' is UNTRUTHFUL.%s\n", 
               "\033[31m", snippet->targetSymbol, "\033[0m");
        return false;
    }

    printf("%sPASS | Documentation verified for '%s'.%s\n", 
           "\033[32m", snippet->targetSymbol, "\033[0m");
    return true;
}

void walia_oracle_run_suite() {
    int count = 0;
    WldDocSnippet* snippets = walia_oracle_get_snippets(&count);

    if (count == 0) {
        printf(">> Walia Oracle: No documentation contracts found.\n");
        return;
    }

    printf("\n--- WALIA ORACLE: VERIFYING EXECUTIVE TRUTH ---\n");

    int passCount = 0;
    for (int i = 0; i < count; i++) {
        if (walia_oracle_verify_snippet(&snippets[i])) {
            passCount++;
        }
    }

    printf("-----------------------------------------------\n");
    if (passCount == count) {
        printf("ORACLE STATUS: %sSOVEREIGN TRUTH PRESERVED (%d/%d)%s\n\n", 
               "\033[32m", passCount, count, "\033[0m");
    } else {
        printf("ORACLE STATUS: %sDOCUMENTATION ROT DETECTED (%d faults)%s\n\n", 
               "\033[31m", (count - passCount), "\033[0m");
    }
}
