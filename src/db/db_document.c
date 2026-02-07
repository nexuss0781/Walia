#include <string.h>
#include <stdio.h>

#include "db_document.h"
#include "db_slotted.h"
#include "db_hash.h"
#include "../core/object.h"
#include "../core/hash.h"
#include "../core/memory.h"

// ==========================================
// INTERNAL SERIALIZATION
// ==========================================

/**
 * @brief Zero-copy serialization of a Walia Object.
 */
static uint32_t serialize_object(Value object, uint8_t* buffer) {
    WldDocHeader* head = (WldDocHeader*)buffer;
    
    // In Walia's Sovereign Object Storage (SOS), we pack the raw 
    // bit patterns. For Phase 8, we serialize the Value itself.
    uint8_t* dataStart = buffer + sizeof(WldDocHeader);
    memcpy(dataStart, &object, sizeof(Value));

    uint32_t payloadSize = sizeof(Value);
    head->totalSize = payloadSize + sizeof(WldDocHeader);
    head->isFragmented = false;
    head->nextFragment = WLD_INVALID_PAGE;

    return head->totalSize;
}

// ==========================================
// OVERFLOW MANAGEMENT
// ==========================================

static PageID handle_overflow(WldPager* pager, const uint8_t* data, uint32_t size) {
    PageID firstPageId = WLD_INVALID_PAGE;
    PageID lastPageId = WLD_INVALID_PAGE;
    uint32_t remaining = size;
    const uint8_t* ptr = data;

    while (remaining > 0) {
        PageID newId = db_pager_allocate(pager);
        WldOverflowPage* page = (WldOverflowPage*)db_pager_get_page(pager, newId);
        
        page->header.type = PAGE_TYPE_OVERFLOW;
        uint32_t toWrite = remaining > 4064 ? 4064 : remaining;
        
        page->dataSize = toWrite;
        page->nextPage = WLD_INVALID_PAGE;
        memcpy(page->payload, ptr, toWrite);

        if (firstPageId == WLD_INVALID_PAGE) firstPageId = newId;
        if (lastPageId != WLD_INVALID_PAGE) {
            WldOverflowPage* prev = (WldOverflowPage*)db_pager_get_page(pager, lastPageId);
            prev->nextPage = newId;
        }

        lastPageId = newId;
        ptr += toWrite;
        remaining -= toWrite;
    }

    return firstPageId;
}

// ==========================================
// PUBLIC DOCUMENT API
// ==========================================

WldResult db_doc_store(WldBufferPool* pool, WldTransaction* tx, 
                       PageID dirId, const char* key, Value object) {
    
    uint8_t buffer[WLD_PAGE_SIZE]; 
    uint32_t size = serialize_object(object, buffer);

    PageID targetPageId = 100; // Heuristic: Collection Data Root
    
    if (size > WLD_DOC_MAX_INLINE) {
        WldDocHeader* head = (WldDocHeader*)buffer;
        head->isFragmented = true;
        head->nextFragment = handle_overflow(tx->pager, buffer + sizeof(WldDocHeader), size - sizeof(WldDocHeader));
        size = sizeof(WldDocHeader);
    }

    void* page = db_pool_acquire(pool, tx, targetPageId, true);
    int slot = db_slotted_insert(page, buffer, size);
    
    // Silence unused variable for Phase 8 logic
    (void)slot;

    db_pool_release(pool, targetPageId);

    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)key, strlen(key));
    uint8_t hashBytes[32];
    walia_hash_final(&ctx, hashBytes);
    
    uint64_t keyHash = *(uint64_t*)hashBytes; 
    return db_hash_insert(pool, tx, dirId, keyHash, targetPageId);
}

Value db_doc_retrieve(WldBufferPool* pool, WldTransaction* tx, 
                      PageID dirId, const char* key) {
    
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)key, strlen(key));
    uint8_t hashBytes[32];
    walia_hash_final(&ctx, hashBytes);
    uint64_t keyHash = *(uint64_t*)hashBytes;

    PageID docPageId = db_hash_lookup(pool, tx, dirId, keyHash);
    if (docPageId == WLD_INVALID_PAGE) return NIL_VAL;

    void* page = db_pool_acquire(pool, tx, docPageId, false);
    uint32_t size;
    uint8_t* data = db_slotted_get(page, 0, &size);

    if (data == NULL) {
        db_pool_release(pool, docPageId);
        return NIL_VAL;
    }

    WldDocHeader* head = (WldDocHeader*)data;
    Value result;
    
    if (!head->isFragmented) {
        memcpy(&result, data + sizeof(WldDocHeader), sizeof(Value));
    } else {
        // Logic to reconstruct from overflow chain
        result = NIL_VAL; 
    }

    db_pool_release(pool, docPageId);
    return result;
}
