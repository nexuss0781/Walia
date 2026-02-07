#ifndef WALIA_DB_SHADOW_H
#define WALIA_DB_SHADOW_H

#include "db_common.h"
#include "db_pager.h"

// ==========================================
// SHADOW TABLE STRUCTURE
// ==========================================

typedef struct {
    PageID logicalId;
    PageID physicalId;
} ShadowEntry;

/**
 * @brief The Shadow Mapping Table Page.
 * Total Size: 16 (Header) + 4 (Count) + (509 * 8) = 4092 bytes.
 * Fits perfectly within the 4096-byte hardware page limit.
 */
typedef struct {
    WldPageHeader header;
    uint32_t      entryCount;
    ShadowEntry   entries[509]; 
} ShadowPage;

// ==========================================
// TRANSACTION CONTEXT
// ==========================================

typedef struct {
    WldPager* pager;
    PageID    activeShadowRoot; 
    bool      inTransaction;
} WldTransaction;

// ==========================================
// SHADOW PAGING API
// ==========================================

WldTransaction db_transaction_begin(WldPager* pager);
void* db_get_page_write(WldTransaction* tx, PageID logicalId);
const void* db_get_page_read(WldTransaction* tx, PageID logicalId);
WldResult db_transaction_commit(WldTransaction* tx);
void db_transaction_rollback(WldTransaction* tx);

#endif // WALIA_DB_SHADOW_H
