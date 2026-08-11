#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef WALIA_KERNEL
#include <sys/stat.h>
#endif

#include "memory.h"
#include "vm.h"
#include "../telemetry.h"

// ==========================================
// GLOBAL PERSISTENT STATE
// ==========================================

/**
 * @brief The base address of the Walia Sovereign Heap.
 * Defined globally to allow the Persistence Engine to map the Superblock.
 */
uint8_t* heapStart = NULL; 

/**
 * @brief Internal heap trackers.
 */
static uint8_t* heapNext = NULL;  // Bump pointer for Nursery
static uint8_t* cardTable = NULL; // Write Barrier bitset
static size_t cardTableSize;

// ==========================================
// HEAP INITIALIZATION
// ==========================================

void initPersistentHeap() {
    // 1. Create/Open the Walia state image
    int fd = open(WALIA_HEAP_FILE, O_RDWR | O_CREAT, 0600);
    if (fd == -1) {
        perror("Walia: Critical IO failure opening heap");
        exit(EXIT_IO_ERROR);
    }

    // 2. Sizing
    bool freshState = false;
#ifndef WALIA_KERNEL
    struct stat stateInfo;
    freshState = (fstat(fd, &stateInfo) == 0 && stateInfo.st_size == 0);
#endif
    if (ftruncate(fd, WALIA_HEAP_SIZE) == -1) {
        perror("Walia: Failed to size persistent segment");
        exit(EXIT_IO_ERROR);
    }

    // 3. Mapping
    heapStart = mmap(NULL, WALIA_HEAP_SIZE, PROT_READ | PROT_WRITE, 
                     MAP_SHARED, fd, 0);
    
    if (heapStart == MAP_FAILED) {
        perror("Walia: mmap failure");
        exit(EXIT_IO_ERROR);
    }

    close(fd);

    if (freshState) {
        // Newly extended file mappings are logically zero-filled, but explicit
        // initialization is required for deterministic checkpoint diagnostics.
        memset(heapStart, 0, WALIA_HEAP_SIZE);
    }

    heapNext = heapStart;
    cardTableSize = WALIA_HEAP_SIZE / CARD_SIZE;
    cardTable = (uint8_t*)calloc(cardTableSize, sizeof(uint8_t));
}

void syncPersistentHeap() {
    if (heapStart != NULL && heapNext != NULL && heapNext > heapStart) {
        size_t initializedBytes = (size_t)(heapNext - heapStart);
        msync(heapStart, initializedBytes, MS_SYNC);
    }
}

// ==========================================
// WRITE BARRIER (Card Marking)
// ==========================================

void markCard(void* pointer) {
    if (pointer == NULL || heapStart == NULL || cardTable == NULL) {
        return;
    }

    uintptr_t address = (uintptr_t)pointer;
    uintptr_t heapBase = (uintptr_t)heapStart;
    uintptr_t heapLimit = heapBase + WALIA_HEAP_SIZE;
    if (address < heapBase || address >= heapLimit) {
        return;
    }

    size_t offset = (size_t)(address - heapBase);
    cardTable[offset / CARD_SIZE] = 1;
}

bool isCardDirty(void* pointer) {
    if (pointer == NULL || heapStart == NULL || cardTable == NULL) {
        return false;
    }

    uintptr_t address = (uintptr_t)pointer;
    uintptr_t heapBase = (uintptr_t)heapStart;
    uintptr_t heapLimit = heapBase + WALIA_HEAP_SIZE;
    if (address < heapBase || address >= heapLimit) {
        return false;
    }

    size_t offset = (size_t)(address - heapBase);
    return cardTable[offset / CARD_SIZE] == 1;
}

void clearCards() {
    if (cardTable) memset(cardTable, 0, cardTableSize);
}

// ==========================================
// SOVEREIGN ALLOCATOR
// ==========================================

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    // 1. Deallocation path
    if (newSize == 0) {
        return NULL;
    }

    if (heapStart == NULL || heapNext == NULL) {
        fprintf(stderr, "Walia: Persistent heap is not initialized.\n");
        exit(EXIT_IO_ERROR);
    }

    // 2. Allocation path (cache-line aligned bump pointer)
    if (pointer == NULL) {
        size_t alignedSize = (newSize + (WALIA_ALLOC_ALIGNMENT - 1)) &
                             ~(size_t)(WALIA_ALLOC_ALIGNMENT - 1);
        size_t used = (size_t)(heapNext - heapStart);
        if (used > WALIA_HEAP_SIZE || alignedSize > WALIA_HEAP_SIZE - used) {
            fprintf(stderr, "Walia: Sovereign Heap Exhausted.\n");
            exit(EXIT_IO_ERROR);
        }

        void* result = heapNext;
        memset(result, 0, alignedSize);
        heapNext += alignedSize;
        markCard(result);
        recordMetric(METRIC_HEAP_SIZE, (uint64_t)(heapNext - heapStart));
        return result;
    }

    // 3. Reallocation path
    void* newPointer = reallocate(NULL, 0, newSize);
    size_t copySize = oldSize < newSize ? oldSize : newSize;
    memcpy(newPointer, pointer, copySize);
    
    return newPointer;
}
