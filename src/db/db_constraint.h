#ifndef WALIA_DB_CONSTRAINT_H
#define WALIA_DB_CONSTRAINT_H

#include "db_common.h"
#include "db_schema.h"
#include "db_pool.h"

// ==========================================
// CONSTRAINT TYPES
// ==========================================

typedef enum {
    CONSTRAINT_OK = 0,
    CONSTRAINT_VIOLATION_PK,      // Duplicate Primary Key
    CONSTRAINT_VIOLATION_NULL,    // Null value in NOT NULL column
    CONSTRAINT_VIOLATION_UNIQUE,  // Duplicate value in unique index
    CONSTRAINT_VIOLATION_TYPE     // Data type mismatch
} WldConstraintResult;

// ==========================================
// THE SENTRY API
// ==========================================

/**
 * @brief Performs a pre-flight integrity check on a row buffer.
 * 
 * Logic:
 * 1. Checks every 'NOT NULL' column for NIL_VAL.
 * 2. Checks 'PRIMARY KEY' against the existing B+ Tree to ensure uniqueness.
 * 
 * @param pool The active buffer pool.
 * @param tx   The active transaction.
 * @param schema The table descriptor.
 * @param rowData The binary-packed row to validate.
 * @return WldConstraintResult OK if valid, otherwise the specific violation.
 */
WldConstraintResult db_validate_row(WldBufferPool* pool, WldTransaction* tx, 
                                    WldTableDescriptor* schema, const uint8_t* rowData);

/**
 * @brief Helper to extract the Primary Key value from a packed row.
 */
uint64_t db_get_pk_value(WldTableDescriptor* schema, const uint8_t* rowData);

#endif // WALIA_DB_CONSTRAINT_H
