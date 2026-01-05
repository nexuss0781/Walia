#ifndef WALIA_TOOLING_LEDGER_H
#define WALIA_TOOLING_LEDGER_H

#include "common.h"
#include <pthread.h>

// ==========================================
// DIAGNOSTIC PROTOCOL
// ==========================================

typedef enum {
    FAULT_CRITICAL, // Blocks bytecode generation
    FAULT_WARNING,  // Logic risk (Shadowing, Nil-drift)
    FAULT_INFO,     // Style/Optimization hint
    FAULT_REPAIRED  // Successfully cured by the Healer
} FaultSeverity;

typedef enum {
    CURE_NONE,
    CURE_RENAME_SYMBOL,
    CURE_INJECT_NIL_GUARD,
    CURE_PRUNE_DEAD_CODE,
    CURE_REALIGN_SIGNATURE
} CureAction;

/**
 * @brief An immutable record of a logical or syntactic fault.
 */
typedef struct {
    FaultSeverity severity;
    char reason[256];
    char context[128];   // The parent Function/Class name
    char filePath[512];
    int  line;
    int  tokenStart;
    int  tokenEnd;
    uint8_t blockHash[32]; // Link to Phase 10.1 History
    int  rippleScore;      // Number of dependent nodes affected
    CureAction cure;       // Suggested autonomous repair
} WaliaDiagnostic;

// ==========================================
// THE UNIVERSAL LEDGER (Knowledge Base)
// ==========================================

#define MAX_PROJECT_ERRORS 4096

typedef struct {
    WaliaDiagnostic records[MAX_PROJECT_ERRORS];
    int count;
    int criticalCount;
    int warningCount;
    pthread_mutex_t lock; // Thread-safety for parallel workers
} WaliaDiagnosticLedger;

// ==========================================
// LEDGER API
// ==========================================

/**
 * @brief Initializes the project-wide ledger.
 */
void walia_ledger_init();

/**
 * @brief Records a fault into the ledger.
 * Thread-safe: Multiple parallel workers can log simultaneously.
 */
void walia_ledger_log(FaultSeverity sev, const char* reason, const char* context, 
                      const char* path, int line, uint8_t* hash, CureAction cure);

/**
 * @brief Calculates the "Ripple Effect" of all current errors.
 * Ranks errors by how many functions they block across the registry.
 */
void walia_ledger_calculate_ripple();

/**
 * @brief Retrieves the full ledger for the Visual HUD.
 */
WaliaDiagnosticLedger* walia_ledger_get_master();

/**
 * @brief Resets the ledger for a fresh analysis pass.
 */
void walia_ledger_clear();

#endif // WALIA_TOOLING_LEDGER_H
