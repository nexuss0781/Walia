
#ifndef WALIA_VM_ASYNC_CORE_H
#define WALIA_VM_ASYNC_CORE_H

#include "common.h"
#include "value.h"
#include "object.h"

// ==========================================
// SOVEREIGN FUTURE SPECIFICATION
// ==========================================

typedef enum {
    FUTURE_PENDING = 0,
    FUTURE_RESOLVED = 1,
    FUTURE_FAULTED = 2
} FutureStatus;

/**
 * @brief The Sovereign Future.
 * A persistent anchor for a continuation.
 */
typedef struct {
    Obj          obj;
    FutureStatus status;
    Value        result;    // Data or Error context
    
    // The link to the suspended logic
    struct ObjContinuation* waiter; 
} ObjFuture;

// ==========================================
// ASYNC SUBSTRATE API
// ==========================================

/**
 * @brief Allocates a fresh future in the Sovereign segment.
 */
ObjFuture* walia_future_new();

/**
 * @brief Transitions a future to the RESOLVED state and triggers resumption.
 */
void walia_future_resolve(ObjFuture* future, Value result);

/**
 * @brief Transitions a future to the FAULTED state.
 */
void walia_future_fail(ObjFuture* future, Value error);

/**
 * @brief Attaches a continuation to a pending future.
 */
void walia_future_add_waiter(ObjFuture* future, struct ObjContinuation* continuation);

#endif // WALIA_VM_ASYNC_CORE_H
