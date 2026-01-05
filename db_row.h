#ifndef WALIA_DB_ROW_H
#define WALIA_DB_ROW_H

#include "db_common.h"
#include "value.h"

// ==========================================
// ROW MARSHALLING API
// ==========================================

/**
 * @brief Packs a Walia Map into a binary row segment.
 */
WldResult db_row_pack(WldTableDescriptor* schema, Value object, uint8_t* outRow);

/**
 * @brief Hydrates a binary row into a Walia Map.
 */
Value db_row_unpack(WldTableDescriptor* schema, const uint8_t* rowData);

/**
 * @brief Direct register-lifting of a single column.
 */
Value db_row_get_column(WldTableDescriptor* schema, const uint8_t* rowData, int colIndex);

#endif // WALIA_DB_ROW_H