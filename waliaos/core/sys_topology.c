#include "../include/sys_topology.h"
#include "../include/walia_kernel_base.h"
#include "../include/driver_serial.h"
#include <string.h>

// Global Singleton
static WldCoreMap global_core_map;

// ==========================================
// ACPI BINARY STRUCTURES
// ==========================================

typedef struct {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;
} __attribute__((packed)) RSDPDescriptor;

typedef struct {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) ACPISDTHeader;

// ==========================================
// INTERNAL PROBE UTILITIES
// ==========================================

/**
 * @brief Searches for the RSDP signature in the BIOS memory regions.
 * Limits: 0xE0000 to 0xFFFFF.
 */
static RSDPDescriptor* find_rsdp() {
    uint8_t* ptr = (uint8_t*)0xE0000;
    while ((uintptr_t)ptr < 0x100000) {
        if (strncmp((char*)ptr, "RSD PTR ", 8) == 0) { // Fixed: Use strncmp for comparison
            // Checksum validation would happen here
            return (RSDPDescriptor*)ptr;
        }
        ptr += 16; // RSDP is always 16-byte aligned
    }
    return NULL;
}

/**
 * @brief Parses the Multiple APIC Description Table (MADT).
 */
static void parse_madt(ACPISDTHeader* madt) {
    uint8_t* ptr = (uint8_t*)madt + sizeof(ACPISDTHeader) + 8; // Skip header and APIC address
    uint8_t* end = (uint8_t*)madt + madt->length;

    while (ptr < end) {
        uint8_t entry_type = ptr[0];
        uint8_t entry_length = ptr[1];

        if (entry_type == 0) { // TYPE 0: Processor Local APIC
            uint8_t proc_id = ptr[2];
            uint8_t apic_id = ptr[3];
            uint32_t flags = *(uint32_t*)&ptr[4];

            if (flags & 1) { // If the core is actually enabled
                int idx = global_core_map.total_cores++;
                global_core_map.cores[idx].processor_id = proc_id;
                global_core_map.cores[idx].apic_id = apic_id;
                global_core_map.cores[idx].is_enabled = true;
                global_core_map.cores[idx].is_bootstrap = (idx == 0);
            }
        }
        ptr += entry_length;
    }
}

// ==========================================
// PUBLIC TOPOLOGY API
// ==========================================

void sys_topology_probe() {
    k_memset(&global_core_map, 0, sizeof(WldCoreMap));
    k_serial_print(">> Walia Topology: Commencing Hardware Discovery...\n");

    RSDPDescriptor* rsdp = find_rsdp();
    if (!rsdp) {
        k_serial_print("[FAULT] ACPI RSDP not found. Falling back to Single-Core mode.\n");
        global_core_map.total_cores = 1;
        return;
    }

    // Traverse RSDT to find MADT
    ACPISDTHeader* rsdt = (ACPISDTHeader*)(uintptr_t)rsdp->rsdt_address;
    int entries = (rsdt->length - sizeof(ACPISDTHeader)) / 4;
    uint32_t* table_ptrs = (uint32_t*)((uint8_t*)rsdt + sizeof(ACPISDTHeader));

    for (int i = 0; i < entries; i++) {
        ACPISDTHeader* table = (ACPISDTHeader*)(uintptr_t)table_ptrs[i];
        if (strncmp(table->signature, "APIC", 4) == 0) {
            parse_madt(table);
            break;
        }
    }

    if (global_core_map.total_cores == 0) {
        global_core_map.total_cores = 1; // Minimum Sovereignty
    }

    k_serial_print(">> Walia Topology: Discovery complete.\n");
}

int sys_topology_get_core_count() {
    return global_core_map.total_cores;
}
