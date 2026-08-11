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
#ifdef WALIA_KERNEL

// The freestanding kernel build shares VM/memory sources but does not link
// the host telemetry exporter. Keep instrumentation source-compatible while
// making it a zero-cost no-op in the kernel image.
static inline void initTelemetry(void) {}
static inline void setTelemetryEnabled(bool enabled) { UNUSED(enabled); }
static inline bool isTelemetryEnabled(void) { return false; }
static inline void recordMetric(WaliaMetric metric, uint64_t value) {
    UNUSED(metric);
    UNUSED(value);
}
static inline void exportMetrics(void) {}

#else

/**
 * @brief Initializes the telemetry subsystem.
 * Clears the internal ring buffer and resets counters.
 */
void initTelemetry();

/**
 * @brief Enables or disables counter collection without changing VM behavior.
 */
void setTelemetryEnabled(bool enabled);

/**
 * @brief Returns whether telemetry counters are currently being collected.
 */
bool isTelemetryEnabled();

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

#endif

#endif // WALIA_TELEMETRY_H
