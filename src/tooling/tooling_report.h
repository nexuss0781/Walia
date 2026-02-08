#ifndef WALIA_TOOLING_REPORT_H
#define WALIA_TOOLING_REPORT_H

#include "../core/common.h"
#include "../tooling/tooling_ledger.h"

// ==========================================
// RIPPLE METRICS
// ==========================================

typedef struct {
    int totalBlocking;     // Total functions currently inoperable
    int totalRisky;        // Functions dependent on a 'Warning' node
    int healthPercentage;  // 0-100 score of project logic integrity
} WaliaHealthMetrics;

// ==========================================
// REPORTING API
// ==========================================

/**
 * @brief Renders the Master Flight Report to the terminal.
 * 
 * Logic:
 * 1. Clears the HUD segment.
 * 2. Draws the Severity Summary (Critical/Warning/Info).
 * 3. Highlights the "Top 3 Ripple Blockers".
 */
void walia_report_flight_deck();

/**
 * @brief Performs deep impact analysis using the Sovereign Registry.
 * Fills the 'rippleScore' fields in the Ledger.
 */
WaliaHealthMetrics walia_report_analyze_impact();

/**
 * @brief Displays a "Cure Suggestion" card for a specific diagnostic.
 */
void walia_report_show_cure(WaliaDiagnostic* diagnostic);

#endif // WALIA_TOOLING_REPORT_H
