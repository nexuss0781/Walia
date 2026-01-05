#ifndef WALIA_DB_DISPATCH_H
#define WALIA_DB_DISPATCH_H

#include "db_common.h"
#include <pthread.h>

// ==========================================
// DISPATCHER CONFIGURATION
// ==========================================

#define WLD_MAX_THREADS 64           // Support for massive multi-core servers
#define WLD_PAGES_PER_TASK 1024      // Sovereign Work Unit size
#define WLD_TASK_QUEUE_SIZE 1024

/**
 * @brief Definition of a Query Task.
 * Represents a compiled Walia Bytecode logic applied to a page range.
 */
typedef struct {
    PageID   startPage;
    PageID   endPage;
    void*    queryCtx;  // Pointer to the SQE JIT-compiled logic
    void*    results;   // Sovereign Result Buffer (Thread-safe)
} WldTask;

/**
 * @brief The Parallel Worker.
 */
typedef struct {
    pthread_t thread;
    int       workerId;
    bool      isActive;
    WldTask   localQueue[WLD_TASK_QUEUE_SIZE];
    int       queueHead;
    int       queueTail;
} WldWorker;

/**
 * @brief The Sovereign Dispatcher.
 */
typedef struct {
    WldWorker workers[WLD_MAX_THREADS];
    int       threadCount;
    bool      isShutdown;
    
    pthread_mutex_t globalLock;
    pthread_cond_t  workAvailable;
} WldDispatcher;

// ==========================================
// PARALLEL API
// ==========================================

/**
 * @brief Initializes the Dispatcher based on available CPU cores.
 */
void db_dispatch_init(WldDispatcher* d, int threads);

/**
 * @brief Orchestrates a Parallel Scan across a table or collection.
 * Divides the entire logical page range into 1024-page chunks.
 */
void db_dispatch_parallel_scan(WldDispatcher* d, PageID root, uint32_t totalPages, void* queryCtx);

/**
 * @brief Gracefully shuts down the thread pool.
 */
void db_dispatch_shutdown(WldDispatcher* d);

#endif // WALIA_DB_DISPATCH_H
