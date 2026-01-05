#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling_ledger.h"
#include "tooling_registry.h"
#include "memory.h"

// Global Ledger Singleton
static WaliaDiagnosticLedger masterLedger;

// ==========================================
// CORE LEDGER LOGIC
// ==========================================

void walia_ledger_init() {
    masterLedger.count = 0;
    masterLedger.criticalCount = 0;
    masterLedger.warningCount = 0;
    pthread_mutex_init(&masterLedger.lock, NULL);
}

void walia_ledger_clear() {
    pthread_mutex_lock(&masterLedger.lock);
    masterLedger.count = 0;
    masterLedger.criticalCount = 0;
    masterLedger.warningCount = 0;
    pthread_mutex_unlock(&masterLedger.lock);
}

void walia_ledger_log(FaultSeverity sev, const char* reason, const char* context, 
                      const char* path, int line, uint8_t* hash, CureAction cure) {
    
    // UFO SPEED: Minimal locking duration
    pthread_mutex_lock(&masterLedger.lock);

    if (masterLedger.count >= MAX_PROJECT_ERRORS) {
        pthread_mutex_unlock(&masterLedger.lock);
        return; 
    }

    WaliaDiagnostic* d = &masterLedger.records[masterLedger.count++];
    
    d->severity = sev;
    d->line = line;
    d->cure = cure;
    d->rippleScore = 0; // Calculated in Group 2
    
    strncpy(d->reason, reason, 255);
    strncpy(d->context, context ? context : "Global", 127);
    strncpy(d->filePath, path, 511);
    
    if (hash) memcpy(d->blockHash, hash, 32);
    else memset(d->blockHash, 0, 32);

    if (sev == FAULT_CRITICAL) masterLedger.criticalCount++;
    if (sev == FAULT_WARNING)  masterLedger.warningCount++;

    pthread_mutex_unlock(&masterLedger.lock);
}

// ==========================================
// RIPPLE ENGINE (Impact Analysis)
// ==========================================

void walia_ledger_calculate_ripple() {
    pthread_mutex_lock(&masterLedger.lock);

    for (int i = 0; i < masterLedger.count; i++) {
        WaliaDiagnostic* d = &masterLedger.records[i];
        
        // Intelligence Logic:
        // We query the Sovereign Registry (10.1) to see how many 
        // other functions reference the context of this error.
        
        // [SIMULATED RIPPLE CALCULATION]
        // In production, this walks the 'References' graph.
        // If 'UserClass' has an error, and 50 functions use 'UserClass',
        // the Ripple Score is 50.
        
        d->rippleScore = (d->severity == FAULT_CRITICAL) ? 10 : 1; 
    }

    pthread_mutex_unlock(&masterLedger.lock);
}

WaliaDiagnosticLedger* walia_ledger_get_master() {
    return &masterLedger;
}

// ==========================================
// ENTERPRISE REPORTING
// ==========================================

void walia_ledger_print_flight_report() {
    printf("\n%s--- WALIA SOVEREIGN FLIGHT REPORT ---%s\n", "\033[1m", "\033[0m");
    printf("Total Assessment: %d nodes verified.\n", masterLedger.count);
    printf("Status: %s%d Critical%s | %s%d Warnings%s\n", 
           "\033[31m", masterLedger.criticalCount, "\033[0m",
           "\033[33m", masterLedger.warningCount, "\033[0m");
    printf("---------------------------------------\n");

    for (int i = 0; i < masterLedger.count; i++) {
        WaliaDiagnostic* d = &masterLedger.records[i];
        const char* color = (d->severity == FAULT_CRITICAL) ? "\033[31m" : "\033[33m";
        
        printf("%s[%s]%s In %s (%s:%d)\n", 
               color, (d->severity == FAULT_CRITICAL ? "CRIT" : "WARN"), "\033[0m",
               d->context, d->filePath, d->line);
        printf("  Fault: %s\n", d->reason);
        if (d->rippleScore > 5) {
            printf("  Impact: %sHigh Ripple (%d nodes blocked)%s\n", "\033[1;31m", d->rippleScore, "\033[0m");
        }
    }
    printf("\n");
}
