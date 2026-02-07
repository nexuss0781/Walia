#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include "object.h"
#include "memory.h"

/**
 * @brief THE SOVEREIGN INTERPRETER BRIDGE
 * 
 * UFO-GRADE Implementation:
 * This function provides an atomic pipeline from source code to VM execution.
 * It is used for both REPL interactions and Kernel-level substrate ignition.
 */
void walia_interpret(const char* source) {
    if (source == NULL) return;

    // 1. FRONT-END: Lexical & Syntactic Analysis
    int stmtCount = 0;
    Stmt** statements = parse(source, &stmtCount);

    // Check for parse-time errors (Truth Gate)
    if (hadError || statements == NULL) {
        return; 
    }

    // 2. MIDDLE-END: JIT Compilation to Bytecode
    Chunk chunk;
    initChunk(&chunk);
    
    if (!compile(statements, stmtCount, &chunk)) {
        // Critical Failure: Reclaim partial work
        freeChunk(&chunk);
        for (int i = 0; i < stmtCount; i++) {
            if (statements[i] != NULL) freeStmt(statements[i]);
        }
        free(statements);
        return;
    }

    // 3. BACK-END: Logical Realization
    // Wrap the chunk in a top-level script function object.
    ObjFunction* function = newFunction();
    function->chunk = chunk;
    // Embed the function name for stack tracing
    function->name = copyString("substrate_ignition", 18);

    // 4. EXECUTION
    InterpretResult result = interpret(function);
    
    // 5. HYGIENE: Post-Execution Resource Reclamation
    // Note: The 'function' object is managed by the GC since it's on the heap.
    for (int i = 0; i < stmtCount; i++) {
        if (statements[i] != NULL) freeStmt(statements[i]);
    }
    free(statements);

    if (result != INTERPRET_OK) {
        // Optional: Log runtime failure for kernel debugging
    }
}
