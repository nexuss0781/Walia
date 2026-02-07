#ifndef WALIA_DB_HASH_H
#define WALIA_DB_HASH_H

#include "db_common.h"
#include "db_pool.h"

// ==========================================
// HASH ENGINE CONFIGURATION
// ==========================================

// A Bucket stores [KeyHash][LogicalPageID]. 
// 4096 bytes - header / 12 bytes per entry = ~330 entries per bucket.
#define WLD_HASH_BUCKET_CAPACITY 330

// The Directory stores PageIDs of buckets.
// 2^10 = 1024 entries per directory page.
#define WLD_HASH_DIR_CAPACITY 1024

typedef struct {
    uint64_t keyHash;
    PageID   documentPageId; // Pointer to the Slotted Page containing the document
} HashEntry;

typedef struct {
    WldPageHeader header;
    uint32_t      localDepth;
    uint32_t      entryCount;
    HashEntry     entries[WLD_HASH_BUCKET_CAPACITY];
} HashBucketPage;

typedef struct {
    WldPageHeader header;
    uint32_t      globalDepth;
    PageID        bucketPointers[WLD_HASH_DIR_CAPACITY];
} HashDirectoryPage;

// ==========================================
// HASH ENGINE API
// ==========================================

/**
 * @brief Initializes a new Extendible Hash Index.
 * @return PageID The logical ID of the Directory Page.
 */
PageID db_hash_create(WldBufferPool* pool, WldTransaction* tx);

/**
 * @brief Performs an O(1) document lookup by key hash.
 * @return PageID The ID of the page where the document resides.
 */
PageID db_hash_lookup(WldBufferPool* pool, WldTransaction* tx, 
                      PageID dirId, uint64_t keyHash);

/**
 * @brief Inserts a document reference into the hash index.
 * Handles incremental bucket splitting and directory doubling.
 */
WldResult db_hash_insert(WldBufferPool* pool, WldTransaction* tx, 
                         PageID dirId, uint64_t keyHash, PageID docPageId);

#endif // WALIA_DB_HASH_H
