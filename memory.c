#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "memory.h"
#include "vm.h"

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

    heapNext = heapStart;
    cardTableSize = WALIA_HEAP_SIZE / CARD_SIZE;
    cardTable = (uint8_t*)calloc(cardTableSize, sizeof(uint8_t));
}

void syncPersistentHeap() {
    if (heapStart != NULL) {
        msync(heapStart, WALIA_HEAP_SIZE, MS_SYNC);
    }
}

// ==========================================
// WRITE BARRIER (Card Marking)
// ==========================================

void markCard(void* pointer) {
    if (pointer < (void*)heapStart || pointer >= (void*)(heapStart + WALIA_HEAP_SIZE)) {
        return; 
    }
    size_t offset = (uint8_t*)pointer - heapStart;
    cardTable[offset / CARD_SIZE] = 1;
}

bool isCardDirty(void* pointer) {
    size_t offset = (uint8_t*)pointer - heapStart;
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

    // 2. Allocation path (Bump Pointer)
    if (pointer == NULL) {
        if (heapNext + newSize > heapStart + WALIA_HEAP_SIZE) {
            // In a production scenario, this triggers the GC
            fprintf(stderr, "Walia: Sovereign Heap Exhausted.\n");
            exit(1);
        }

        void* result = heapNext;
        heapNext += newSize;
        markCard(result);
        return result;
    }

    // 3. Reallocation path
    void* newPointer = reallocate(NULL, 0, newSize);
    size_t copySize = oldSize < newSize ? oldSize : newSize;
    memcpy(newPointer, pointer, copySize);
    
    return newPointer;
}
