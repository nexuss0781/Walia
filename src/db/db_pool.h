#ifndef WALIA_DB_POOL_H
#define WALIA_DB_POOL_H

#include "db_common.h"
#include "db_shadow.h"

// ==========================================
// POOL CONFIGURATION
// ==========================================
#define WLD_POOL_SIZE 1024 // Number of logical pages to track in active cache

// ==========================================
// THE POOL FRAME
// ==========================================
// Tracks a single logical page currently in use by the engine.

typedef struct {
    PageID logicalId;
    void*  dataPtr;      // Direct pointer to the mapped memory
    int    pinCount;     // Number of active threads using this page
    bool   isDirty;      // Has this page been modified?
    uint64_t lastAccess; // For LRU (Least Recently Used) replacement
} WldPoolFrame;

typedef struct {
    WldPager* pager;
    WldPoolFrame frames[WLD_POOL_SIZE];
    uint64_t clock;      // Monotonic counter for LRU
} WldBufferPool;

// ==========================================
// BUFFER POOL API
// ==========================================

/**
 * @brief Initializes the Buffer Pool for a specific .wld file.
 */
void db_pool_init(WldBufferPool* pool, WldPager* pager);

/**
 * @brief Retrieves a page from the pool.
 * 
 * Logic:
 * 1. Checks if logicalId is already in frames[].
 * 2. If not, finds an empty slot or evicts the LRU page.
 * 3. Resolves the Logical->Physical address via the Shadow Table.
 * 4. Pins the page.
 */
void* db_pool_acquire(WldBufferPool* pool, WldTransaction* tx, PageID logicalId, bool forWrite);

/**
 * @brief Decrements the access count for a page.
 * Allows the pool to eventually evict this page if memory is needed.
 */
void db_pool_release(WldBufferPool* pool, PageID logicalId);

/**
 * @brief Flushes all dirty pages in the pool to disk.
 */
void db_pool_flush(WldBufferPool* pool);

/**
 * @brief Pins a page in the pool to prevent eviction.
 */
void db_pool_pin(WldBufferPool* pool, PageID logicalId);

extern WldBufferPool global_pool;

#endif // WALIA_DB_POOL_H
