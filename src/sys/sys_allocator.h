#ifndef WALIA_SYS_ALLOCATOR_H
#define WALIA_SYS_ALLOCATOR_H

#include "core/common.h"

// ==========================================
// ALLOCATOR SPECIFICATIONS
// ==========================================

#define WLD_MEM_ALIGNMENT 8
#define WLD_MIN_BLOCK_SIZE 16

/**
 * @brief Header for a manually managed memory block.
 * Resides in the persistent heap, immediately preceding the data.
 */
typedef struct WldMemBlock {
    uint32_t size;            // Size of the data payload
    bool     isFree;          // True if available for reuse
    struct WldMemBlock* next; // Persistent link to next block
    struct WldMemBlock* prev; // Persistent link for coalescing
} WldMemBlock;

// ==========================================
// SOVEREIGN ALLOCATOR API
// ==========================================

/**
 * @brief Initializes the manual allocation arena.
 * carves a dedicated segment out of the persistent heap.
 */
void sys_allocator_init();

/**
 * @brief Allocates a contiguous block of raw bytes.
 * 
 * Logic:
 * 1. Performs a 'Best-Fit' search on the free-list.
 * 2. If a large block is found, it splits it to minimize waste.
 * 3. Marks the segment as 'Dirty' to ensure persistence.
 * 
 * @return void* A raw pointer to the data segment.
 */
void* sys_alloc(size_t size);

/**
 * @brief Releases a manually allocated block.
 * 
 * Logic:
 * 1. Marks block as free.
 * 2. Immediate Coalescing: Merges with adjacent free blocks to prevent fragmentation.
 */
void sys_release(void* ptr);

/**
 * @brief Returns the physical byte-size of a persistent block.
 */
size_t sys_sizeof(void* ptr);

#endif // WALIA_SYS_ALLOCATOR_H
