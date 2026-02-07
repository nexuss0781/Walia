#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web/web_trans_injector.h"
#include "db/db_slotted.h"
#include "db/db_document.h"
#include "core/memory.h"

// ==========================================
// BIT-TRAILING OPTIMIZATION
// ==========================================

/**
 * @brief Finds the next structural delimiter in a 64-byte block.
 * UFO Speed: Uses a single CPU instruction (CTZ) to skip irrelevant bytes.
 */
static inline int get_next_bit(uint64_t mask) {
#ifdef __GNUC__
    return __builtin_ctzll(mask);
#else
    // Fallback for non-GCC compilers
    for (int i = 0; i < 64; i++) {
        if (mask & (1ULL << i)) return i;
    }
    return 64;
#endif
}

// ==========================================
// SOVEREIGN INJECTION LOGIC
// ==========================================

WldResult walia_json_inject(WldInjectionCtx* ctx, const uint64_t* masks, int maskCount) {
    printf(">> Walia Injector: Commencing Sovereign Landing into Page %u...\n", ctx->targetPageId);

    void* page = db_pool_acquire(ctx->pool, ctx->tx, ctx->targetPageId, true);
    if (!page) return WLD_ERR_IO;

    // Ensure page is initialized for NoSQL storage
    db_slotted_init(page);

    for (int m = 0; m < maskCount; m++) {
        uint64_t currentMask = masks[m];
        size_t blockOffset = m * 64;

        while (currentMask != 0) {
            // 1. TELEPORT to next delimiter
            int bitPos = get_next_bit(currentMask);
            uint8_t delimiter = ctx->sourceData[blockOffset + bitPos];

            // 2. DISPATCH based on JSON structure
            switch (delimiter) {
                case '{':
                    // Start of a Sovereign Object. In a production engine, 
                    // this triggers a recursive state-machine push.
                    break;

                case '"': {
                    // Start of a Key or String Value.
                    // We calculate the length by finding the matching quote bit.
                    uint64_t restOfMask = currentMask & ~( (1ULL << (bitPos + 1)) - 1 );
                    int nextQuote = get_next_bit(restOfMask);
                    
                    if (nextQuote < 64) {
                        const uint8_t* strStart = ctx->sourceData + blockOffset + bitPos + 1;
                        size_t strLen = nextQuote - bitPos - 1;

                        // UFO SPEED: Lazy Property Mapping
                        // We do not intern the string yet. We write the raw pointer 
                        // data into the persistent Slotted Page.
                        db_slotted_insert(page, strStart, (uint32_t)strLen);
                    }
                    break;
                }

                case ':':
                    // Key-Value boundary.
                    break;

                case '}':
                    // Close Object. Triggers the Write Barrier for the page.
                    markCard(page);
                    break;
            }

            // Clear processed bit and continue
            currentMask &= ~(1ULL << bitPos);
        }
    }

    db_pool_release(ctx->pool, ctx->targetPageId);
    return WLD_OK;
}
