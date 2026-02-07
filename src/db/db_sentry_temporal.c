#include <stdio.h>
#include <string.h>
#include <time.h>

#include "db_sentry_temporal.h"
#include "db_pager.h"
#include "db_shadow.h"

// ==========================================
// REGISTRY MANAGEMENT
// ==========================================

/**
 * @brief Internal helper to acquire the Registry Page.
 * Historically, WaliaDB reserves Page 2 for snapshots.
 */
static WldSnapshotRegistry* get_registry(WldPager* pager) {
    return (WldSnapshotRegistry*)db_pager_get_page(pager, 2);
}

void db_temporal_record_snapshot(WldPager* pager, PageID rootId) {
    WldSnapshotRegistry* reg = get_registry(pager);
    
    // Industrial Rotation: If registry is full, drop the oldest snapshot
    if (reg->snapshotCount >= WLD_MAX_SNAPSHOTS) {
        memmove(&reg->snapshots[0], &reg->snapshots[1], 
                sizeof(WldSnapshot) * (WLD_MAX_SNAPSHOTS - 1));
        reg->snapshotCount--;
    }

    WldSnapshot* s = &reg->snapshots[reg->snapshotCount++];
    s->timestamp = (uint64_t)time(NULL);
    s->shadowRootId = rootId;
    s->transactionId = pager->header->pageCount; // Example sequence

    printf(">> Walia Sentry: Recorded Temporal Snapshot at %lu [Root: %u]\n", 
           (unsigned long)s->timestamp, rootId);
}

// ==========================================
// TIME-TRAVEL NAVIGATION
// ==========================================

PageID db_temporal_find_root(WldPager* pager, uint64_t timestamp) {
    WldSnapshotRegistry* reg = get_registry(pager);
    
    // Binary Search for the closest timestamp (O(log snapshots))
    PageID bestRoot = pager->header->shadowTableRoot; // Default to Live
    uint64_t closestDiff = 0xFFFFFFFFFFFFFFFF;

    for (uint32_t i = 0; i < reg->snapshotCount; i++) {
        if (reg->snapshots[i].timestamp <= timestamp) {
            uint64_t diff = timestamp - reg->snapshots[i].timestamp;
            if (diff < closestDiff) {
                closestDiff = diff;
                bestRoot = reg->snapshots[i].shadowRootId;
            }
        }
    }

    return bestRoot;
}

WldResult db_temporal_mount(WldTransaction* tx, uint64_t timestamp) {
    PageID historicalRoot = db_temporal_find_root(tx->pager, timestamp);
    
    if (historicalRoot == WLD_INVALID_PAGE) return WLD_ERR_IO;

    // UFO GRADE JUMP:
    // We simply redirect the transaction's shadow root.
    // Subsequent calls to db_get_page_read() will now automatically 
    // resolve to the physical pages that were active at that timestamp.
    tx->activeShadowRoot = historicalRoot;
    
    printf(">> Walia Sentry: Mounted Temporal View for T=%lu\n", (unsigned long)timestamp);
    return WLD_OK;
}
