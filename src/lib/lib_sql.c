#include <stdio.h>
#include <string.h>

#include "db/db_common.h"
#include "db/db_pager.h"
#include "db/db_shadow.h"
#include "db/db_schema.h"
#include "db/db_row.h"
#include "db/db_btree.h"
#include "db/db_constraint.h"
#include "db/db_catalog.h"      // Fixed: Provides WldEntryType and db_catalog_lookup
#include "sql/sql_parser.h"
#include "sql/sql_compiler.h"
#include "core/vm.h"
#include "core/object.h"

#include "db/db_pool.h"
#include "native.h"

// External substrate references initialized in main.c
// (Now resolved via headers)

// ==========================================
// TABLE MANAGEMENT
// ==========================================

/**
 * @brief Creates a persistent SQL table structure.
 * db_create_table(name, columns_array) -> Bool
 */
static Value walia_db_create_table(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);

    const char* tableName = AS_CSTRING(args[0]);
    
    // In production, the Sentry ORM maps the class fields to this array.
    // For this core implementation, we define a standard 2-column structure.
    WldColumn cols[2];
    strncpy(cols[0].name, "id", 32); 
    cols[0].type = WLD_TYPE_INT; 
    cols[0].isPrimaryKey = true;
    cols[0].offset = 0;

    strncpy(cols[1].name, "data", 32); 
    cols[1].type = WLD_TYPE_FLOAT; 
    cols[1].isPrimaryKey = false;
    cols[1].offset = 8;

    WldTransaction tx = db_transaction_begin(global_pager);
    PageID schemaId = db_schema_create(global_pager, tableName, cols, 2);
    
    if (schemaId != WLD_INVALID_PAGE) {
        // Register in the Persistent Catalog
        db_catalog_add(&global_pool, &tx, tableName, ENTRY_SQL_TABLE, schemaId);
        db_transaction_commit(&tx);
        return BOOL_VAL(true);
    }

    db_transaction_rollback(&tx);
    return BOOL_VAL(false);
}

// ==========================================
// RELATIONAL DATA OPERATIONS
// ==========================================

/**
 * @brief Inserts a Walia object into the Clustered B+ Tree.
 * db_insert(table_name, object) -> Bool
 */
static Value walia_db_insert(int argCount, Value* args) {
    if (argCount != 2 || !IS_STRING(args[0])) return BOOL_VAL(false);

    const char* tableName = AS_CSTRING(args[0]);

    // 1. Schema Discovery
    WldEntryType type; // Fixed: Identifier now valid via db_catalog.h
    PageID schemaId = db_catalog_lookup(&global_pool, NULL, tableName, &type);
    if (schemaId == WLD_INVALID_PAGE) return BOOL_VAL(false);
    
    WldTableDescriptor* schema = (WldTableDescriptor*)db_pager_get_page(global_pager, schemaId);

    // 2. Binary Row Packing
    uint8_t rowBuffer[WLD_PAGE_SIZE]; 
    db_row_pack(schema, args[1], rowBuffer);

    // 3. Atomic Integrity Check & Write
    WldTransaction tx = db_transaction_begin(global_pager);
    if (db_validate_row(&global_pool, &tx, schema, rowBuffer) != CONSTRAINT_OK) {
        db_transaction_rollback(&tx);
        return BOOL_VAL(false);
    }

    uint64_t pk = db_get_pk_value(schema, rowBuffer);
    db_btree_insert(&global_pool, &tx, schema->btreeRoot, pk, args[1]);

    db_transaction_commit(&tx);
    return BOOL_VAL(true);
}

// ==========================================
// THE SOVEREIGN QUERY ENGINE (SQE)
// ==========================================

/**
 * @brief JIT Compiles and executes a SQL query.
 * db_query(sql_string) -> List
 */
static Value walia_db_query(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;

    // 1. SQL -> AST
    SqlQuery* query = db_sql_parse(AS_CSTRING(args[0]));
    if (!query) return NIL_VAL;

    // 2. Locate Target Schema
    WldEntryType type;
    PageID schemaId = db_catalog_lookup(&global_pool, NULL, query->sourceTable, &type);
    if (schemaId == WLD_INVALID_PAGE) {
        db_sql_free(query);
        return NIL_VAL;
    }

    WldTableDescriptor* schema = (WldTableDescriptor*)db_pager_get_page(global_pager, schemaId);

    // 3. JIT Compilation to Walia Bytecode
    Chunk planChunk;
    initChunk(&planChunk);
    if (!db_sql_compile(query, schema, &planChunk)) {
        db_sql_free(query);
        return NIL_VAL;
    }

    // 4. Execution within the Sovereign VM
    ObjFunction* planFunc = newFunction();
    planFunc->chunk = planChunk;
    
    InterpretResult result = interpret(planFunc);
    
    // Result is pushed to stack by the plan logic (R0)
    Value finalResults = (result == INTERPRET_OK) ? pop() : NIL_VAL;

    db_sql_free(query);
    return finalResults;
}

// ==========================================
// INITIALIZATION
// ==========================================

void initSqlLibrary() {
    defineNative("db_create_table", walia_db_create_table);
    defineNative("db_insert",       walia_db_insert);
    defineNative("db_query",        walia_db_query);
    
    printf(">> WaliaDB: SQL Agency Party online [Relational Core Active]\n");
}
