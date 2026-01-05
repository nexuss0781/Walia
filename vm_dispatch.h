#ifndef WALIA_VM_DISPATCH_H
#define WALIA_VM_DISPATCH_H

#include "common.h"
#include "value.h"
#include "object.h"
#include "vm_shape.h"

// ==========================================
// INLINE CACHE (IC) STRUCTURES
// ==========================================

/**
 * @brief A single Monomorphic Cache slot.
 * Matches a specific Shape to a physical memory offset or method pointer.
 */
typedef struct {
    ObjShape* expectedShape; // The "Hidden Class" we are optimized for
    union {
        int   offset;        // For property access (GET/SET)
        Value method;        // For method invocation (INVOKE)
    } target;
} InlineCache;

/**
 * @brief A Polymorphic Cache entry.
 * Used when a single call site encounters multiple different object shapes.
 */
#define POLYMORPHIC_MAX 4
typedef struct {
    InlineCache slots[POLYMORPHIC_MAX];
    int count;
} PolymorphicCache;

// ==========================================
// DISPATCH ENGINE API
// ==========================================

/**
 * @brief UFO-Grade Property Access (The Fast Path).
 * 
 * Logic:
 * 1. Check if the object's shape matches the cached 'monomorphic' shape.
 * 2. If match: return the value at 'offset' instantly.
 * 3. If miss: perform a slow-path lookup and update the cache (Transition to Polymorphic).
 */
Value vm_dispatch_get_property(Value receiver, ObjString* name, InlineCache* ic);

/**
 * @brief Optimized Property Mutation.
 */
void vm_dispatch_set_property(Value receiver, ObjString* name, Value value, InlineCache* ic);

/**
 * @brief High-Speed Method Invocation.
 */
bool vm_dispatch_invoke(Value receiver, ObjString* name, int argCount, InlineCache* ic);

#endif // WALIA_VM_DISPATCH_H