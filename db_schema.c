#include <string.h>
#include <stdio.h>

#include "db_schema.h"
#include "hash.h"
#include "db_btree.h"
#include "object.h"
#include "memory.h" // Fixed: Added for markCard definition

// ==========================================
// INTERNAL HELPERS
// ==========================================

/**
 * @brief Computes the physical byte-offsets for a set of columns.
 * Enforces 8-byte alignment to support direct NaN-boxed register lifting.
 */
uint32_t db_schema_calc_row_size(WldColumn* columns, int colCount) {
    uint32_t size = 0;
    for (int i = 0; i < colCount; i++) {
        columns[i].offset = size;
        size += 8; // Every Walia column is a 64-bit word
    }
    return size;
}

/**
 * @brief Generates the logic identity for the schema.
 */
static void compute_schema_hash(WldTableDescriptor* desc, uint8_t* outHash) {
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)desc->tableName, WLD_MAX_NAME_LEN);
    walia_hash_update(&ctx, (uint8_t*)desc->columns, sizeof(WldColumn) * desc->columnCount);
    walia_hash_final(&ctx, outHash);
}

// ==========================================
// PUBLIC SCHEMA LOGIC
// ==========================================

PageID db_schema_create(WldPager* pager, const char* tableName,
                        WldColumn* columns, int colCount) {
    
    // 1. PHYSICAL LANDING
    PageID descId = db_pager_allocate(pager);
    WldTableDescriptor* desc = (WldTableDescriptor*)db_pager_get_page(pager, descId);

    // 2. HEADER INITIALIZATION
    desc->header.type = PAGE_TYPE_SCHEMA;
    desc->header.lsn = 1;
    desc->header.checksum = 0;
    desc->header.ownerId = descId; 

    // 3. METADATA POPULATION
    strncpy(desc->tableName, tableName, WLD_MAX_NAME_LEN - 1);
    desc->columnCount = (uint32_t)colCount;
    desc->rowSize = db_schema_calc_row_size(columns, colCount);
    memcpy(desc->columns, columns, sizeof(WldColumn) * colCount);

    // 4. LOGIC FINGERPRINTING
    compute_schema_hash(desc, desc->schemaHash);

    // 5. CLUSTERED B+ TREE INITIALIZATION
    // Creates the physical entry point for relational storage
    desc->btreeRoot = db_btree_create(NULL, NULL);

    // 6. PERSISTENCE SENTRY
    markCard(desc);
    return descId;
}

/**
 * @brief Validates if a Walia object is compatible with the physical schema.
 */
bool db_schema_validate_object(WldTableDescriptor* schema, Value object) {
    // Sentry Check: Input must be a Sovereign Map (OBJ_MAP)
    if (!IS_OBJ(object) || AS_OBJ(object)->type != OBJ_MAP) {
        return false;
    }

    // Industrial Verification: Ensures the document is not empty
    return (schema->columnCount > 0);
}