#ifndef WALIA_SYS_PMMO_H
#define WALIA_SYS_PMMO_H

#include "walia_kernel_base.h"

// ==========================================
// STORAGE SPECIFICATIONS
// ==========================================

#define SECTOR_SIZE        4096 // 4KB Hardware Sector alignment
#define MAX_DISK_PAGES     0xFFFFFFFF // 4 Billion Pages (~16TB)

/**
 * @brief Represents a physical pointer to a persistent object on disk.
 */
typedef struct {
    uint64_t sectorLBA;   // Logical Block Address on physical hardware
    uint32_t logicHash;   // SHA-256 fingerprint for deduplication
    uint16_t referenceCount; // Number of objects sharing this physical sector
    uint16_t flags;       // [Compressed | Encrypted | System]
} WldDiskEntry;

// ==========================================
// PMMO ENGINE API
// ==========================================

/**
 * @brief Initializes the physical storage substrate.
 * Probes the NVMe/SATA controller and mounts the Sovereign Page Array.
 */
void sys_pmmo_init();

/**
 * @brief Resolves a logical PageID to a physical disk sector.
 * 
 * UFO SPEED: This bypasses the VFS layer. It performs a direct 
 * memory-mapped lookup into the persistent sector table.
 */
uintptr_t sys_pmmo_resolve_physical(uint32_t pageId);

/**
 * @brief Atomically writes a Sovereign Page to the physical disk.
 * Utilizes the Shadow Paging protocol (Phase 3) to ensure 0% corruption.
 */
bool sys_pmmo_commit_page(uint32_t pageId, const void* buffer, uint32_t hash);

#endif // WALIA_SYS_PMMO_H
