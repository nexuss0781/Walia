#ifndef WALIA_DB_HNSW_H
#define WALIA_DB_HNSW_H

#include "db_common.h"
#include "db_pool.h"
#include "db_vector.h"

// ==========================================
// HNSW CONFIGURATION
// ==========================================

#define WLD_HNSW_MAX_LEVELS 16    // Supports billions of nodes
#define WLD_HNSW_M          32    // Max neighbors per node (fan-out)
#define WLD_HNSW_EF_SEARCH  128   // Search complexity vs accuracy tradeoff

/**
 * @brief A Node in the Persistent Neural Graph.
 * Linked to a specific Vector Page.
 */
typedef struct {
    PageID  vectorPageId;         // Pointer to the ObjVector/ObjQuantizedVector
    uint32_t level;               // Highest level this node reaches
    PageID  neighbors[WLD_HNSW_MAX_LEVELS][WLD_HNSW_M];
    uint16_t neighborCount[WLD_HNSW_MAX_LEVELS];
} WldHNSWNode;

/**
 * @brief The HNSW Index Header.
 * Stored in the Database Catalog.
 */
typedef struct {
    WldPageHeader header;
    PageID        entryPoint;     // Top-level entry node
    int32_t       maxLevel;       // Current highest level in graph
    uint64_t      totalNodes;     
    WldDistanceMetric metric;     // Metric used for this index
} WldHNSWIndex;

// ==========================================
// HNSW ENGINE API
// ==========================================

/**
 * @brief Initializes a fresh HNSW index segment.
 */
PageID db_hnsw_create(WldBufferPool* pool, WldTransaction* tx, WldDistanceMetric metric);

/**
 * @brief Inserts a new vector into the Navigable Graph.
 * Uses a random level generator based on a geometric distribution.
 */
WldResult db_hnsw_insert(WldBufferPool* pool, WldTransaction* tx, 
                         PageID indexId, ObjVector* vec, PageID vecPageId);

/**
 * @brief Performs a UFO-speed similarity search.
 * 
 * @param k Number of nearest neighbors to return.
 * @return Value A Walia List containing the Top-K PageIDs.
 */
Value db_hnsw_search(WldBufferPool* pool, WldTransaction* tx, 
                     PageID indexId, ObjVector* query, int k);

#endif // WALIA_DB_HNSW_H
