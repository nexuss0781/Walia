#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tooling/tooling_report.h"
#include "../tooling/tooling_ui.h"
#include "../tooling/tooling_ledger.h"
#include "../tooling/tooling_registry.h"

// ==========================================
// IMPACT ANALYSIS (The Ripple Engine)
// ==========================================

WaliaHealthMetrics walia_report_analyze_impact() {
    WaliaDiagnosticLedger* ledger = walia_ledger_get_master();
    WaliaHealthMetrics metrics = {0, 0, 100};

    if (ledger->count == 0) return metrics;

    for (int i = 0; i < ledger->count; i++) {
        WaliaDiagnostic* d = &ledger->records[i];
        
        int dependents = (d->severity == FAULT_CRITICAL) ? 5 : 1;
        if (d->severity == FAULT_CRITICAL) metrics.totalBlocking += dependents;
        else metrics.totalRisky += dependents;
        
        d->rippleScore = dependents;
    }

    int penalty = (ledger->criticalCount * 10) + (ledger->warningCount * 2);
    metrics.healthPercentage = (penalty > 100) ? 0 : (100 - penalty);

    return metrics;
}

// ==========================================
// VISUAL RENDERING (The Cockpit)
// ==========================================

void walia_report_flight_deck() {
    WaliaDiagnosticLedger* ledger = walia_ledger_get_master();
    WaliaHealthMetrics metrics = walia_report_analyze_impact();

    printf(UI_CLEAR);
    printf("%s%s==========================================================\n", UI_BOLD, UI_CYAN);
    printf("  WALIA SOVEREIGN FLIGHT DECK : PROJECT INTEGRITY REPORT\n");
    printf("==========================================================%s\n", UI_RESET);
    
    printf("  Health Score: [%d%%] | %d Critical | %d Warnings\n", 
           metrics.healthPercentage, ledger->criticalCount, ledger->warningCount);
    printf("----------------------------------------------------------\n\n");

    for (int i = 0; i < ledger->count; i++) {
        WaliaDiagnostic* d = &ledger->records[i];
        const char* tag = (d->severity == FAULT_CRITICAL) ? "CRIT" : "WARN";
        const char* color = (d->severity == FAULT_CRITICAL) ? UI_RED : UI_YELLOW;

        // Corrected: All color identifiers now match tooling_ui.h
        printf("  %s[%-5s]%s %-15s | %-20s (L%d)\n", 
               color, tag, UI_NC, d->context, d->reason, d->line);
    }

    printf("\nAction: [C] Apply All Cures | [R] Re-Scan | [Q] Quit\n");
    printf("Walia Sentry > ");
}

void walia_report_show_cure(WaliaDiagnostic* d) {
    printf("\n%s=== PROPOSED SOVEREIGN CURE ===%s\n", UI_YELLOW, UI_RESET);
    printf("Target:  %s (%s:%d)\n", d->context, d->filePath, d->line);
    printf("Apply this cure? [y/N]: ");
}
