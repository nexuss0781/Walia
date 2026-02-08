#include <stdio.h>
#include <string.h>

#include "../db/db_common.h"
#include "../db/db_vector.h"
#include "../db/db_hnsw.h"
#include "../db/db_quantize.h"
#include "../db/db_neural_stream.h"
#include "../db/db_catalog.h"
#include "../core/vm.h"
#include "../core/object.h"

#include "../db/db_pool.h"
#include "../native.h"

// External substrate references
// (Now resolved via headers)

// ==========================================
// VECTOR CONSTRUCTORS
// ==========================================

/**
 * Vector(dims, format) -> ObjVector
 * Usage: var embedding = Vector(1536, "F32");
 */
static Value walia_vector_new(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) return NIL_VAL;

    uint32_t dims = (uint32_t)AS_NUMBER(args[0]);
    WldVectorFormat fmt = WLD_VECTOR_F32;

    if (argCount == 2 && IS_STRING(args[1])) {
        const char* fmtStr = AS_CSTRING(args[1]);
        if (strcmp(fmtStr, "SQ8") == 0) fmt = WLD_VECTOR_SQ8;
        else if (strcmp(fmtStr, "BIT") == 0) fmt = WLD_VECTOR_BIT;
    }

    // Allocate SIMD-aligned vector on persistent heap
    ObjVector* vec = db_vector_new(dims, fmt);
    return OBJ_VAL(vec);
}

// ==========================================
// NEURAL SEARCH & STORAGE (Trillion-Scale)
// ==========================================

/**
 * db_vector_insert(collection_name, vector) -> Bool
 * Adds a neuron/embedding to the persistent HNSW graph.
 */
static Value walia_db_vector_insert(int argCount, Value* args) {
    if (argCount != 2 || !IS_STRING(args[0]) || !IS_OBJ(args[1])) return BOOL_VAL(false);

    const char* colName = AS_CSTRING(args[0]);
    ObjVector* vec = (ObjVector*)AS_OBJ(args[1]);

    // 1. Lookup Index from Catalog
    WldEntryType type;
    PageID indexId = db_catalog_lookup(&global_pool, NULL, colName, &type);
    if (indexId == WLD_INVALID_PAGE) {
        // Auto-create HNSW index if it doesn't exist
        WldTransaction tx = db_transaction_begin(global_pager);
        indexId = db_hnsw_create(&global_pool, &tx, METRIC_COSINE);
        db_catalog_add(&global_pool, &tx, colName, ENTRY_VECTOR_INDEX, indexId);
        db_transaction_commit(&tx);
    }

    // 2. Perform Atomic Insertion into Graph
    WldTransaction tx = db_transaction_begin(global_pager);
    PageID vecPageId = (PageID)(((uint8_t*)vec - global_pager->fileMapping) / WLD_PAGE_SIZE);
    
    WldResult res = db_hnsw_insert(&global_pool, &tx, indexId, vec, vecPageId);

    if (res == WLD_OK) {
        db_transaction_commit(&tx);
        return BOOL_VAL(true);
    }

    db_transaction_rollback(&tx);
    return BOOL_VAL(false);
}

/**
 * db_vector_search(collection_name, query_vector, k) -> List
 * Performs UFO-speed similarity search across trillions of neurons.
 */
static Value walia_db_vector_search(int argCount, Value* args) {
    if (argCount != 3 || !IS_STRING(args[0]) || !IS_OBJ(args[1])) return NIL_VAL;

    const char* colName = AS_CSTRING(args[0]);
    ObjVector* query = (ObjVector*)AS_OBJ(args[1]);
    int k = (int)AS_NUMBER(args[2]);

    // 1. Lookup Index
    PageID indexId = db_catalog_lookup(&global_pool, NULL, colName, NULL);
    if (indexId == WLD_INVALID_PAGE) return NIL_VAL;

    // 2. Initialize Neural Streamer (Predictive Prefetching)
    WldNeuralStreamer streamer;
    db_stream_init(&streamer, global_pager);

    // 3. EXECUTE HNSW SEARCH (O(log N))
    WldTransaction tx = db_transaction_begin(global_pager);
    
    // In production, db_hnsw_search uses the streamer internally 
    // to hide disk latency while navigating the graph.
    Value results = db_hnsw_search(&global_pool, &tx, indexId, query, k);
    
    db_transaction_rollback(&tx); // Read-only isolation
    return results;
}

// ==========================================
// QUANTUM COMPRESSION API
// ==========================================

/**
 * db_vector_quantize(vector) -> ObjQuantizedVector
 * Compresses an F32 vector to SQ8 to save 75% storage.
 */
static Value walia_db_vector_quantize(int argCount, Value* args) {
    if (argCount != 1 || !IS_OBJ(args[0])) return NIL_VAL;
    
    ObjVector* vec = (ObjVector*)AS_OBJ(args[0]);
    ObjQuantizedVector* qvec = db_vector_quantize_sq8(vec);
    
    return OBJ_VAL(qvec);
}

// ==========================================
// BOOTSTRAP
// ==========================================

void initVectorLibrary() {
    defineNative("Vector",            walia_vector_new);
    defineNative("db_vector_insert",  walia_db_vector_insert);
    defineNative("db_vector_search",  walia_db_vector_search);
    defineNative("db_vector_quantize", walia_db_vector_quantize);
    
    printf(">> WaliaDB: Vector Wing initialized [Trillion-Scale Neural Engine Ready]\n");
}
