#include <stdio.h>
#include <sys/mman.h>

#include "db_neural_stream.h"
#include "db_hnsw.h"

// = = = = = = = = = = = = = = = = = = = = = =
// HARDWARE PRE-FETCH LOGIC
// = = = = = = = = = = = = = = = = = = = = = =

void db_stream_init(WldNeuralStreamer* streamer, WldPager* pager) {
    streamer->pager = pager;
    streamer->hintCount = 0;
}

/**
 * @brief Sovereign Kernel Hinting.
 * Leverages posix_madvise to prepare the OS Page Table.
 */
void db_stream_hint(WldNeuralStreamer* streamer, PageID pageId) {
    if (pageId == WLD_INVALID_PAGE) return;

    // 1. Calculate memory address of the page in the mmap'd region
    void* addr = (void*)(streamer->pager->fileMapping + (pageId * WLD_PAGE_SIZE));

    // 2. Issue MADV_WILLNEED (Non-blocking)
    // This tells the kernel to start moving this 4KB block from disk to RAM
    // in the background without pausing the VM thread.
#ifdef __linux__
    posix_madvise(addr, WLD_PAGE_SIZE, POSIX_MADV_WILLNEED);
#endif

    // 3. (Optional) Software Prefetch Intrinsics
    // For even lower latency, we hint the CPU cache controller directly.
#if defined(__x86_64__)
    __builtin_prefetch(addr, 0, 3); // 0=Read, 3=High Locality (keep in all cache levels)
#endif
}

// = = = = = = = = = = = = = = = = = = = = = =
// PREDICTIVE TRAVERSAL
// = = = = = = = = = = = = = = = = = = = = = =

void db_stream_lookahead(WldNeuralStreamer* streamer, PageID currentNodeId, int level) {
    // 1. Acquire the HNSW Node metadata
    // We assume the node's neighbors are stored on a page adjacent to its vector.
    WldHNSWNode* node = (WldHNSWNode*)(streamer->pager->fileMapping + ((currentNodeId + 1) * WLD_PAGE_SIZE));

    // 2. Hint the top-N neighbors
    // In HNSW, the first neighbors in the list are usually the most relevant.
    int limit = node->neighborCount[level];
    if (limit > WLD_STREAM_LOOKAHEAD) limit = WLD_STREAM_LOOKAHEAD;

    for (int i = 0; i < limit; i++) {
        PageID neighborId = node->neighbors[level][i];
        db_stream_hint(streamer, neighborId);
    }
}
