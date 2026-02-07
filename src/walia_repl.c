#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/common.h"
#include "core/vm.h"
#include "core/scanner.h"
#include "core/parser.h"
#include "core/compiler.h"
#include "core/object.h"
#include "persistence.h"

// ==========================================
// REPL BRANDING & UI
// ==========================================

#define COLOR_WAL "\033[1;34m"
#define COLOR_RES "\033[0;32m"
#define COLOR_ERR "\033[0;31m"
#define COLOR_NC  "\033[0m"

/**
 * @brief Professional REPL Welcome Banner.
 */
static void print_banner() {
    printf("Walia Sovereign Shell [v1.0.0]\n");
    printf("Type \".help\" for commands, \".exit\" to save and quit.\n");
    printf("Persistence: " COLOR_RES "ACTIVE" COLOR_NC " (walia.state)\n\n");
}

/**
 * @brief Native Help Menu for the Interactive Environment.
 */
static void print_help() {
    printf("\nSovereign REPL Commands:\n");
    printf("  .exit      - Atomic checkpoint and sovereign shutdown\n");
    printf("  .clear     - Purge temporary terminal buffer\n");
    printf("  .state     - Show persistent heap statistics\n");
    printf("  .help      - Display this command menu\n\n");
}

// ==========================================
// THE REPL ENGINE
// ==========================================

/**
 * @brief The Interactive Execution Loop.
 * 
 * Logic:
 * 1. Read: Captures a line of Walia source from stdin.
 * 2. Parse: Converts the string into a temporary AST.
 * 3. Compile: JIT compiles the AST into a persistent logic chunk.
 * 4. Execute: Dispatches the VM on the sovereign heap.
 * 5. Checkpoint: Flushes mutations to the .state file immediately.
 */
void walia_repl_start() {
    print_banner();
    
    // Initialize the VM which mounts the persistent walia.state image.
    initVM(); 

    char line[1024];
    for (;;) {
        printf(COLOR_WAL "wal> " COLOR_NC);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        // 1. REPL Meta-Commands
        if (line[0] == '.') {
            if (strncmp(line, ".exit", 5) == 0) break;
            if (strncmp(line, ".help", 5) == 0) {
                print_help();
                continue;
            }
            if (strncmp(line, ".clear", 6) == 0) {
                // Portable clear command
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                continue;
            }
            if (strncmp(line, ".state", 6) == 0) {
                printf("Sovereign Heap Usage: %zu bytes allocated.\n", vm.bytesAllocated);
                continue;
            }
        }

        // 2. Front-End: Parse Line to AST
        int stmtCount = 0;
        Stmt** statements = parse(line, &stmtCount);

        // Fixed: Check the global 'hadError' variable from common.h
        if (hadError || statements == NULL) {
            continue; // Syntax error already reported by the parser
        }

        // 3. Middle-End: JIT Compile to Bytecode
        Chunk chunk;
        initChunk(&chunk);
        if (!compile(statements, stmtCount, &chunk)) {
            freeChunk(&chunk);
            // Cleanup AST nodes before next prompt
            for (int i = 0; i < stmtCount; i++) {
                if (statements[i] != NULL) freeStmt(statements[i]);
            }
            free(statements);
            continue;
        }

        // 4. Back-End: Sovereign Execution
        ObjFunction* scriptFunc = newFunction();
        scriptFunc->chunk = chunk;
        
        InterpretResult result = interpret(scriptFunc);

        // 5. Orthogonal Persistence: Immediate Durability
        if (result == INTERPRET_OK) {
            // Every successful REPL interaction is atomically saved
            walia_checkpoint();
        }

        // 6. Memory Hygiene: Reclaim Volatile Logic
        for (int i = 0; i < stmtCount; i++) {
            if (statements[i] != NULL) freeStmt(statements[i]);
        }
        free(statements);
        // Note: The scriptFunc object itself is eventually reclaimed by the GC
    }

    printf(">> Walia: Performing final sovereign checkpoint...\n");
    freeVM(); // Triggers atomic msync and unmap
    printf("Sovereign Shutdown Complete.\n");
}
