#include <stdio.h>
#include <time.h>

#include "telemetry.h"
#include "vm.h"

// ==========================================
// INTERNAL STORAGE
// ==========================================
// We use a fixed-size array to ensure that monitoring the language 
// never triggers a memory allocation (which could trigger a GC).

static uint64_t counters[METRIC_SENTINEL];
static const char* metricNames[] = {
    "walia_instructions_total",
    "walia_gc_cycles_total",
    "walia_gc_reclaimed_bytes_total",
    "walia_effects_performed_total",
    "walia_heap_bytes_current",
    "walia_stack_depth_current"
};

static const char* metricHelp[] = {
    "Total instructions executed since boot",
    "Total garbage collection cycles triggered",
    "Total memory reclaimed by the GC",
    "Total algebraic effects performed",
    "Current size of the persistent heap file",
    "Current call frame depth"
};

// ==========================================
// CORE LOGIC
// ==========================================

void initTelemetry() {
    for (int i = 0; i < METRIC_SENTINEL; i++) {
        counters[i] = 0;
    }
}

void recordMetric(WaliaMetric metric, uint64_t value) {
    if (metric >= METRIC_SENTINEL) return;

    // Gauge vs Counter Logic
    if (metric == METRIC_HEAP_SIZE || metric == METRIC_STACK_DEPTH) {
        // These are Gauges (point-in-time values)
        counters[metric] = value;
    } else {
        // These are Counters (ever-increasing values)
        counters[metric] += value;
    }
}

void exportMetrics() {
    printf("\n# WALIA TELEMETRY EXPORT\n");
    printf("# Generated at: %ld\n", (long)time(NULL));

    for (int i = 0; i < METRIC_SENTINEL; i++) {
        // Prometheus Format:
        // # HELP metric_name description
        // # TYPE metric_name counter/gauge
        // metric_name value
        
        printf("# HELP %s %s\n", metricNames[i], metricHelp[i]);
        
        const char* type = (i == METRIC_HEAP_SIZE || i == METRIC_STACK_DEPTH) 
                           ? "gauge" : "counter";
        
        printf("# TYPE %s %s\n", metricNames[i], type);
        printf("%s %lu\n", metricNames[i], (unsigned long)counters[i]);
    }
    printf("# END WALIA EXPORT\n\n");
}
