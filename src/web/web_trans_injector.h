#ifndef WALIA_WEB_TRANS_INJECTOR_H
#define WALIA_WEB_TRANS_INJECTOR_H

#include "../db/db_common.h"
#include "../db/db_pool.h"
#include "../web/web_trans_common.h"

// ==========================================
// INJECTION CONTEXT
// ==========================================

typedef struct {
    WldBufferPool*  pool;
    WldTransaction* tx;
    PageID          targetPageId;
    const uint8_t*  sourceData;
    size_t          sourceLen;
} WldInjectionCtx;

// ==========================================
// INJECTOR API
// ==========================================

/**
 * @brief Performs transactional landing of JSON data into the heap.
 */
WldResult walia_json_inject(WldInjectionCtx* ctx, const uint64_t* masks, int maskCount);

#endif // WALIA_WEB_TRANS_INJECTOR_H
