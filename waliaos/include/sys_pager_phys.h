#ifndef WALIA_SYS_PAGER_PHYS_H
#define WALIA_SYS_PAGER_PHYS_H

#include "walia_kernel_base.h"

// ==========================================
// PHYSICAL RAM SPECIFICATIONS
// ==========================================

#define PAGE_SIZE          4096
#define RAM_2GB_PAGES      524288 // (2 * 1024 * 1024 * 1024) / 4096

/**
 * @brief Represents a physical frame of RAM.
 * 
 * Architecture:
 * This is the foundation of the 'Infinite Memory' model.
 * Each 4KB frame tracks its 'Logic Hash' for deduplication.
 */
typedef struct {
    uint32_t logic_hash_prefix; // First 32-bits of SHA-256 (Phase 10.1)
    uint16_t reference_count;   // For folding: how many virtual pages point here?
    uint16_t flags;             // [Dirty | Compressed | Sovereign]
} WldRAMFrame;

// ==========================================
// PHYSICAL PAGER API
// ==========================================

/**
 * @brief Maps the physical 2GB address space.
 */
void sys_pager_phys_init();

/**
 * @brief Requests a physical RAM frame for a logical PageID.
 * 
 * UFO SPEED Logic:
 * 1. Checks if a frame with the same 'Logic Hash' already exists.
 * 2. If Yes: Returns the existing frame (Folding).
 * 3. If No: Allocates a new frame from the free-list.
 */
uintptr_t sys_pager_allocate_frame(uint32_t logicHashPrefix);

/**
 * @brief Marks a frame as 'Cold' for the SIMD Compression engine.
 */
void sys_pager_mark_cold(uintptr_t physicalAddr);

#endif // WALIA_SYS_PAGER_PHYS_H
