#include <stdio.h>
#include <string.h>

#include "../web/wld_event_common.h" // Added for Result Type consistency
#include "../web/web_surface.h"      // Now includes its own dependencies
#include "../core/common.h"
#include "../core/object.h"
#include "../core/value.h"
#include "../core/vm.h"
#include "../native.h"
#include "../web/web_proxy.h"
#include "../web/web_surface.h"

// External Reference for Native Registration
extern void defineNative(const char* name, NativeFn function);

// ==========================================
// PROXY CONSTRUCTOR
// ==========================================

/**
 * Proxy(target, handler) -> ObjProxy
 * 
 * Logic: Wraps a Walia object in a Reactive Interceptor.
 * Usage: var reactiveUser = Proxy(user, { "set": fun(t, k, v) { ... } });
 */
static Value walia_native_proxy_new(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;

    Value target = args[0];
    Value handler = (argCount >= 2) ? args[1] : NIL_VAL;

    // Allocate the proxy on the sovereign heap
    ObjProxy* proxy = walia_proxy_new(target, handler);
    
    return OBJ_VAL(proxy);
}

// ==========================================
// REACTIVE BINDING (The Observer)
// ==========================================

/**
 * observe(proxy, ui_element) -> Bool
 * 
 * UFO-GRADE LOGIC:
 * Physically links the proxy to a UI element's memory address.
 * Any future mutation of the proxy will automatically trigger 
 * the 'markCard' write barrier on the UI element, causing 
 * the hardware rasterizer to refresh that specific tile.
 */
static Value walia_native_observe(int argCount, Value* args) {
    if (argCount != 2) return BOOL_VAL(false);

    // Verify types: Arg0 must be a Proxy, Arg1 must be an ObjElement
    if (!(IS_OBJ(args[0]) && AS_OBJ(args[0])->type == OBJ_NATIVE)) {
        return BOOL_VAL(false);
    }
    
    if (!(IS_OBJ(args[1]) && AS_OBJ(args[1])->type == OBJ_NATIVE)) {
        return BOOL_VAL(false);
    }

    ObjProxy* proxy = (ObjProxy*)AS_OBJ(args[0]);
    void* elementAddr = (void*)AS_OBJ(args[1]);

    // Establish the hardware-linked subscription
    walia_proxy_subscribe(proxy, elementAddr);

    return BOOL_VAL(true);
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

/**
 * @brief Bootstraps the Reactive Standard Library.
 */
void initReactiveLibrary() {
    defineNative("Proxy",   walia_native_proxy_new);
    defineNative("observe", walia_native_observe);
    
    printf(">> Walia Reactive API: Invisible Data Binding active [Sovereign-Link Mode]\n");
}
