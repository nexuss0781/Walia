#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#include "db_shadow.h"
#include "db_pager.h"

// ==========================================
// INTERNAL TRANSACTION HELPERS
// ==========================================

static PageID find_physical_id(WldTransaction* tx, PageID logicalId) {
    if (tx->activeShadowRoot == WLD_INVALID_PAGE) {
        return logicalId;
    }

    ShadowPage* table = (ShadowPage*)db_pager_get_page(tx->pager, tx->activeShadowRoot);
    
    for (uint32_t i = 0; i < table->entryCount; i++) {
        if (table->entries[i].logicalId == logicalId) {
            return table->entries[i].physicalId;
        }
    }

    return logicalId;
}

// ==========================================
// PUBLIC TRANSACTION API
// ==========================================

WldTransaction db_transaction_begin(WldPager* pager) {
    WldTransaction tx;
    tx.pager = pager;
    tx.inTransaction = true;
    tx.activeShadowRoot = pager->header->shadowTableRoot;
    
    if (tx.activeShadowRoot == WLD_INVALID_PAGE) {
        tx.activeShadowRoot = db_pager_allocate(pager);
        ShadowPage* table = (ShadowPage*)db_pager_get_page(pager, tx.activeShadowRoot);
        
        // Corrected: Initialize the header which is now present in the struct
        table->header.type = PAGE_TYPE_SUPERBLOCK;
        table->header.lsn = 1;
        table->header.checksum = 0;
        table->entryCount = 0;
    }

    return tx;
}

const void* db_get_page_read(WldTransaction* tx, PageID logicalId) {
    PageID physId = find_physical_id(tx, logicalId);
    return db_pager_get_page(tx->pager, physId);
}

void* db_get_page_write(WldTransaction* tx, PageID logicalId) {
    PageID shadowPhysId = db_pager_allocate(tx->pager);
    void* shadowData = db_pager_get_page(tx->pager, shadowPhysId);
    
    PageID currentPhysId = find_physical_id(tx, logicalId);
    const void* currentData = db_pager_get_page(tx->pager, currentPhysId);
    memcpy(shadowData, currentData, WLD_PAGE_SIZE);
    
    ShadowPage* table = (ShadowPage*)db_pager_get_page(tx->pager, tx->activeShadowRoot);
    
    if (table->entryCount >= 509) {
        fprintf(stderr, "WaliaDB: Shadow table overflow. Transaction too large.\n");
        return NULL;
    }

    table->entries[table->entryCount].logicalId = logicalId;
    table->entries[table->entryCount].physicalId = shadowPhysId;
    table->entryCount++;

    return shadowData;
}

WldResult db_transaction_commit(WldTransaction* tx) {
    if (!tx->inTransaction) return WLD_ERR_IO;

    // Flush data to physical storage before updating the root
    if (msync(tx->pager->fileMapping, tx->pager->fileSize, MS_SYNC) == -1) {
        perror("WaliaDB: Commit flush failed");
        return WLD_ERR_IO;
    }

    // Atomic Swap in Superblock
    tx->pager->header->shadowTableRoot = tx->activeShadowRoot;
    tx->pager->header->timestamp = (uint64_t)time(NULL);
    
    // Final sync of the Superblock itself
    db_pager_flush(tx->pager, 0);

    tx->inTransaction = false;
    return WLD_OK;
}

void db_transaction_rollback(WldTransaction* tx) {
    tx->inTransaction = false;
    tx->activeShadowRoot = tx->pager->header->shadowTableRoot;
}
