#ifndef WALIA_WEB_TRANS_WBIN_H
#define WALIA_WEB_TRANS_WBIN_H

#include "../db/db_common.h"
#include "../db/db_pool.h"
#include "../core/value.h"

// ==========================================
// W-BIN PROTOCOL SPECIFICATION
// ==========================================

/**
 * @brief The Sovereign Binary Header (Hardware Aligned).
 */
typedef struct {
    uint32_t magic;           // WLD_BIN_MAGIC (0x5742494E)
    uint32_t version;         // WLD_VERSION
    uint8_t  schemaHash[32];  // SHA-256 integrity contract
    uint64_t payloadSize;     // Size of the binary image
    uint64_t objectCount;     // Number of persistent nodes
} WldWbinHeader;

// ==========================================
// W-BIN API
// ==========================================

/**
 * @brief Encodes a Walia object into its binary memory-mapped image.
 */
size_t walia_wbin_encode(Value source, uint8_t* outBuffer, size_t maxLen);

/**
 * @brief Ingests a W-BIN stream directly into the Pager.
 */
WldResult walia_wbin_decode(WldBufferPool* pool, WldTransaction* tx, 
                            PageID targetPage, const uint8_t* buffer, size_t len);

#endif // WALIA_WEB_TRANS_WBIN_H
