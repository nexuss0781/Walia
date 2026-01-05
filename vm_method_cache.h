#ifndef WALIA_VM_METHOD_CACHE_H
#define WALIA_VM_METHOD_CACHE_H

#include "common.h"
#include "value.h"
#include "object.h"

// ==========================================
// POLYMORPHIC INLINE CACHE (PIC)
// ==========================================
// Reduces method lookup from O(n) hash search to O(1) array index.

#define METHOD_CACHE_SIZE 256
#define CACHE_MASK (METHOD_CACHE_SIZE - 1)

typedef struct {
    ObjString* methodName;  // The selector (e.g., "__add__")
    // In a full class system, we would cache 'klass' here.
    // For Phase 13.1, we cache the 'receiver' type tag.
    ObjType receiverType;   // Object type for cache validation
    Value method;           // The resolved closure
} MethodCacheEntry;

/**
 * @brief The Global Method Cache.
 */
extern MethodCacheEntry vm_method_cache[METHOD_CACHE_SIZE];

// ==========================================
// CACHE API
// ==========================================

/**
 * @brief Clears the cache (Invalidation).
 * Called when a class is modified or hot-swapped.
 */
void vm_cache_clear();

/**
 * @brief Fast-Path Lookup.
 * 
 * Logic:
 * 1. Hash the method name to find cache slot.
 * 2. Check if cached receiverType matches current receiver.
 * 3. Return cached method or NIL.
 */
Value vm_cache_get(Value receiver, ObjString* name);

/**
 * @brief Updates the cache after a slow lookup.
 */
void vm_cache_set(Value receiver, ObjString* name, Value method);

#endif // WALIA_VM_METHOD_CACHE_H