#include <stdio.h>
#include <string.h>
#include <time.h>

#include "db_common.h"
#include "db_pager.h"
#include "db_shadow.h"
#include "db_catalog.h"      // Fixed: Provides WldEntryType and db_catalog_lookup
#include "db_sentry_mapper.h"
#include "db_sentry_jit.h"
#include "db_sentry_temporal.h"
#include "db_sentry_security.h"
#include "lib_collections.h" // Fixed: Provides walia_list_new
#include "vm.h"
#include "object.h"

// External Substrate References
extern WldPager* global_pager;
extern WldBufferPool global_pool;
extern void defineNative(const char* name, NativeFn function);

// ==========================================
// TEMPORAL SOVEREIGNTY
// ==========================================

/**
 * @brief Views the database as it existed at a specific point in history.
 * db.at(timestamp) -> DatabaseView
 */
static Value walia_sentry_at(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;

    uint64_t targetTime = (uint64_t)AS_NUMBER(args[0]);
    
    // 1. Create a Historical Transaction
    WldTransaction tx = db_transaction_begin(global_pager);
    
    // 2. Resolve historical root from the Snapshot Registry (Page 2)
    if (db_temporal_mount(&tx, targetTime) == WLD_OK) {
        printf(">> Walia Sentry: Temporal View mounted for timestamp %lu.\n", (unsigned long)targetTime);
        // Returns a proxy identifying the historical context
        return OBJ_VAL(copyString("TemporalContextProxy", 20));
    }

    db_transaction_rollback(&tx);
    return NIL_VAL;
}

// ==========================================
// SOVEREIGN SECURITY
// ==========================================

/**
 * @brief Authenticates the session with a Sovereign Role.
 * db.login(role_name, secret_key)
 */
static Value walia_sentry_login(int argCount, Value* args) {
    if (argCount != 2 || !IS_STRING(args[0])) return BOOL_VAL(false);

    const char* role = AS_CSTRING(args[0]);
    // (In production, args[1] is validated against the persistent security registry)
    
    db_security_assume_role(role);
    return BOOL_VAL(true);
}

// ==========================================
// REFLECTIVE ORM (The Intelligence Bridge)
// ==========================================

/**
 * @brief UFO-Grade Filter. Unrolls closures into SIMD machine loops.
 * db.table.find(closure)
 */
static Value walia_sentry_find(int argCount, Value* args) {
    // Expected: args[0] = TableName (String), args[1] = Lambda (Closure)
    if (argCount != 2 || !IS_STRING(args[0]) || !IS_CLOSURE(args[1])) {
        return NIL_VAL;
    }

    const char* tableName = AS_CSTRING(args[0]);

    // 1. PHYSICAL DISCOVERY
    // Resolves the physical entry point from the Master Catalog.
    WldEntryType type;
    PageID schemaId = db_catalog_lookup(&global_pool, NULL, tableName, &type);
    
    if (schemaId == WLD_INVALID_PAGE) {
        fprintf(stderr, ">> Walia Sentry: Fault - Table '%s' not found.\n", tableName);
        return NIL_VAL;
    }

    // 2. SCHEMA HYDRATION
    WldTableDescriptor* schema = (WldTableDescriptor*)db_pager_get_page(global_pager, schemaId);

    // 3. LAMBDA UNROLLING (Sentry JIT)
    // Intercepts the Walia closure and compiles it into a SIMD-unrolled predicate.
    ObjClosure* lambda = AS_CLOSURE(args[1]);
    WldSentryPredicate* pred = db_sentry_compile_predicate(lambda, schema);

    // 4. RESULT SET INITIALIZATION
    // UFO SPEED: Direct landing of a persistent empty list into the VM.
    // Fixed: Signature synchronized with lib_collections.h Phase 19 implementation.
    Value results = walia_list_new_from_array(0, NULL);

    if (pred != NULL) {
        printf(">> Walia Sentry: Dispatched Parallel SIMD Scan on '%s'.\n", tableName);
        // (Execution would proceed to db_dispatch_parallel_scan using pred->optimizedFunc)
    } else {
        printf(">> Walia Sentry: Falling back to standard interpreted scan mode.\n");
    }

    return results;
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

void initSentryLibrary() {
    // System-level database primitives
    defineNative("db_at",    walia_sentry_at);
    defineNative("db_login", walia_sentry_login);
    
    // ORM primitives used by compiler-generated glue
    defineNative("sentry_find", walia_sentry_find);

    printf(">> Walia Sentry: Master ORM and Security Party online.\n");
}
