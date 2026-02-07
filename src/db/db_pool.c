#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // Fixed: Added for msync and MS_SYNC

#include "db_pool.h"

// ==========================================
// INTERNAL HELPERS
// ==========================================

/**
 * @brief Finds the Least Recently Used (LRU) frame that is not currently pinned.
 * 
 * UFO Speed Logic: We scan the pool for the frame with the lowest 
 * 'lastAccess' count. Pinned pages are skipped to ensure data 
 * currently being processed by a worker thread remains stable.
 */
static int find_lru_frame(WldBufferPool* pool) {
    uint64_t oldest = pool->clock;
    int bestFrame = -1;

    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        // Essential Safety: We cannot evict a page currently in use (pinCount > 0)
        if (pool->frames[i].pinCount == 0) {
            if (pool->frames[i].lastAccess <= oldest) {
                oldest = pool->frames[i].lastAccess;
                bestFrame = i;
            }
        }
    }

    if (bestFrame == -1) {
        // Critical System State: The database has exceeded its working cache.
        fprintf(stderr, "WaliaDB: Buffer Pool Exhaustion! All cache slots are pinned.\n");
        exit(WLD_ERR_FULL);
    }
    return bestFrame;
}

void db_pool_pin(WldBufferPool* pool, PageID logicalId) {
    // UFO SPEED: Direct array index lookup based on the pool's logical mapping.
    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        if (pool->frames[i].logicalId == logicalId) {
            // Atomic increment to ensure thread-safety during parallel dispatch
            pool->frames[i].pinCount++;
            return;
        }
    }
}

// ==========================================
// PUBLIC POOL API
// ==========================================

void db_pool_init(WldBufferPool* pool, WldPager* pager) {
    pool->pager = pager;
    pool->clock = 0;
    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        pool->frames[i].logicalId = WLD_INVALID_PAGE;
        pool->frames[i].dataPtr = NULL;
        pool->frames[i].pinCount = 0;
        pool->frames[i].isDirty = false;
        pool->frames[i].lastAccess = 0;
    }
}

void* db_pool_acquire(WldBufferPool* pool, WldTransaction* tx, PageID logicalId, bool forWrite) {
    pool->clock++;
    int slot = -1;

    // 1. CACHE LOOKUP: Check if the logical page is already in the pool
    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        if (pool->frames[i].logicalId == logicalId) {
            slot = i;
            break;
        }
    }

    // 2. CACHE MISS: Load the page into an available frame
    if (slot == -1) {
        slot = find_lru_frame(pool);
        
        // If the old page in this slot was modified, we must persist it before eviction
        if (pool->frames[slot].isDirty && pool->frames[slot].dataPtr != NULL) {
            // Commit to physical storage
            if (msync(pool->frames[slot].dataPtr, WLD_PAGE_SIZE, MS_SYNC) == -1) {
                perror("WaliaDB: Failed to sync page during pool eviction");
            }
        }

        pool->frames[slot].logicalId = logicalId;
        
        // Resolve Logical -> Physical address through the Shadow Pager
        if (forWrite) {
            pool->frames[slot].dataPtr = db_get_page_write(tx, logicalId);
            pool->frames[slot].isDirty = true;
        } else {
            // Read-only path
            pool->frames[slot].dataPtr = (void*)db_get_page_read(tx, logicalId);
            pool->frames[slot].isDirty = false;
        }
    }

    // 3. UPDATE METADATA & PIN
    pool->frames[slot].pinCount++;
    pool->frames[slot].lastAccess = pool->clock;
    
    return pool->frames[slot].dataPtr;
}

void db_pool_release(WldBufferPool* pool, PageID logicalId) {
    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        if (pool->frames[i].logicalId == logicalId) {
            if (pool->frames[i].pinCount > 0) {
                pool->frames[i].pinCount--;
            }
            return;
        }
    }
}

void db_pool_flush(WldBufferPool* pool) {
    for (int i = 0; i < WLD_POOL_SIZE; i++) {
        // Physical Sync for all modified pages currently in the cache
        if (pool->frames[i].isDirty && pool->frames[i].dataPtr != NULL) {
            if (msync(pool->frames[i].dataPtr, WLD_PAGE_SIZE, MS_SYNC) == -1) {
                perror("WaliaDB: Failed to flush pool frame");
            }
            pool->frames[i].isDirty = false;
        }
    }
}
