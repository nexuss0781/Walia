#include <stdio.h>
#include <stdint.h>

#include "db_constraint.h"
#include "db_row.h"
#include "db_btree.h"
#include "db_pool.h"
#include "../core/value.h"

// ==========================================
// INTERNAL HELPERS
// ==========================================

/**
 * @brief Identifies the physical value of the Primary Key.
 */
uint64_t db_get_pk_value(WldTableDescriptor* schema, const uint8_t* rowData) {
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        if (schema->columns[i].isPrimaryKey) {
            // UFO SPEED: Direct register-lifting of the PK bit pattern
            Value pkVal = db_row_get_column(schema, rowData, (int)i);
            return (uint64_t)pkVal;
        }
    }
    return 0; 
}

// ==========================================
// SENTRY VALIDATION LOGIC
// ==========================================

WldConstraintResult db_validate_row(WldBufferPool* pool, WldTransaction* tx,
                                    WldTableDescriptor* schema, const uint8_t* rowData) {
    
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        WldColumn* col = &schema->columns[i];
        Value val = db_row_get_column(schema, rowData, (int)i);

        // 1. SOVEREIGN NULL CHECK
        // If the column is marked NOT NULL, we verify the NaN-boxed value is not NIL
        if (!col->isNullable && IS_NIL(val)) {
            fprintf(stderr, ">> Walia Sentry: NULL violation in column '%s'.\n", col->name);
            return CONSTRAINT_VIOLATION_NULL;
        }

        // 2. PRIMARY KEY UNIQUENESS CHECK
        if (col->isPrimaryKey) {
            uint64_t pkValue = (uint64_t)val;
            
            // Search the Clustered B+ Tree for existing keys.
            // This is O(log n) but optimized via the Buffer Pool cache.
            Value existing = db_btree_find(pool, tx, schema->btreeRoot, pkValue);
            
            if (!IS_NIL(existing)) {
                fprintf(stderr, ">> Walia Sentry: PK violation. Duplicate key [0x%lx].\n", (unsigned long)pkValue);
                return CONSTRAINT_VIOLATION_PK;
            }
        }
    }

    // 3. INTEGRITY VERIFIED
    return CONSTRAINT_OK;
}
