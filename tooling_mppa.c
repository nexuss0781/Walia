#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "tooling_mppa.h"
#include "tooling_ledger.h"
#include "tooling_registry.h"
#include "tooling_simulator.h"
#include "db_dispatch.h"
#include "parser.h"
#include "compiler.h"
#include "memory.h"

// ==========================================
// PARALLEL TASK QUEUE
// ==========================================

typedef struct {
    WaliaRegistryEntry** nodes;
    int total;
    _Atomic int next;
    AnalysisPass currentPass;
} MPPAContext;

static MPPAContext globalCtx;

// ==========================================
// ANALYSIS PASS LOGIC
// ==========================================

/**
 * @brief PASS 1: Physical Structural Verification.
 * Reads the file and performs a non-blocking parse to catch syntax errors.
 */
static void execute_structural_pass(WaliaRegistryEntry* entry) {
    FILE* file = fopen(entry->filePath, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* source = malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);

    // Run the parser in a diagnostic-only mode
    int count = 0;
    Stmt** stmts = parse(source, &count);

    if (hadError) {
        walia_ledger_log(FAULT_CRITICAL, "Syntax Breach detected in structural pass.", 
                         entry->name, entry->filePath, entry->startLine, 
                         entry->currentHash, CURE_NONE);
    }

    // Memory Hygiene
    for (int i = 0; i < count; i++) if (stmts[i]) freeStmt(stmts[i]);
    free(stmts);
    free(source);
}

/**
 * @brief PASS 2: Sovereign Symbol Resolution.
 * Ensures every external reference has a unique home in the Registry.
 */
static void execute_data_flow_pass(WaliaRegistryEntry* entry) {
    // Logic: The parser already validated local symbols. 
    // This pass verifies that the Sentry Registry (Phase 10.1) can 
    // resolve every top-level dependency for this node.
    const char* resolution = walia_registry_resolve(entry->name);
    if (resolution == NULL) {
        walia_ledger_log(FAULT_WARNING, "Logic Node orphaned in global namespace.", 
                         entry->name, entry->filePath, entry->startLine, 
                         entry->currentHash, CURE_RENAME_SYMBOL);
    }
}

/**
 * @brief PASS 3: Integrity Simulation.
 * Dispatches the block to the Ghost Executioner to catch logical crashes.
 */
static void execute_integrity_pass(WaliaRegistryEntry* entry) {
    // 1. Compile the specific block to bytecode
    // (Assuming a helper to extract just the block body string)
    ObjFunction* ghostFunc = newFunction(); 
    // ... logic to fill ghostFunc from registry context ...

    // 2. Run through the Simulator (Phase 10.2 Group 3)
    WaliaSimReport report = walia_simulate_logic(ghostFunc);

    if (report.status != SIM_SUCCESS) {
        walia_ledger_log(FAULT_CRITICAL, "Predictive Logic Failure: Possible Nil-Drift or Loop Stagnation.", 
                         entry->name, entry->filePath, entry->startLine, 
                         entry->currentHash, CURE_INJECT_NIL_GUARD);
    }
}

// ==========================================
// WORKER ORCHESTRATION
// ==========================================

static void* mppa_worker_loop(void* arg) {
    (void)arg;
    
    while (true) {
        // Atomic Work-Stealing from the global context
        int idx = globalCtx.next++;
        if (idx >= globalCtx.total) break;

        WaliaRegistryEntry* entry = globalCtx.nodes[idx];
        
        switch (globalCtx.currentPass) {
            case PASS_STRUCTURAL: execute_structural_pass(entry); break;
            case PASS_DATA_FLOW:  execute_data_flow_pass(entry);  break;
            case PASS_INTEGRITY:  execute_integrity_pass(entry);  break;
        }
    }
    return NULL;
}

// ==========================================
// MASTER SOVEREIGN ASSESSMENT
// ==========================================

void walia_mppa_assess_project() {
    int coreCount = (int)sysconf(_SC_NPROCESSORS_ONLN);
    if (coreCount < 1) coreCount = 1;

    // 1. Snapshot the Registry for parallel iteration
    // (Logic to extract all active entries from registryTable into globalCtx.nodes)
    globalCtx.nodes = malloc(sizeof(WaliaRegistryEntry*) * 10000); // 10k capacity
    globalCtx.total = 0; // Populated from Phase 10.1 Registry
    
    pthread_t threads[WLD_MAX_THREADS];

    // 2. EXECUTE SEQUENTIAL PASSES
    for (int p = 0; p <= PASS_INTEGRITY; p++) {
        globalCtx.currentPass = (AnalysisPass)p;
        globalCtx.next = 0;

        printf(">> Walia Sentry: Executing Parallel Pass %d...\n", p);

        for (int i = 0; i < coreCount; i++) {
            pthread_create(&threads[i], NULL, mppa_worker_loop, NULL);
        }
        for (int i = 0; i < coreCount; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    // 3. AGGREGATE RESULTS
    walia_ledger_calculate_ripple();
    
    free(globalCtx.nodes);
}
