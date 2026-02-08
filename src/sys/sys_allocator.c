#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sys/sys_allocator.h"
#include "../core/memory.h"

// ==========================================
// INTERNAL ARENA STATE
// ==========================================

static WldMemBlock* freeListHead = NULL;
static uint8_t* arenaStart = NULL;

void sys_allocator_init() {
    // Carve the first 10MB of the persistent heap for manual systems work
    // In production, this range is defined in the Superblock.
    extern uint8_t* heapStart;
    arenaStart = heapStart + (1024 * 1024); // Offset past VM metadata
    
    freeListHead = (WldMemBlock*)arenaStart;
    freeListHead->size = (1024 * 1024 * 10) - sizeof(WldMemBlock);
    freeListHead->isFree = true;
    freeListHead->next = NULL;
    freeListHead->prev = NULL;

    printf(">> Walia: Manual Sovereign Allocator online [10MB Arena]\n");
}

// ==========================================
// CORE ALLOCATION LOGIC (Best-Fit)
// ==========================================

void* sys_alloc(size_t size) {
    // 1. Align the size for CPU efficiency (8-byte boundary)
    size = (size + WLD_MEM_ALIGNMENT - 1) & ~(WLD_MEM_ALIGNMENT - 1);
    
    WldMemBlock* current = freeListHead;
    WldMemBlock* bestBlock = NULL;

    // 2. Search for the most efficient fit
    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            if (bestBlock == NULL || current->size < bestBlock->size) {
                bestBlock = current;
                if (current->size == size) break; // Perfect match
            }
        }
        current = current->next;
    }

    if (bestBlock == NULL) {
        fprintf(stderr, "Walia Error: Manual Arena Out of Memory.\n");
        return NULL;
    }

    // 3. SPLIT LOGIC: If the block is significantly larger, split it.
    if (bestBlock->size > size + sizeof(WldMemBlock) + WLD_MIN_BLOCK_SIZE) {
        WldMemBlock* newBlock = (WldMemBlock*)((uint8_t*)bestBlock + sizeof(WldMemBlock) + size);
        newBlock->size = bestBlock->size - size - sizeof(WldMemBlock);
        newBlock->isFree = true;
        newBlock->next = bestBlock->next;
        newBlock->prev = bestBlock;

        if (bestBlock->next) bestBlock->next->prev = newBlock;
        bestBlock->next = newBlock;
        bestBlock->size = (uint32_t)size;
    }

    bestBlock->isFree = false;
    
    // 4. Persistence: Mark both the metadata and the start of data as dirty
    markCard(bestBlock);
    
    // Return pointer to data segment (after the header)
    return (void*)((uint8_t*)bestBlock + sizeof(WldMemBlock));
}

// ==========================================
// RELEASE & COALESCING
// ==========================================

void sys_release(void* ptr) {
    if (ptr == NULL) return;

    // Move back to find the header
    WldMemBlock* block = (WldMemBlock*)((uint8_t*)ptr - sizeof(WldMemBlock));
    block->isFree = true;

    // 1. Merge with NEXT block if it is free
    if (block->next && block->next->isFree) {
        block->size += sizeof(WldMemBlock) + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }

    // 2. Merge with PREVIOUS block if it is free
    if (block->prev && block->prev->isFree) {
        WldMemBlock* p = block->prev;
        p->size += sizeof(WldMemBlock) + block->size;
        p->next = block->next;
        if (block->next) block->next->prev = p;
        block = p; // Pointer now refers to the unified larger block
    }

    markCard(block);
}

size_t sys_sizeof(void* ptr) {
    if (ptr == NULL) return 0;
    WldMemBlock* block = (WldMemBlock*)((uint8_t*)ptr - sizeof(WldMemBlock));
    return (size_t)block->size;
}
