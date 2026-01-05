#ifndef WALIA_MEMORY_H
#define WALIA_MEMORY_H

#include "common.h"

// ==========================================
// PERSISTENT HEAP CONFIGURATION
// ==========================================
#define WALIA_HEAP_FILE "walia.state"
#define WALIA_HEAP_SIZE (128 * 1024 * 1024) // 128 MB Persistent Heap
#define CARD_SIZE 512                       // Bytes per card for the Write Barrier

// ==========================================
// MEMORY GROWTH MACROS
// ==========================================

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)

// ==========================================
// PERSISTENCE & GC API
// ==========================================

// Initializes the Walia heap by mapping it to a file on disk.
void initPersistentHeap();

// Flushes all "dirty" pages from RAM to the physical .state file.
void syncPersistentHeap();

// The central allocator. Now carves memory from the Persistent Segment.
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

// WRITE BARRIER: Marks a memory region as "Dirty" (modified).
// Crucial for both Persistence and Generational GC.
void markCard(void* pointer);

// Checks if a card is dirty.
bool isCardDirty(void* pointer);

// Clears all dirty flags (usually called after a sync or GC cycle).
void clearCards();

#endif // WALIA_MEMORY_H
