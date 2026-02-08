#ifndef WALIA_SYS_HARDWARE_PROBE_H
#define WALIA_SYS_HARDWARE_PROBE_H

#include "../core/common.h"

// ==========================================
// TOPOLOGY SPECIFICATIONS
// ==========================================

typedef struct {
    int coreCount;
    int l1CacheSize;
    int l2CacheSize;
    int l3CacheSize;
    int cacheLineSize; // Usually 64 bytes
    
    bool hasAVX512;
    bool hasNEON;
} WldHardwareTopology;

/**
 * @brief Global pointer to the detected system topology.
 */
extern WldHardwareTopology globalTopology;

// ==========================================
// HARDWARE SENTRY API
// ==========================================

/**
 * @brief Performs a 'Cold-Start' probe of the physical CPU.
 * 
 * Logic:
 * 1. Executes CPUID (x64) or parses sysfs (ARM).
 * 2. Populates the global topology structure.
 * 3. Adjusts Pager batch-sizes to fit within the L3 cache.
 */
void sys_hardware_probe_init();

/**
 * @brief Returns the optimal alignment for a memory allocation.
 * Defaults to 64 (Cache Line Size).
 */
static inline size_t sys_get_optimal_alignment() {
    return (globalTopology.cacheLineSize > 0) ? globalTopology.cacheLineSize : 64;
}

#endif // WALIA_SYS_HARDWARE_PROBE_H
