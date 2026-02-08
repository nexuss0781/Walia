#ifndef WALIA_TOOLING_MPPA_H
#define WALIA_TOOLING_MPPA_H

#include "../core/common.h"
#include "../tooling/tooling_registry.h"

// ==========================================
// MPPA PASS DEFINITIONS
// ==========================================

typedef enum {
    PASS_STRUCTURAL, // Syntax and block boundary validation
    PASS_DATA_FLOW,  // Verification of cross-file variable usage
    PASS_INTEGRITY   // Logic simulation and contract matching
} AnalysisPass;

// ==========================================
// PARALLEL ORCHESTRATOR API
// ==========================================

/**
 * @brief Performs a Project-Wide Sovereign Assessment.
 * 
 * Logic:
 * 1. Spawns threads proportional to CPU cores.
 * 2. Distributes logic blocks (from Phase 10.1 Registry).
 * 3. Executes multiple passes sequentially.
 * 4. Fills the Diagnostic Ledger (Group 1).
 */
void walia_mppa_assess_project();

/**
 * @brief Analyzes a specific logic block for the current pass.
 */
void walia_mppa_analyze_node(WaliaRegistryEntry* entry, AnalysisPass pass);

#endif // WALIA_TOOLING_MPPA_H/
