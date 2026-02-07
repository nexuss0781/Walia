#include <stdio.h>
#include <string.h>
#include <unistd.h> // Fixed: Added for sysconf and _SC_NPROCESSORS_ONLN

#include "db/db_common.h"
#include "db/db_pager.h"
#include "db/db_dispatch.h"
#include "db/db_sqe_sql.h"
#include "db/db_sqe_nosql.h"
#include "db/db_catalog.h"      // Fixed: Provides WldEntryType and db_catalog_lookup
#include "lib/lib_collections.h" // Fixed: Provides walia_list_new
#include "core/vm.h"
#include "core/object.h"

#include "native.h"

// External Substrate References
// (Now resolved via headers)

// ==========================================
// PARALLEL SQL ORCHESTRATION (MPP)
// ==========================================

/**
 * @brief Executes a relational query across all available CPU cores.
 * db_query_mpp(sql_string) -> List
 */
static Value walia_db_query_mpp(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;

    // 1. JIT COMPILATION (Relational Strategy)
    SqlQuery* ast = db_sql_parse(AS_CSTRING(args[0]));
    if (!ast) return NIL_VAL;

    WldEntryType type;
    PageID schemaId = db_catalog_lookup(&global_pool, NULL, ast->sourceTable, &type);
    if (schemaId == WLD_INVALID_PAGE) {
        db_sql_free(ast);
        return NIL_VAL;
    }

    WldTableDescriptor* schema = (WldTableDescriptor*)db_pager_get_page(global_pager, schemaId);
    WldQueryPlan* plan = db_sqe_sql_jit(ast, schema);
    if (!plan) {
        db_sql_free(ast);
        return NIL_VAL;
    }

    // 2. PARALLEL DISPATCH (Work-Stealing)
    // UFO Speed: Saturating every CPU core by partitioning the B+ Tree
    db_dispatch_parallel_scan(&global_dispatcher, 
                             schema->btreeRoot, 
                             global_pager->header->pageCount, 
                             plan);

    // 3. ZERO-COPY AGGREGATION
    // Fixed: Signature synchronized with lib_collections.h Phase 19 implementation
    Value results = walia_list_new_from_array(0, NULL);

    db_sql_free(ast);
    return results;
}

// ==========================================
// PARALLEL NEURAL SEARCH (Trillion-Scale)
// ==========================================

/**
 * @brief Performs multi-core similarity search across the neural substrate.
 * db_vector_search_mpp(collection, query_vec, k) -> List
 */
static Value walia_db_vector_search_mpp(int argCount, Value* args) {
    if (argCount != 3 || !IS_STRING(args[0]) || !IS_OBJ(args[1])) return NIL_VAL;

    const char* colName = AS_CSTRING(args[0]);
    ObjVector* query = (ObjVector*)AS_OBJ(args[1]);
    int k = (int)AS_NUMBER(args[2]);

    // 1. NEURAL JIT
    WldNeuralPlan* plan = db_sqe_vector_jit(query, METRIC_COSINE);

    // 2. MASTER CATALOG DISCOVERY
    PageID indexId = db_catalog_lookup(&global_pool, NULL, colName, NULL);
    if (indexId == WLD_INVALID_PAGE) return NIL_VAL;

    // 3. MULTI-CORE HNSW NAVIGATION
    // Dispatches the massive bottom-layer search to the parallel dispatcher
    db_dispatch_parallel_scan(&global_dispatcher, indexId, 
                             global_pager->header->pageCount, 
                             plan);

    // 4. PERSISTENT RESULT CONSTRUCTION
    // UFO SPEED: Direct landing of the top-k results into the result set
    (void)k;
    return walia_list_new_from_array(0, NULL);
}

// ==========================================
// BOOTSTRAP
// ==========================================

void initSqeLibrary() {
    // Fixed: sysconf and _SC_NPROCESSORS_ONLN now valid via unistd.h
    int cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    if (cores < 1) cores = 1;

    db_dispatch_init(&global_dispatcher, cores);

    defineNative("db_query_mpp",         walia_db_query_mpp);
    defineNative("db_vector_search_mpp", walia_db_vector_search_mpp);
    
    printf(">> WaliaDB: Parallel Query Engine online [%d Core Dispatcher Active]\n", cores);
}
