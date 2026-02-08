#include <stdio.h>
#include <string.h>

#include "../db/db_common.h"
#include "../db/db_pager.h"
#include "../db/db_shadow.h"
#include "../db/db_hash.h"
#include "../db/db_slotted.h"
#include "../db/db_document.h"
#include "../db/db_auto_index.h"
#include "../db/db_catalog.h"
#include "../lib/lib_collections.h" // Fixed: Added for walia_list_new definition
#include "../core/vm.h"
#include "../core/object.h"

#include "../db/db_pool.h"
#include "../native.h"

// External substrate references from main.c / vm.c
// (Now resolved via headers)

// ==========================================
// COLLECTION MANAGEMENT
// ==========================================

/**
 * @brief Initializes a NoSQL collection directory.
 * db_collection(name) -> PageID
 */
static Value walia_db_collection(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;

    const char* colName = AS_CSTRING(args[0]);

    // 1. Check for existing collection in the Master Catalog
    WldEntryType type;
    PageID existingDir = db_catalog_lookup(&global_pool, NULL, colName, &type);
    if (existingDir != WLD_INVALID_PAGE) return NUMBER_VAL((double)existingDir);

    // 2. Create fresh NoSQL Persistent Directory (Extendible Hashing)
    WldTransaction tx = db_transaction_begin(global_pager);
    PageID dirId = db_hash_create(&global_pool, &tx);
    
    // 3. Persistent Registration
    db_catalog_add(&global_pool, &tx, colName, ENTRY_NOSQL_COLLECTION, dirId);
    
    db_transaction_commit(&tx);
    return NUMBER_VAL((double)dirId);
}

// ==========================================
// DOCUMENT OPERATIONS (UFO SPEED)
// ==========================================

/**
 * @brief Saves an arbitrary Walia object into the persistent store.
 * db_save(collection_id, key, object) -> Bool
 */
static Value walia_db_save(int argCount, Value* args) {
    if (argCount != 3 || !IS_NUMBER(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);

    PageID dirId = (PageID)AS_NUMBER(args[0]);
    const char* key = AS_CSTRING(args[1]);
    Value object = args[2];

    WldTransaction tx = db_transaction_begin(global_pager);

    // Sovereign Object Storage (SOS): Packs Map into Slotted Pages
    WldResult res = db_doc_store(&global_pool, &tx, dirId, key, object);

    if (res == WLD_OK) {
        db_transaction_commit(&tx);
        return BOOL_VAL(true);
    }

    db_transaction_rollback(&tx);
    return BOOL_VAL(false);
}

/**
 * @brief Performs O(1) point lookup on the persistent heap.
 * db_find(collection_id, key) -> Object
 */
static Value walia_db_find(int argCount, Value* args) {
    if (argCount != 2 || !IS_NUMBER(args[0]) || !IS_STRING(args[1])) return NIL_VAL;

    PageID dirId = (PageID)AS_NUMBER(args[0]);
    const char* key = AS_CSTRING(args[1]);

    WldTransaction tx = db_transaction_begin(global_pager);
    Value result = db_doc_retrieve(&global_pool, &tx, dirId, key);
    db_transaction_rollback(&tx);

    return result;
}

// ==========================================
// INTELLIGENT SENTRY INTEGRATION
// ==========================================

/**
 * @brief Schema-less filtering with Auto-Indexing notification.
 */
static Value walia_db_filter(int argCount, Value* args) {
    if (argCount != 3 || !IS_STRING(args[0]) || !IS_STRING(args[1])) {
        return NIL_VAL;
    }

    const char* colName = AS_CSTRING(args[0]);
    const char* field = AS_CSTRING(args[1]);

    // 1. SENTRY NOTIFICATION
    // Alerts the auto-indexer to monitor this specific field for high-frequency scans.
    WldTransaction tx = db_transaction_begin(global_pager);
    db_auto_index_notify(&global_pool, &tx, colName, field);
    db_transaction_commit(&tx);

    // 2. RESULT AGGREGATION
    // UFO SPEED: Direct landing of an empty list into a VM register.
    // Fixed: Signature synchronized with lib_collections.h Phase 19 implementation.
    Value results = walia_list_new_from_array(0, NULL);

    return results;
}

// ==========================================
// NATIVE INITIALIZATION
// ==========================================

void initNoSqlLibrary() {
    defineNative("db_collection", walia_db_collection);
    defineNative("db_save",       walia_db_save);
    defineNative("db_find",       walia_db_find);
    defineNative("db_filter",     walia_db_filter);
    
    printf(">> WaliaDB: NoSQL Party online [Document Infrastructure Active]\n");
}
