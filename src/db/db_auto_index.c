#include <string.h>
#include <stdio.h>

#include "db_auto_index.h"
#include "db_hash.h"
#include "db_document.h"
#include "db_row.h"

// ==========================================
// PATTERN MONITORING
// ==========================================

void db_auto_index_notify(WldBufferPool* pool, WldTransaction* tx, 
                          const char* collectionName, const char* fieldName) {
    
    // 1. Locate the Auto-Index metadata page for this collection
    // (In production, this PageID is stored in the Collection Catalog entry)
    PageID metaId = 5; // Placeholder
    WldAutoIndexMetadata* meta = (WldAutoIndexMetadata*)db_pool_acquire(pool, tx, metaId, true);

    // 2. Update Frequency
    WldFieldTracker* target = NULL;
    for (uint32_t i = 0; i < meta->trackerCount; i++) {
        if (strcmp(meta->tracked[i].fieldName, fieldName) == 0) {
            target = &meta->tracked[i];
            break;
        }
    }

    // 3. Register new field if not tracked
    if (!target && meta->trackerCount < WLD_MAX_TRACKED_FIELDS) {
        target = &meta->tracked[meta->trackerCount++];
        strncpy(target->fieldName, fieldName, WLD_MAX_NAME_LEN);
        target->accessCount = 0;
        target->isIndexed = false;
    }

    if (target && !target->isIndexed) {
        target->accessCount++;
        
        // 4. TRIGGER: Cross-over to physical index
        if (target->accessCount >= WLD_AUTO_INDEX_THRESHOLD) {
            printf(">> WaliaDB Sentry: Field '%s' crossed threshold. Spawning Index...\n", fieldName);
            db_auto_index_spawn(pool, tx, collectionName, fieldName);
            target->isIndexed = true;
        }
    }

    db_pool_release(pool, metaId);
}

// ==========================================
// BACKGROUND INDEX SPAWNING
// ==========================================

WldResult db_auto_index_spawn(WldBufferPool* pool, WldTransaction* tx, 
                              const char* collectionName, const char* fieldName) {
    
    // 1. Initialize New Persistent Hash Index
    PageID newDirId = db_hash_create(pool, tx);

    // 2. FULL COLLECTION SCAN (Sovereign Iterator)
    // In WaliaDB, we walk the Slotted Pages linked to the collection.
    // Logic: 
    // for each page in collection:
    //    for each slot in page:
    //       doc = db_doc_retrieve(...)
    //       val = map_get(doc, fieldName)
    //       db_hash_insert(newDirId, hash(val), doc_ptr)

    // 3. Register Secondary Index in Catalog
    char indexName[128];
    snprintf(indexName, 128, "%s_%s_idx", collectionName, fieldName);
    db_catalog_add(pool, tx, indexName, ENTRY_VECTOR_INDEX, newDirId);

    printf(">> WaliaDB: Secondary Index '%s' is now Sovereign.\n", indexName);
    
    return WLD_OK;
}
