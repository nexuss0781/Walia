#ifndef WALIA_TELEMETRY_H
#define WALIA_TELEMETRY_H

#include "core/common.h"

// ==========================================
// METRIC CATEGORIES
// ==========================================

typedef enum {
    METRIC_INST_COUNT,    // Total instructions executed
    METRIC_GC_COUNT,      // Number of GC cycles
    METRIC_GC_BYTES,      // Total bytes reclaimed
    METRIC_EFFECT_COUNT,  // Number of algebraic effects performed
    METRIC_HEAP_SIZE,     // Current size of the persistent heap
    METRIC_STACK_DEPTH,   // Current call stack depth
    METRIC_SENTINEL       // Total count
} WaliaMetric;

// ==========================================
// TELEMETRY API
// ==========================================

/**
 * @brief Initializes the telemetry subsystem.
 * Clears the internal ring buffer and resets counters.
 */
void initTelemetry();

/**
 * @brief Records an occurrence of a specific metric.
 * 
 * @param metric The category to update.
 * @param value The amount to increment/update.
 */
void recordMetric(WaliaMetric metric, uint64_t value);

/**
 * @brief Exports current VM state in Prometheus-compatible text format.
 * 
 * Usage: Can be called via a native Walia function or a dedicated 
 * telemetry endpoint in a cloud environment.
 */
void exportMetrics();

#endif // WALIA_TELEMETRY_H
