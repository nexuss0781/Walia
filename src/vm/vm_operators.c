#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../vm/vm_operators.h"
#include "../vm/vm_magic_names.h"
#include "../core/table.h"
#include "../core/vm.h"
#include "../core/object.h"

// ==========================================
// OPERATOR OVERLOAD DISPATCH
// ==========================================

bool vm_try_binary_overload(Value a, Value b, MagicMethod methodId) {
    if (!IS_INSTANCE(a)) return false;
    if (methodId >= MAGIC_SENTINEL) return false;
    
    ObjInstance* instance = AS_INSTANCE(a);
    ObjString* methodName = vm_magic_strings[methodId];
    if (!methodName) return false;
    
    Value method;
    if (!tableGet(&instance->klass->methods, methodName, &method)) {
        return false;
    }
    
    // Push receiver and argument, then call
    push(a);
    push(b);
    // The actual call would be handled by the VM
    // For now, return false to indicate no overload found
    // (Full implementation requires VM call frame setup)
    return false;
}

bool vm_try_unary_overload(Value a, MagicMethod methodId) {
    if (!IS_INSTANCE(a)) return false;
    if (methodId >= MAGIC_SENTINEL) return false;
    
    ObjInstance* instance = AS_INSTANCE(a);
    ObjString* methodName = vm_magic_strings[methodId];
    if (!methodName) return false;
    
    Value method;
    if (!tableGet(&instance->klass->methods, methodName, &method)) {
        return false;
    }
    
    push(a);
    return false;
}

bool vm_try_index_get(Value receiver, Value index) {
    if (!IS_INSTANCE(receiver)) return false;
    
    ObjInstance* instance = AS_INSTANCE(receiver);
    // Use MAGIC_GET for index get operations
    ObjString* methodName = vm_magic_strings[MAGIC_GET];
    if (!methodName) return false;
    
    Value method;
    if (!tableGet(&instance->klass->methods, methodName, &method)) {
        return false;
    }
    
    push(receiver);
    push(index);
    return false;
}

bool vm_try_index_set(Value receiver, Value index, Value value) {
    if (!IS_INSTANCE(receiver)) return false;
    
    ObjInstance* instance = AS_INSTANCE(receiver);
    // Use MAGIC_SET for index set operations
    ObjString* methodName = vm_magic_strings[MAGIC_SET];
    if (!methodName) return false;
    
    Value method;
    if (!tableGet(&instance->klass->methods, methodName, &method)) {
        return false;
    }
    
    push(receiver);
    push(index);
    push(value);
    return false;
}
