#ifndef WALIA_TOOLING_HEALER_H
#define WALIA_TOOLING_HEALER_H

#include "core/common.h"
#include "tooling/tooling_ledger.h"

// ==========================================
// HEALER API
// ==========================================

/**
 * @brief Orchestrates the repair of a specific diagnostic fault.
 * 
 * @param diagnostic The fault record from the Ledger.
 * @return bool True if the logic was successfully cured on disk.
 */
bool walia_healer_apply_cure(WaliaDiagnostic* diagnostic);

/**
 * @brief Performs a project-wide refactor to realign function calls.
 * 
 * Triggered when a function signature changes. Uses the Sovereign Registry 
 * to find all call-sites across 60k+ files and updates them.
 */
void walia_healer_realign_signature(const char* symbolName, int newArity);

/**
 * @brief Injects a safety guard (e.g., Nil-check) into a specific line.
 */
void walia_healer_inject_guard(const char* path, int line, const char* guardCode);

#endif // WALIA_TOOLING_HEALER_H
