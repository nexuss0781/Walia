#include <string.h>
#include <stdio.h>

#include "db_catalog.h"

// ==========================================
// INTERNAL CATALOG LOGIC
// ==========================================

void db_catalog_init(WldBufferPool* pool, WldTransaction* tx) {
    // Page 1 is hard-coded as the Catalog entry point in db_common.h
    WldCatalogPage* cat = (WldCatalogPage*)db_pool_acquire(pool, tx, 1, true);
    
    cat->header.type = PAGE_TYPE_CATALOG;
    cat->header.lsn = 1;
    cat->header.checksum = 0;
    
    cat->entryCount = 0;
    for (int i = 0; i < WLD_MAX_ENTRIES_PER_PAGE; i++) {
        cat->entries[i].isActive = false;
    }

    db_pool_release(pool, 1);
}

WldResult db_catalog_add(WldBufferPool* pool, WldTransaction* tx, 
                         const char* name, WldEntryType type, PageID root) {
    WldCatalogPage* cat = (WldCatalogPage*)db_pool_acquire(pool, tx, 1, true);

    // 1. Uniqueness Guard
    for (uint32_t i = 0; i < WLD_MAX_ENTRIES_PER_PAGE; i++) {
        if (cat->entries[i].isActive && strcmp(cat->entries[i].name, name) == 0) {
            db_pool_release(pool, 1);
            return WLD_ERR_IO; // Duplicate entry name
        }
    }

    // 2. Slot Allocation
    for (uint32_t i = 0; i < WLD_MAX_ENTRIES_PER_PAGE; i++) {
        if (!cat->entries[i].isActive) {
            strncpy(cat->entries[i].name, name, WLD_MAX_NAME_LEN);
            cat->entries[i].type = (uint32_t)type;
            cat->entries[i].rootPage = root;
            cat->entries[i].isActive = true;
            cat->entryCount++;
            
            db_pool_release(pool, 1);
            return WLD_OK;
        }
    }

    db_pool_release(pool, 1);
    return WLD_ERR_FULL; 
}

PageID db_catalog_lookup(WldBufferPool* pool, WldTransaction* tx, 
                         const char* name, WldEntryType* outType) {
    // Acquire for Read (Shadow Paging ensures snapshot isolation)
    WldCatalogPage* cat = (WldCatalogPage*)db_pool_acquire(pool, tx, 1, false);

    for (uint32_t i = 0; i < WLD_MAX_ENTRIES_PER_PAGE; i++) {
        if (cat->entries[i].isActive && strcmp(cat->entries[i].name, name) == 0) {
            if (outType) *outType = (WldEntryType)cat->entries[i].type;
            PageID root = cat->entries[i].rootPage;
            db_pool_release(pool, 1);
            return root;
        }
    }

    db_pool_release(pool, 1);
    return WLD_INVALID_PAGE;
}

WldResult db_catalog_remove(WldBufferPool* pool, WldTransaction* tx, const char* name) {
    WldCatalogPage* cat = (WldCatalogPage*)db_pool_acquire(pool, tx, 1, true);

    for (uint32_t i = 0; i < WLD_MAX_ENTRIES_PER_PAGE; i++) {
        if (cat->entries[i].isActive && strcmp(cat->entries[i].name, name) == 0) {
            cat->entries[i].isActive = false;
            cat->entryCount--;
            db_pool_release(pool, 1);
            return WLD_OK;
        }
    }

    db_pool_release(pool, 1);
    return WLD_ERR_IO; 
}
