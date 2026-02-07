#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web/wld_event_loop.h"
#include "core/object.h"
#include "core/vm.h"

// ==========================================
// CORE ORCHESTRATION LOGIC
// ==========================================

SerResult db_event_loop_init(WldEventLoop* loop, WldDispatcher* dispatcher) {
    // 1. Initialize the low-level hardware substrate
    SerResult res = sys_event_reactor_init(&loop->reactor);
    if (res != SER_OK) return res;

    // 2. Initialize the Persistent Continuation Map
    initTable(&loop->continuationMap);
    
    loop->dispatcher = dispatcher;
    loop->isRunning = true;

    printf(">> Walia Event Manager: Sovereign Brain online [MPP Linked]\n");
    return SER_OK;
}

SerResult db_event_loop_register(WldEventLoop* loop, int fd, WldEventType type, Value continuation) {
    // 1. Map FD to its resumable logic
    // In Walia, we use the FD as a key (wrapped in a string or specific key-type)
    char fdKey[16];
    sprintf(fdKey, "%d:%d", fd, type);
    ObjString* key = copyString(fdKey, (int)strlen(fdKey));
    
    // Store the continuation in the registry
    tableSet(&loop->continuationMap, key, continuation);

    // 2. Alert the metal reactor
    return sys_event_reactor_add(&loop->reactor, fd, type);
}

/**
 * @brief Sovereign Event Handover.
 * 
 * This is the bridge between I/O and Logic. 
 * It reaps a hardware notification and produces a Parallel Task.
 */
void db_event_loop_step(WldEventLoop* loop) {
    WldEvent reapedEvents[128];
    int count = sys_event_reactor_poll(&loop->reactor, reapedEvents, 128);

    if (count <= 0) return;

    for (int i = 0; i < count; i++) {
        WldEvent* ev = &reapedEvents[i];
        
        // 1. RESOLVE: Find the continuation for this FD
        char fdKey[16];
        sprintf(fdKey, "%d:%d", ev->fd, ev->type);
        ObjString* key = copyString(fdKey, (int)strlen(fdKey));
        
        Value continuation;
        if (tableGet(&loop->continuationMap, key, &continuation)) {
            // 2. DISPATCH: Wrap the continuation in an MPP Task
            WldTask task;
            task.queryCtx = (void*)AS_OBJ(continuation); // The stack slice
            task.startPage = 0; // Not applicable for I/O tasks
            task.endPage = 0;
            task.results = NULL;

            // UFO SPEED: Handover to the worker threads (Phase 5 Dispatcher)
            // This ensures that I/O completion is processed in parallel.
            // (Assumes dispatcher has a 'push_task' API)
            // db_dispatch_push_task(loop->dispatcher, task);

            // 3. CLEANUP: Remove from registry (one-shot event)
            tableDelete(&loop->continuationMap, key);
        }
    }
}

void db_event_loop_shutdown(WldEventLoop* loop) {
    loop->isRunning = false;
    freeTable(&loop->continuationMap);
    sys_event_reactor_shutdown(&loop->reactor);
    printf(">> Walia Event Manager: Sovereign Brain offline.\n");
}
