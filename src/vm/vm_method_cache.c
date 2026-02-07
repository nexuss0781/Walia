#include "vm/vm_method_cache.h"
#include "core/value.h"

MethodCacheEntry vm_method_cache[METHOD_CACHE_SIZE];

void vm_cache_clear() {
    for (int i = 0; i < METHOD_CACHE_SIZE; i++) {
        vm_method_cache[i].methodName = NULL;
        vm_method_cache[i].method = NIL_VAL;
    }
}

Value vm_cache_get(Value receiver, ObjString* name) {
    // Simple hash index based on method name pointer address
    uint32_t index = name->hash & CACHE_MASK;
    MethodCacheEntry* entry = &vm_method_cache[index];

    // Check if cache hit
    // Note: We use raw type check. In full OOP, check Klass ID.
#ifdef NAN_BOXING
    // For NaN boxing, we don't have a simple 'type' enum for everything.
    // We skip cache for primitives in Phase 13.1, only cache Objects.
    if (!IS_OBJ(receiver)) return NIL_VAL;
    
    if (entry->methodName == name && 
        IS_OBJ(entry->method) && // Verify method is still valid
        OBJ_TYPE(receiver) == OBJ_CLOSURE) { // Simplified check
        return entry->method;
    }
#else
    if (entry->methodName == name && entry->receiverType == receiver.type) {
        return entry->method;
    }
#endif

    return NIL_VAL;
}

void vm_cache_set(Value receiver, ObjString* name, Value method) {
    uint32_t index = name->hash & CACHE_MASK;
    MethodCacheEntry* entry = &vm_method_cache[index];

    entry->methodName = name;
    entry->method = method;
#ifndef NAN_BOXING
    entry->receiverType = receiver.type;
#endif
}
