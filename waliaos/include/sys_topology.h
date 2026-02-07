#ifndef WALIA_SYS_TOPOLOGY_H
#define WALIA_SYS_TOPOLOGY_H

#include "walia_kernel_base.h"

// ==========================================
// TOPOLOGY SPECIFICATIONS
// ==========================================

#define MAX_CORES 256 // Support up to 256 logical processors

/**
 * @brief Represents a physical CPU core in the Sovereign Map.
 */
typedef struct {
    uint8_t  processor_id;
    uint8_t  apic_id;
    bool     is_enabled;
    bool     is_bootstrap; // True if this is Core 0
} WldCore;

/**
 * @brief The Master Topology Map.
 */
typedef struct {
    WldCore  cores[MAX_CORES];
    int      total_cores;
    uint32_t local_apic_address; // Physical address of the interrupt controller
} WldCoreMap;

// ==========================================
// TOPOLOGY API
// ==========================================

/**
 * @brief Performs a hardware probe to map the CPU landscape.
 * 
 * Logic:
 * 1. Locates the Root System Description Pointer (RSDP).
 * 2. Traverses ACPI tables to find the MADT.
 * 3. Records all enabled Local APICs.
 */
void sys_topology_probe();

/**
 * @brief Returns the total number of logical cores detected.
 */
int sys_topology_get_core_count();

#endif // WALIA_SYS_TOPOLOGY_H
