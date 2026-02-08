#include "../include/sys_pmmo.h"
#include "../include/sys_security.h"
#include "../include/driver_serial.h"

// THE SOVEREIGN DISK MAP
// A persistent table representing the physical state of the 16TB space.
static WldDiskEntry* master_disk_map;
static uint64_t      total_disk_sectors;

// ==========================================
// PMMO SUBSTRATE BOOTSTRAP
// ==========================================

void sys_pmmo_init() {
    k_serial_print(">> Walia PMMO: Mapping Physical Block Substrate...\n");

    // 1. Hardware Probe
    // In a full OS, this queries the PCI bus for the NVMe controller.
    // We assume the Master Disk Map is located at a fixed persistent offset.
    extern uint8_t* heapStart;
    master_disk_map = (WldDiskEntry*)(heapStart + (1024 * 1024 * 20)); // Offset 20MB
    
    // 2. Identify Disk Topology
    // Example: 2TB SSD = 536,870,912 Sectors
    total_disk_sectors = 536870912; 

    k_serial_print(">> Walia PMMO: Sovereign File System Online [Direct-to-Metal].\n");
}

// ==========================================
// THE JUMP-NOT-SEARCH LOOKUP (UFO SPEED)
// ==========================================

uintptr_t sys_pmmo_resolve_physical(uint32_t pageId) {
    // Logic: In Walia OS, there is no 'Searching' for a file.
    // The PageID is the direct index into the Master Disk Map.
    WldDiskEntry* entry = &master_disk_map[pageId];

    if (entry->sectorLBA == 0) {
        return 0; // Page not physically allocated
    }

    // Return the absolute hardware offset
    return (uintptr_t)(entry->sectorLBA * SECTOR_SIZE);
}

// ==========================================
// ATOMIC OBJECT INGRESS (Persistence)
// ==========================================

bool sys_pmmo_commit_page(uint32_t pageId, const void* buffer, uint32_t hash) {
    // 1. SECURITY AUDIT (Phase 23 Integration)
    // Before writing to the physical platter, the Bit-Shield must 
    // verify the integrity of the data.
    WldRoleFootprint fp; 
    k_memcpy(fp.fingerprint, &hash, 4); // Simplified hash footprint
    
    if (sys_security_audit_page(buffer, &fp) == BIT_TYPE_FOREIGNER) {
        k_serial_print("[ALERT] PMMO Reject: Attempted to write Foreigner bits to disk.\n");
        return false;
    }

    // 2. DISK FOLDING (Deduplication)
    // Check if another PageID is already using this exact logic hash.
    for (uint32_t i = 0; i < pageId; i++) {
        if (master_disk_map[i].logicHash == hash && master_disk_map[i].referenceCount > 0) {
            // UFO-GRADE OPTIMIZATION: 
            // We do not write to the disk. We point this page to the 
            // existing physical sector.
            master_disk_map[pageId].sectorLBA = master_disk_map[i].sectorLBA;
            master_disk_map[pageId].logicHash = hash;
            master_disk_map[pageId].referenceCount++;
            
            k_serial_print(">> Walia PMMO: Disk-Fold established. 0 bytes written.\n");
            return true;
        }
    }

    // 3. PHYSICAL HARDWARE WRITE
    // If logic is unique, we perform the physical I/O.
    // (Actual LBA allocation logic and NVMe OUT command goes here)
    
    master_disk_map[pageId].sectorLBA = pageId; // Simple mapping for MVP
    master_disk_map[pageId].logicHash = hash;
    master_disk_map[pageId].referenceCount = 1;

    // 4. ATOMIC CHECKPOINT
    // Mark the disk map as dirty for the Superblock commit.
    // markCard(&master_disk_map[pageId]); // TODO: Implement kernel-level page dirtying

    return true;
}
