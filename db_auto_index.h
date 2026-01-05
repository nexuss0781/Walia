#ifndef WALIA_DB_AUTO_INDEX_H
#define WALIA_DB_AUTO_INDEX_H

#include "db_common.h"
#include "db_pool.h"
#include "db_catalog.h"

// ==========================================
// SENTRY CONFIGURATION
// ==========================================

// Access threshold before spawning a physical index
#define WLD_AUTO_INDEX_THRESHOLD 10000 
#define WLD_MAX_TRACKED_FIELDS    64

typedef struct {
    char     fieldName[WLD_MAX_NAME_LEN];
    uint32_t accessCount;
    bool     isIndexed;
    PageID   indexDirId; // Link to the Hash Directory if created
} WldFieldTracker;

typedef struct {
    WldPageHeader   header;
    uint32_t        trackerCount;
    WldFieldTracker tracked[WLD_MAX_TRACKED_FIELDS];
} WldAutoIndexMetadata;

// ==========================================
// AUTO-INDEX API
// ==========================================

/**
 * @brief Notifies the Sentry that a field was used in a query.
 * 
 * If the access count crosses the threshold, it triggers 
 * the 'db_auto_index_spawn' logic.
 */
void db_auto_index_notify(WldBufferPool* pool, WldTransaction* tx, 
                          const char* collectionName, const char* fieldName);

/**
 * @brief Physically constructs a secondary index for an existing collection.
 * 
 * This involves a full collection scan to populate the new Hash Directory.
 */
WldResult db_auto_index_spawn(WldBufferPool* pool, WldTransaction* tx, 
                              const char* collectionName, const char* fieldName);

#endif // WALIA_DB_AUTO_INDEX_H
