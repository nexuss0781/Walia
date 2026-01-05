#ifndef WALIA_DB_SCHEMA_H
#define WALIA_DB_SCHEMA_H

#include "db_common.h"
#include "db_pager.h"

// ==========================================
// SCHEMA MANAGEMENT API
// ==========================================

/**
 * @brief Physically writes a table descriptor to the persistent heap.
 */
PageID db_schema_create(WldPager* pager, const char* name, WldColumn* cols, int count);

/**
 * @brief Validates a running logic contract against a persistent schema.
 */
bool db_schema_verify(WldTableDescriptor* schema, uint8_t* expectedHash);

#endif // WALIA_DB_SCHEMA_H