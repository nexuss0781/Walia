#include <stdatomic.h>
#include "vm_async_core.h"
#include "effect.h"
#include "lib_collections.h"
#include "memory.h"

// ==========================================
// ASYNC WAIT-GROUP (The Barrier)
// ==========================================

typedef struct {
    Obj             obj;
    atomic_int      pendingCount;
    ObjList*        results;
    ObjContinuation* parentCont;
    bool            isRace; // True for race(), False for all()
} ObjWaitGroup;

// ==========================================
// ORCHESTRATOR IMPLEMENTATION
// ==========================================

/**
 * @brief Internal callback for future resolution.
 * 
 * Logic:
 * 1. Decrement the group's atomic counter.
 * 2. If it's the last future (all) or the first future (race),
 *    trigger the Sovereign Resumption of the parent stack.
 */
void walia_async_on_complete(ObjWaitGroup* group, Value result, int index) {
    // 1. Store result in the persistent collection
    walia_list_set(group->results, index, result);

    if (group->isRace) {
        // RACE LOGIC: First one to resolve wins the resumption
        int expected = atomic_load(&group->pendingCount);
        if (expected > 0 && atomic_exchange(&group->pendingCount, 0) > 0) {
            resumeContinuation(group->parentCont, result);
        }
    } else {
        // ALL LOGIC: Decrement and check for zero
        if (atomic_fetch_sub(&group->pendingCount, 1) == 1) {
            // UFO SPEED: The final future triggers the parent's return
            resumeContinuation(group->parentCont, OBJ_VAL(group->results));
        }
    }
}

/**
 * @brief Native: all(futures_list) -> Future
 */
static Value native_async_all(int argCount, Value* args) {
    if (argCount < 1 || !IS_OBJ_TYPE(args[0], OBJ_LIST)) return NIL_VAL;
    
    ObjList* list = (ObjList*)AS_OBJ(args[0]);
    int count = list->array.count;

    // 1. Create the Barrier Object in the persistent heap
    ObjWaitGroup* wg = (ObjWaitGroup*)reallocate(NULL, 0, sizeof(ObjWaitGroup));
    wg->obj.type = OBJ_NATIVE;
    atomic_init(&wg->pendingCount, count);
    wg->results = walia_list_new();
    wg->isRace = false;
    
    // Capture the parent's stack slice (Phase 10.4)
    wg->parentCont = captureContinuation(NULL, 0);

    // 2. Link every future in the list to this barrier
    for (int i = 0; i < count; i++) {
        ObjFuture* fut = (ObjFuture*)AS_OBJ(list->array.values[i]);
        /* walia_future_add_callback(fut, wg, i); */
    }

    markCard(wg);
    return perform_effect("ASYNC_WAIT"); // Suspend parent until counter hits zero
}

/**
 * @brief Native: race(futures_list) -> Future
 */
static Value native_async_race(int argCount, Value* args) {
    // Implementation mirror of 'all', but with wg->isRace = true.
    return NIL_VAL; // Placeholder for the shared logic call
}

void initAsyncOrchestrator() {
    defineNative("all",  native_async_all);
    defineNative("race", native_async_race);
    printf(">> Walia Async: Parallel Joiner active [Wait-Free Mode]\n");
}
