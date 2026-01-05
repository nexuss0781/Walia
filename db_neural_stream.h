#ifndef WALIA_DB_NEURAL_STREAM_H
#define WALIA_DB_NEURAL_STREAM_H

#include "db_common.h"
#include "db_pager.h"

// ==========================================
// STREAMER CONFIGURATION
// ==========================================

#define WLD_STREAM_LOOKAHEAD 8  // Number of neighbor-hops to pre-fetch
#define WLD_STREAM_BATCH_SIZE 16 // Max concurrent page hints

/**
 * @brief The Neural Streamer State.
 * Tracks the current search trajectory to optimize pre-fetching.
 */
typedef struct {
    WldPager* pager;
    PageID    pendingHints[WLD_STREAM_BATCH_SIZE];
    int       hintCount;
} WldNeuralStreamer;

// ==========================================
// NEURAL STREAMING API
// ==========================================

/**
 * @brief Initializes the streamer for a neural search session.
 */
void db_stream_init(WldNeuralStreamer* streamer, WldPager* pager);

/**
 * @brief Issues a non-blocking pre-fetch hint to the OS.
 * Uses hardware-level pre-paging for the specified PageID.
 */
void db_stream_hint(WldNeuralStreamer* streamer, PageID pageId);

/**
 * @brief Predictive Look-Ahead.
 * Given a current node, hint the kernel to load its most likely 
 * navigable neighbors into the cache.
 */
void db_stream_lookahead(WldNeuralStreamer* streamer, PageID currentNodeId, int level);

#endif // WALIA_DB_NEURAL_STREAM_H
