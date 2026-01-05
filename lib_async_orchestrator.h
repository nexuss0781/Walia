#ifndef WALIA_LIB_ASYNC_ORCHESTRATOR_H
#define WALIA_LIB_ASYNC_ORCHESTRATOR_H

#include "common.h"
#include "object.h"
#include "value.h"
#include "vm_async_core.h"
#include <stdatomic.h>

// ==========================================
// WAIT GROUP SPECIFICATION
// ==========================================

typedef enum {
    MODE_ALL,
    MODE_RACE
} WaitMode;

/**
 * @brief Persistent synchronization primitive for parallel futures.
 */
typedef struct {
    struct Obj obj;
    
    WaitMode   mode;
    atomic_int pendingCount;
    int        totalCount;
    
    // The result set (for MODE_ALL) or winning value (for MODE_RACE)
    Value*     results; 
    
    // The master continuation to resume when the group is satisfied
    struct ObjContinuation* masterContinuation;
    
    atomic_bool isSatisfied;
} ObjWaitGroup;

// ==========================================
// ORCHESTRATOR API
// ==========================================

/**
 * @brief all(List<Future>) -> Future
 * 
 * Logic:
 * 1. Creates an ObjWaitGroup in MODE_ALL.
 * 2. Attaches the group to every future in the list.
 * 3. Suspends the current logic stack.
 */
Value walia_async_all(int argCount, Value* args);

/**
 * @brief race(List<Future>) -> Future
 */
Value walia_async_race(int argCount, Value* args);

/**
 * @brief Internal callback triggered when a member of a group resolves.
 */
void walia_waitgroup_notify(ObjWaitGroup* group, Value result, int index);

#endif // WALIA_LIB_ASYNC_ORCHESTRATOR_H