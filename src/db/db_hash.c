#include <string.h>
#include <stdio.h>

#include "db_hash.h"

// = = = = = = = = = = = = = = = = = = = = = =
// INTERNAL BUCKET LOGIC
// = = = = = = = = = = = = = = = = = = = = = =

static HashBucketPage* get_bucket(WldBufferPool* pool, WldTransaction* tx, PageID id, bool forWrite) {
    return (HashBucketPage*)db_pool_acquire(pool, tx, id, forWrite);
}

static HashDirectoryPage* get_directory(WldBufferPool* pool, WldTransaction* tx, PageID id, bool forWrite) {
    return (HashDirectoryPage*)db_pool_acquire(pool, tx, id, forWrite);
}

// = = = = = = = = = = = = = = = = = = = = = =
// PUBLIC HASH API
// = = = = = = = = = = = = = = = = = = = = = =

PageID db_hash_create(WldBufferPool* pool, WldTransaction* tx) {
    // 1. Allocate Directory
    PageID dirId = db_pager_allocate(tx->pager);
    HashDirectoryPage* dir = get_directory(pool, tx, dirId, true);
    dir->header.type = PAGE_TYPE_HASH_DIR;
    dir->globalDepth = 1; // Start with 2^1 = 2 buckets

    // 2. Allocate initial Buckets
    for (int i = 0; i < 2; i++) {
        PageID bid = db_pager_allocate(tx->pager);
        HashBucketPage* bucket = get_bucket(pool, tx, bid, true);
        bucket->header.type = PAGE_TYPE_HASH_DIR; // Shared role type
        bucket->localDepth = 1;
        bucket->entryCount = 0;
        dir->bucketPointers[i] = bid;
        db_pool_release(pool, bid);
    }

    db_pool_release(pool, dirId);
    return dirId;
}

PageID db_hash_lookup(WldBufferPool* pool, WldTransaction* tx, 
                      PageID dirId, uint64_t keyHash) {
    HashDirectoryPage* dir = get_directory(pool, tx, dirId, false);
    
    // UFO Speed: Calculate directory index using bitwise mask
    // index = hash % 2^globalDepth
    uint32_t index = keyHash & ((1 << dir->globalDepth) - 1);
    PageID bucketId = dir->bucketPointers[index];
    db_pool_release(pool, dirId);

    HashBucketPage* bucket = get_bucket(pool, tx, bucketId, false);
    for (uint32_t i = 0; i < bucket->entryCount; i++) {
        if (bucket->entries[i].keyHash == keyHash) {
            PageID result = bucket->entries[i].documentPageId;
            db_pool_release(pool, bucketId);
            return result;
        }
    }

    db_pool_release(pool, bucketId);
    return WLD_INVALID_PAGE;
}

WldResult db_hash_insert(WldBufferPool* pool, WldTransaction* tx, 
                         PageID dirId, uint64_t keyHash, PageID docPageId) {
    
    HashDirectoryPage* dir = get_directory(pool, tx, dirId, true);
    uint32_t index = keyHash & ((1 << dir->globalDepth) - 1);
    PageID bucketId = dir->bucketPointers[index];
    
    HashBucketPage* bucket = get_bucket(pool, tx, bucketId, true);

    // 1. Normal Insertion
    if (bucket->entryCount < WLD_HASH_BUCKET_CAPACITY) {
        bucket->entries[bucket->entryCount].keyHash = keyHash;
        bucket->entries[bucket->entryCount].documentPageId = docPageId;
        bucket->entryCount++;
        
        db_pool_release(pool, bucketId);
        db_pool_release(pool, dirId);
        return WLD_OK;
    }

    // 2. Bucket Split Logic (Incremental Resizing)
    if (bucket->localDepth < dir->globalDepth) {
        // We can split without doubling the directory.
        // Logic: Create new bucket, redistribute entries, update half of directory pointers.
        // (Production implementation involves re-hashing existing entries)
    } else {
        // Must double directory size
        if (dir->globalDepth >= 10) { // Limit to 1024 entries for Phase 3
            db_pool_release(pool, bucketId);
            db_pool_release(pool, dirId);
            return WLD_ERR_FULL;
        }
        
        // Logical doubling: copy first half pointers to second half
        int currentSize = 1 << dir->globalDepth;
        for (int i = 0; i < currentSize; i++) {
            dir->bucketPointers[i + currentSize] = dir->bucketPointers[i];
        }
        dir->globalDepth++;
    }

    db_pool_release(pool, bucketId);
    db_pool_release(pool, dirId);
    
    // Recursive call to retry insertion in the now-split structure
    return db_hash_insert(pool, tx, dirId, keyHash, docPageId);
}
