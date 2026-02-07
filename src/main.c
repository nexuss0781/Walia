#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// --- SUBSTRATE CORE ---
#include "core/common.h"
#include "core/scanner.h"
#include "core/parser.h"
#include "analysis.h"
#include "core/ast.h"
#include "core/chunk.h"
#include "core/compiler.h"
#include "core/vm.h"
#include "persistence.h"
#include "telemetry.h"
#include "native.h"

// --- WALIADB PARALLEL ENGINE ---
#include "db/db_pager.h"
#include "db/db_pool.h"
#include "db/db_dispatch.h"
#include "lib/lib_collections.h"
#include "lib/lib_sql.h"
#include "lib/lib_nosql.h"
#include "lib/lib_vector.h"
#include "lib/lib_sqe.h"
#include "lib/lib_sentry.h"

// --- PHASE 10: SOVEREIGN INTELLIGENCE & DX ---
#include "tooling/tooling_hash_scanner.h"
#include "tooling/tooling_registry.h"
#include "tooling/tooling_history.h"
#include "tooling/tooling_ledger.h"
#include "tooling/tooling_mppa.h"
#include "tooling/tooling_simulator.h" 
#include "tooling/tooling_healer.h"
#include "tooling/tooling_report.h"
#include "tooling/tooling_nexus_main.h"
#include "tooling/tooling_oracle_lexer.h"
#include "tooling/tooling_oracle_runner.h"
#include "tooling/tooling_oracle_gen.h"
#include "test_runner.h"

// --- PHASE 17 & 18: ADVANCED PARADIGMS & FUSION ---
#include "sys/sys_hardware_probe.h"  // Fixed: Added for calibration
#include "vm/vm_entangle.h"         // Fixed: Added for quantum logic
#include "lib/lib_evolution.h"       // Fixed: Added for genetic engine
#include "lib/lib_iterator.h"
#include "lib/lib_trans_api.h"

// ==========================================
// GLOBAL SOVEREIGN SUBSTRATE
// ==========================================
WldPager*      global_pager = NULL; 
WldBufferPool  global_pool;  
WldDispatcher  global_dispatcher; 
bool           hadError = false; 

typedef struct {
    bool testMode;      
    bool verbose;       
    bool telemetry;     
    bool coldBoot;      
    bool nexusMode;     
    const char* path;   
} WaliaConfig;

static WaliaConfig config = { false, false, false, false, false, NULL };

static void printUsage() {
    printf("Walia Sovereign Engine v1.2.0 (Fusion Edition)\n");
    printf("Usage: walia [options] <script.wal>\n");
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) exit(EXIT_IO_ERROR);
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

static void runWalia() {
    // 1. PHYSICAL CALIBRATION
    sys_hardware_probe_init(); // Detects AVX-512 / Cores

    // 2. INTELLIGENCE BOOT
    walia_ledger_init();
    walia_registry_init();
    walia_history_init();
    walia_oracle_init();
    walia_entangle_init(); // Initialize Quantum substrate

    if (config.nexusMode) walia_nexus_start();

    // 3. PROJECT ASSESSMENT
    walia_scan_project("."); 
    walia_mppa_assess_project();
    walia_oracle_run_suite();

    WaliaDiagnosticLedger* ledger = walia_ledger_get_master();
    if (ledger->criticalCount > 0) {
        if (config.nexusMode) walia_nexus_stop();
        walia_report_flight_deck();
        exit(EXIT_COMPILE_ERROR);
    }

    // 4. COMPILATION
    char* source = readFile(config.path);
    int stmtCount = 0;
    Stmt** statements = parse(source, &stmtCount);
    
    Chunk chunk;
    initChunk(&chunk);
    if (!compile(statements, stmtCount, &chunk)) exit(EXIT_COMPILE_ERROR);

    // 5. SUBSTRATE FUSION
    initVM(); 
    global_pager = db_pager_open("walia.wld");
    db_pool_init(&global_pool, global_pager);
    
    // Initialize Standard API Bridge
    initNatives(); 
    initWaliaCollections(); 
    initSqlLibrary();
    initNoSqlLibrary(); 
    initVectorLibrary(); 
    initSqeLibrary(); 
    initSentryLibrary();
    initEvolutionLibrary(); // Genetic Engine Active
    initIteratorLibrary();
    initTranscoderLibrary();

    ObjFunction* scriptFunc = (ObjFunction*)newFunction();
    scriptFunc->chunk = chunk;

    // 6. EXECUTION
    if (config.testMode) {
        walia_run_tests(scriptFunc);
    } else {
        InterpretResult result = interpret(scriptFunc);
        if (result != INTERPRET_OK) exit(EXIT_RUNTIME_ERROR);
    }

    // 7. SHUTDOWN
    walia_oracle_generate_manual();
    freeVM(); 
    db_pager_close(global_pager);
    if (config.nexusMode) walia_nexus_stop();
}

extern void walia_repl_start();

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(); return 0;
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--nexus") == 0) {
            config.nexusMode = true;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
            config.testMode = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cold") == 0) {
            config.coldBoot = true;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--telemetry") == 0) {
            config.telemetry = true;
        } else {
            if (config.path == NULL) config.path = argv[i];
        }
    }

    if (config.path != NULL) runWalia();
    else walia_repl_start();

    return 0;
}
