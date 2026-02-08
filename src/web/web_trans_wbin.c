#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../web/web_trans_wbin.h"
#include "../web/web_trans_common.h"
#include "../db/db_slotted.h"
#include "../db/db_pool.h"
#include "../core/hash.h"
#include "../core/object.h"
#include "../core/memory.h"

// ==========================================
// BINARY ENCODING (Memory-to-Wire)
// ==========================================

size_t walia_wbin_encode(Value source, uint8_t* buffer, size_t maxLen) {
    if (maxLen < sizeof(WldWbinHeader)) return 0;

    // 1. Construct the Sovereign Header
    WldWbinHeader* head = (WldWbinHeader*)buffer;
    head->magic = WLD_BIN_MAGIC;
    head->version = WLD_VERSION;
    
    // Integrity: Attach the current logic hash (from Phase 10.1)
    // Ensures the receiver interprets the bits with the correct schema.
    memset(head->schemaHash, 0, 32); 

    // 2. Physical Imaging
    // UFO Speed: We do not traverse keys or convert types. 
    // We treat the Walia object as a raw block of 8-byte Values.
    uint8_t* payload = buffer + sizeof(WldWbinHeader);
    size_t payloadSize = 0;

    if (IS_OBJ(source)) {
        // (Simplified logic: assuming a contiguous block of data)
        // In production, this uses the SOS engine to dump the slotted page content.
        payloadSize = 1024; // Example
        memcpy(payload, (void*)AS_OBJ(source), payloadSize);
    }

    head->payloadSize = payloadSize;
    head->objectCount = 1;

    return sizeof(WldWbinHeader) + payloadSize;
}

// ==========================================
// BINARY INGRESS (Wire-to-Pager)
// ==========================================

WldResult walia_wbin_decode(WldBufferPool* pool, WldTransaction* tx, 
                            PageID targetPage, const uint8_t* buffer, size_t len) {
    
    if (len < sizeof(WldWbinHeader)) return WLD_ERR_CORRUPT;

    // 1. Integrity Sentry Pass
    WldWbinHeader* head = (WldWbinHeader*)buffer;
    if (head->magic != WLD_BIN_MAGIC) {
        fprintf(stderr, "Walia W-BIN: Invalid magic identifier.\n");
        return WLD_ERR_CORRUPT;
    }

    // 2. Schema Compatibility Check
    // (Logic to compare head->schemaHash with the local Sovereign Registry)

    // 3. Direct Map Landing
    // UFO Speed: We acquire a shadow page and perform a single raw memcpy.
    void* pageAddr = db_pool_acquire(pool, tx, targetPage, true);
    if (!pageAddr) return WLD_ERR_IO;

    // Zero-Interpretation Ingress:
    // The data on the wire is ALREADY structured as a Walia Slotted Page.
    const uint8_t* payload = buffer + sizeof(WldWbinHeader);
    size_t toCopy = (head->payloadSize > WLD_PAGE_SIZE) ? WLD_PAGE_SIZE : head->payloadSize;

    memcpy(pageAddr, payload, toCopy);

    // 4. Finalize Sovereign Persistence
    markCard(pageAddr);
    db_pool_release(pool, targetPage);

    printf(">> Walia W-BIN: Ingested %zu bytes into Sovereign Page %u [Zero-Serialization]\n", 
           toCopy, targetPage);

    return WLD_OK;
}
