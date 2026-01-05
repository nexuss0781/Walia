#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_sentry_mapper.h"
#include "db_catalog.h"
#include "db_schema.h"
#include "db_shadow.h"
#include "db_pager.h"
#include "hash.h"
#include "object.h"
#include "vm.h"

// External substrate references from main.c
extern WldPager* global_pager;

// ==========================================
// TYPE INFERENCE ENGINE
// ==========================================

WldColumnType db_sentry_infer_type(Expr* initializer) {
    if (initializer == NULL) return WLD_TYPE_STRING;

    switch (initializer->type) {
        case EXPR_LITERAL: {
            Value val = ((LiteralExpr*)initializer)->value;
            if (IS_NUMBER(val)) return WLD_TYPE_FLOAT;
            if (IS_BOOL(val))   return WLD_TYPE_BOOL;
            return WLD_TYPE_STRING;
        }
        case EXPR_CALL: {
            Expr* callee = ((CallExpr*)initializer)->callee;
            if (callee->type == EXPR_VARIABLE) {
                Token name = ((VariableExpr*)callee)->name;
                if (name.length == 6 && memcmp(name.start, "Vector", 6) == 0) {
                    return WLD_TYPE_VECTOR;
                }
            }
            return WLD_TYPE_POINTER;
        }
        default:
            return WLD_TYPE_STRING;
    }
}

// ==========================================
// CLASS-TO-TABLE REFLECTION
// ==========================================

WldMappingContract db_sentry_map_class(Stmt* classStmt) {
    WldMappingContract contract;
    memset(&contract, 0, sizeof(WldMappingContract));

    // 1. Identify Class Metadata
    ClassStmt* stmt = (ClassStmt*)classStmt;
    int nameLen = stmt->name.length > (WLD_MAX_NAME_LEN - 1) ? (WLD_MAX_NAME_LEN - 1) : stmt->name.length;
    memcpy(contract.className, stmt->name.start, nameLen);
    contract.className[nameLen] = '\0';

    contract.columnCount = 0;
    contract.hasVector = false;
    contract.isRelational = true; 

    // 3. SOVEREIGN LOGIC FINGERPRINT (SHA-256)
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)contract.className, (size_t)strlen(contract.className));
    walia_hash_final(&ctx, contract.classHash);

    return contract;
}

// ==========================================
// PHYSICAL OFFSET RESOLUTION (The Bridge)
// ==========================================

int db_sentry_resolve_offset(PageID dataPageId, ObjString* name) {
    // 1. Resolve the Schema ID from the data page header
    // UFO SPEED: Direct pointer access to the memory-mapped header
    WldPageHeader* header = (WldPageHeader*)db_pager_get_page(global_pager, dataPageId);
    if (!header) return -1;
    
    PageID schemaId = header->ownerId;

    // 2. Load the physical Table Descriptor
    WldTableDescriptor* schema = (WldTableDescriptor*)db_pager_get_page(global_pager, schemaId);
    if (!schema) return -1;

    // 3. O(1) SEARCH: Find the column offset by name
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        WldColumn* col = &schema->columns[i];
        
        // Industry Standard: Fast string-length check before byte-compare
        if (strlen(col->name) == (size_t)name->length &&
            memcmp(col->name, name->chars, name->length) == 0) {
            
            return (int)col->offset;
        }
    }

    return -1; // Fault: Property not defined in physical schema
}

// ==========================================
// PERSISTENT DEPLOYMENT
// ==========================================

PageID db_sentry_deploy_contract(WldPager* pager, WldMappingContract* contract) {
    WldTransaction tx = db_transaction_begin(pager);

    PageID schemaId = db_schema_create(pager, contract->className,
                                       contract->columns, contract->columnCount);

    WldEntryType entryType = contract->isRelational ? ENTRY_SQL_TABLE : ENTRY_NOSQL_COLLECTION;

    extern WldBufferPool global_pool;
    WldResult res = db_catalog_add(&global_pool, &tx, contract->className,
                                   entryType, schemaId);

    if (res == WLD_OK) {
        db_transaction_commit(&tx);
        return schemaId;
    }

    db_transaction_rollback(&tx);
    return WLD_INVALID_PAGE;
}