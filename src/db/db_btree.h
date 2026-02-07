#ifndef WALIA_DB_BTREE_H
#define WALIA_DB_BTREE_H

#include "db_common.h"
#include "db_pool.h"
#include "../core/value.h"

// ==========================================
// B+ TREE GEOMETRY (4KB Optimized)
// ==========================================

// Internal Node: [Key (8 bytes)][PageID (4 bytes)] = 12 bytes
// (4096 - 32 header) / 12 = 338. We use 330 for safe padding.
#define BTREE_INTERNAL_CAPACITY 330

// Leaf Node: [Key (8 bytes)][Value (8 bytes)] = 16 bytes
// (4096 - 32 header) / 16 = 254. We use 250 for safe padding.
#define BTREE_LEAF_CAPACITY 250

typedef struct {
    uint64_t key;
    union {
        PageID pageId; 
        Value  value;  
    } data;
} BTreeEntry;

typedef struct {
    WldPageHeader header;
    
    bool     isLeaf;
    uint32_t entryCount;
    PageID   parent;
    PageID   next; 
    PageID   prev; 
    
    BTreeEntry entries[BTREE_INTERNAL_CAPACITY]; 
} BTreeNode;

// ==========================================
// B+ TREE API
// ==========================================

/**
 * @brief Creates a new B+ Tree and returns the Root Page ID.
 */
PageID db_btree_create(WldBufferPool* pool, WldTransaction* tx);

/**
 * @brief Inserts a key-value pair into the Clustered Index.
 */
WldResult db_btree_insert(WldBufferPool* pool, WldTransaction* tx, 
                          PageID root, uint64_t key, Value value);

/**
 * @brief Retrieves a value by its Primary Key.
 */
Value db_btree_find(WldBufferPool* pool, WldTransaction* tx, 
                    PageID root, uint64_t key);

/**
 * @brief Performs a range scan (UFO-Speed).
 */
Value db_btree_range_scan(WldBufferPool* pool, WldTransaction* tx, 
                          PageID root, uint64_t startKey, uint64_t endKey);

#endif // WALIA_DB_BTREE_H
