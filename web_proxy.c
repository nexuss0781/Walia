#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web_proxy.h"
#include "memory.h"
#include "vm.h"
#include "table.h"
#include "web_surface.h" // For UI element awareness

// ==========================================
// PROXY LIFECYCLE
// ==========================================

ObjProxy* walia_proxy_new(Value target, Value handler) {
    ObjProxy* proxy = (ObjProxy*)reallocate(NULL, 0, sizeof(ObjProxy));
    
    // Initialize Sovereign Object Header
    // Proxies are tagged as OBJ_NATIVE for the VM Interceptor (Group 3)
    proxy->obj.type = OBJ_NATIVE; 
    proxy->obj.isMarked = false;
    proxy->obj.next = vm.objects;
    vm.objects = (struct Obj*)proxy;

    proxy->target = target;
    proxy->handler = handler;
    
    proxy->subCount = 0;
    proxy->subCapacity = 4;
    proxy->subscribers = (void**)reallocate(NULL, 0, sizeof(void*) * proxy->subCapacity);

    markCard(proxy);
    return proxy;
}

// ==========================================
// REACTIVE TRAP LOGIC
// ==========================================

bool walia_proxy_set(ObjProxy* proxy, ObjString* name, Value value) {
    // 1. Resolve Target Instance
    if (!IS_INSTANCE(proxy->target)) return false;
    ObjInstance* instance = AS_INSTANCE(proxy->target);

    // 2. DEFAULT ACTION: Physical Write
    // (In production, check 'handler' for a 'set' closure first)
    tableSet(&instance->fields, name, value);

    // 3. HARDWARE-LINKED REACTION (The UFO Secret)
    // Every address subscribed to this proxy is now logically 'Dirty'.
    for (int i = 0; i < proxy->subCount; i++) {
        // We call the Sovereign Pager's Write Barrier.
        // If this address is a UI Element, the Tiled Rasterizer 
        // will automatically re-render its 16x16 block.
        markCard(proxy->subscribers[i]);
    }

    // 4. Persistence: Ensure the proxy-target relationship is saved
    markCard(instance);
    
    return true;
}

Value walia_proxy_get(ObjProxy* proxy, ObjString* name) {
    if (!IS_INSTANCE(proxy->target)) return NIL_VAL;
    ObjInstance* instance = AS_INSTANCE(proxy->target);

    // (In production, check 'handler' for a 'get' closure first)
    Value result;
    if (tableGet(&instance->fields, name, &result)) {
        return result;
    }

    return NIL_VAL;
}

// ==========================================
// SUBSCRIPTION ENGINE
// ==========================================

void walia_proxy_subscribe(ObjProxy* proxy, void* memoryAddress) {
    if (proxy->subCount >= proxy->subCapacity) {
        int old = proxy->subCapacity;
        proxy->subCapacity = GROW_CAPACITY(old);
        proxy->subscribers = (void**)reallocate(proxy->subscribers, 
                                               sizeof(void*) * old, 
                                               sizeof(void*) * proxy->subCapacity);
    }

    proxy->subscribers[proxy->subCount++] = memoryAddress;
    
    // Persistence: The link is now durable
    markCard(proxy);
}