#ifndef WALIA_DB_SENTRY_TEMPORAL_H
#define WALIA_DB_SENTRY_TEMPORAL_H

#include "db_common.h"
#include "db_pool.h"

// ==========================================
// TEMPORAL SPECIFICATIONS
// ==========================================

#define WLD_MAX_SNAPSHOTS 250 // Fits within a single 4KB Registry Page

/**
 * @brief Represents a point-in-time state of the sovereign heap.
 */
typedef struct {
    uint64_t timestamp;      // Unix Epoch when checkpoint was committed
    PageID   shadowRootId;   // The Shadow Table Root ID at that time
    uint32_t transactionId;  // Sequential ID for audit trails
} WldSnapshot;

/**
 * @brief The Persistent Snapshot Registry (Page-Aligned).
 */
typedef struct {
    WldPageHeader header;
    uint32_t      snapshotCount;
    WldSnapshot   snapshots[WLD_MAX_SNAPSHOTS];
} WldSnapshotRegistry;

// ==========================================
// TEMPORAL API
// ==========================================

/**
 * @brief Registers the current committed state as a named snapshot.
 * Called automatically during walia_checkpoint().
 */
void db_temporal_record_snapshot(WldPager* pager, PageID rootId);

/**
 * @brief Locates the Shadow Root closest to a specific timestamp.
 */
PageID db_temporal_find_root(WldPager* pager, uint64_t timestamp);

/**
 * @brief Temporary "Time-Travel" Mounting.
 * 
 * Switches the Pager's mapping to resolve Logical IDs through 
 * a historical shadow table, enabling read-only queries on the past.
 */
WldResult db_temporal_mount(WldTransaction* tx, uint64_t timestamp);

#endif // WALIA_DB_SENTRY_TEMPORAL_H
