#ifndef WALIA_WEB_PROXY_H
#define WALIA_WEB_PROXY_H

#include "core/common.h"
#include "core/object.h"
#include "core/value.h"

// ==========================================
// PROXY SPECIFICATIONS
// ==========================================

/**
 * @brief The Sovereign Proxy Object.
 * 
 * Architecture:
 * Wraps a target object and a 'handler' (usually a Map or Closure).
 * It maintains a 'subscriber' list of memory addresses that must 
 * be marked dirty when this proxy is mutated.
 */
typedef struct {
    struct Obj obj;
    Value      target;      // The actual data object (Instance/Map/List)
    Value      handler;     // The trap logic (Map of closures)
    
    // Hardware-Linked Subscriptions
    void**     subscribers; // Array of persistent memory addresses (e.g., ObjElement*)
    int        subCount;
    int        subCapacity;
} ObjProxy;

// ==========================================
// PROXY API
// ==========================================

/**
 * @brief Allocates a new Reactive Proxy on the persistent heap.
 */
ObjProxy* walia_proxy_new(Value target, Value handler);

/**
 * @brief Standard 'SET' Interceptor.
 * 
 * Logic:
 * 1. Checks handler for a 'set' trap.
 * 2. If present, executes the trap logic.
 * 3. If absent, performs a default write to the target.
 * 4. AUTOMATIC REACTION: Invokes markCard() on all subscribers.
 */
bool walia_proxy_set(ObjProxy* proxy, ObjString* name, Value value);

/**
 * @brief Standard 'GET' Interceptor.
 */
Value walia_proxy_get(ObjProxy* proxy, ObjString* name);

/**
 * @brief Links a memory-mapped resource to this proxy.
 * Used by the Sentry ORM to connect variables to UI elements.
 */
void walia_proxy_subscribe(ObjProxy* proxy, void* memoryAddress);

#endif // WALIA_WEB_PROXY_H
