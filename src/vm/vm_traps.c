#include <stdio.h>
#include "../vm/vm_traps.h"
#include "../web/web_proxy.h"
#include "../core/object.h"

// ==========================================
// SOVEREIGN INTERCEPTION LOGIC
// ==========================================

Value vm_trap_get_property(Value receiver, ObjString* name) {
    // 1. FAST BIT CHECK
    // In Walia, proxies are tagged as OBJ_NATIVE with a specific 
    // internal subtype or flag.
    if (IS_OBJ(receiver) && AS_OBJ(receiver)->type == OBJ_NATIVE) {
        // We cast to ObjProxy to verify the trap availability
        ObjProxy* proxy = (ObjProxy*)AS_OBJ(receiver);
        
        // UFO SPEED: Direct jump to C-native trap handler
        return walia_proxy_get(proxy, name);
    }

    // This path should not be reached if the VM properly branches, 
    // but we return NIL for industrial safety.
    return NIL_VAL;
}

bool vm_trap_set_property(Value receiver, ObjString* name, Value value) {
    if (IS_OBJ(receiver) && AS_OBJ(receiver)->type == OBJ_NATIVE) {
        ObjProxy* proxy = (ObjProxy*)AS_OBJ(receiver);
        
        // Trigger the Reactive Write Barrier + Data Binding
        return walia_proxy_set(proxy, name, value);
    }

    return false;
}
