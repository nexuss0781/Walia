#ifndef WALIA_DB_RELATIONAL_H
#define WALIA_DB_RELATIONAL_H

#include "db_common.h"
#include "db_schema.h"
#include "db_pool.h"
#include "../core/value.h" // Fixed: Added for Value type definition

// ==========================================
// RELATIONSHIP TYPES
// ==========================================

typedef enum {
    REL_ONE_TO_ONE,
    REL_ONE_TO_MANY,
    REL_MANY_TO_MANY
} WldRelType;

typedef enum {
    ACTION_CASCADE,
    ACTION_RESTRICT,
    ACTION_SET_NULL
} WldRelAction;

/**
 * @brief Foreign Key Metadata.
 * Encapsulates the link between two sovereign tables.
 */
typedef struct {
    char         fkName[32];
    char         targetTable[64];
    uint32_t     localColIndex;
    uint32_t     targetColIndex; 
    uint32_t     onDelete;       // WldRelAction
    bool         isPointerBased; // UFO-Speed Optimization Toggle
} WldForeignKey;

// ==========================================
// RELATIONAL ENGINE API
// ==========================================

/**
 * @brief Resolves a Pointer-Based Join.
 * 
 * UFO SPEED: Instead of searching an index, this function uses the PageID 
 * stored in the FK column to jump directly to the related record.
 * 
 * @return Value The related record as a Sovereign Map.
 */
Value db_rel_follow_pointer(WldBufferPool* pool, WldTransaction* tx,
                            WldTableDescriptor* targetSchema, PageID targetPage);

/**
 * @brief Enforces referential integrity during a DELETE operation.
 */
WldResult db_rel_enforce_integrity(WldBufferPool* pool, WldTransaction* tx,
                                   WldForeignKey* fk, uint64_t deletedKey);

#endif // WALIA_DB_RELATIONAL_H
