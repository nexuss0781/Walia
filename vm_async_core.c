#include <stdio.h>
#include <stdlib.h>

#include "vm_async_core.h"
#include "memory.h"
#include "vm.h"
#include "effect.h"
#include "tooling_nexus_bus.h"
#include "lib_async_orchestrator.h"

// ==========================================
// FUTURE LIFECYCLE
// ==========================================

ObjFuture* walia_future_new() {
    ObjFuture* future = (ObjFuture*)reallocate(NULL, 0, sizeof(ObjFuture));
    
    // Initialize Sovereign Object Header
    future->obj.type = OBJ_NATIVE; 
    future->obj.isMarked = false;
    future->obj.next = vm.objects;
    vm.objects = (struct Obj*)future;

    future->status = FUTURE_PENDING;
    future->result = NIL_VAL;
    future->waiter = NULL;
    future->groupOwner = NULL;
    future->groupIndex = 0;

    markCard(future);
    return future;
}

// ==========================================
// RESOLUTION ENGINE (MPP Handover)
// ==========================================

void walia_future_resolve(ObjFuture* future, Value result) {
    if (future->status != FUTURE_PENDING) return;

    future->result = result;
    future->status = FUTURE_RESOLVED;
    markCard(future);

    // PHASE 19.1: WAIT GROUP NOTIFICATION
    // If this future is part of a collective 'all' or 'race' call
    if (future->groupOwner != NULL) {
        walia_waitgroup_notify(future->groupOwner, result, future->groupIndex);
    }

    // Standard Single-Await Resumption
    if (future->waiter != NULL) {
        walia_pulse_emit(PULSE_RESUME_CONTINUATION, (uint32_t)future->waiter, 0);
    }
}

void walia_future_fail(ObjFuture* future, Value error) {
    if (future->status != FUTURE_PENDING) return;

    future->result = error;
    future->status = FUTURE_FAULTED;
    markCard(future);

    if (future->waiter != NULL) {
        // Resume with the error value; the VM will handle the 
        // Algebraic Effect 'Fault' propagation.
        walia_pulse_emit(PULSE_RESUME_CONTINUATION, (uint32_t)future->waiter, 1);
    }
}

// ==========================================
// SUSPENSION LOGIC
// ==========================================

void walia_future_add_waiter(ObjFuture* future, struct ObjContinuation* continuation) {
    // Current limitation: 1 waiter per future for Phase 15.2.
    // Future expansion: dynamic list of waiters for 'all' / 'race' logic.
    future->waiter = continuation;
    markCard(future);
}

void walia_future_add_group_waiter(ObjFuture* future, void* group, int index) {
    // Associate this future with a wait group (for all/race operations)
    future->groupOwner = group;
    future->groupIndex = index;
    markCard(future);
}