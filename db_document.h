#ifndef WALIA_DB_DOCUMENT_H
#define WALIA_DB_DOCUMENT_H

#include "db_common.h"
#include "db_pool.h"
#include "value.h"

// ==========================================
// DOCUMENT STORAGE METADATA
// ==========================================

#define WLD_DOC_MAX_INLINE 3800 // Max bytes before triggering overflow

typedef struct {
    uint32_t totalSize;     // Total size of the serialized object
    bool     isFragmented;  // True if overflow pages are used
    PageID   nextFragment;  // Link to the first Overflow Page
} WldDocHeader;

typedef struct {
    WldPageHeader header;
    PageID        nextPage;  // Linked list of overflow segments
    uint32_t      dataSize;  // Bytes stored in this specific page
    uint8_t       payload[4064]; // Remaining 4KB space
} WldOverflowPage;

// ==========================================
// DOCUMENT API
// ==========================================

/**
 * @brief Serializes a Walia Map/Object into a persistent document.
 * 
 * @param object The Walia Value to store.
 * @param dirId  The PageID of the Hash Directory for the collection.
 * @param key    The unique identifier (String) for the document.
 */
WldResult db_doc_store(WldBufferPool* pool, WldTransaction* tx, 
                       PageID dirId, const char* key, Value object);

/**
 * @brief Retrieves and reconstructs a Walia Object from storage.
 */
Value db_doc_retrieve(WldBufferPool* pool, WldTransaction* tx, 
                      PageID dirId, const char* key);

#endif // WALIA_DB_DOCUMENT_H
