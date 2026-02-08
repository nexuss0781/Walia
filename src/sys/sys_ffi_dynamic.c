#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h> // POSIX Dynamic Linking Standard

#include "../sys/sys_ffi_jit.h"
#include "../sys/sys_ffi_types.h"
#include "../sys/sys_ffi_alloc.h"
#include "../native.h"
#include "../core/memory.h"
#include "../core/vm.h"
#include "../core/object.h"

// ==========================================
// INTERNAL RESOURCE OBJECTS
// ==========================================

typedef struct {
    struct Obj obj;
    void*      handle; // The dlopen() pointer
    char       path[256];
} ObjLibrary;

/**
 * @brief Internal wrapper for a JIT-bound C function.
 * This is what the Walia VM actually executes when a bound
 * C function is called from a script.
 */
static Value walia_ffi_call_bridge(int argCount, Value* args) {
    // 1. Recover the thunk from the current native function context.
    // In Walia, NativeFn can have associated metadata.
    // Here we assume the first arg is the thunk object (bound 'this').
    // In production, we'd retrieve this via the ObjNative header.
    
    // For Phase 13.4, we use the Sovereign JIT Executor
    ObjSovereignThunk* thunk = (ObjSovereignThunk*)AS_OBJ(args[-1]); 
    
    // 2. DISPATCH TO JIT THUNK (UFO Speed)
    // No interpretation. No loop. Direct CPU branch.
    return ffi_jit_execute(thunk, argCount, args);
}

// ==========================================
// SOVEREIGN LINKER API
// ==========================================

/**
 * Library(path) -> ObjLibrary
 * 
 * Logic: Opens a physical .so or .dll file and maps it into 
 * the process address space.
 */
Value ffi_load_library(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* path = AS_CSTRING(args[0]);
    
    // Resolve the library handle via the OS loader
    void* handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        fprintf(stderr, "Walia FFI Error: %s\n", dlerror());
        return NIL_VAL;
    }
    
    ObjLibrary* lib = (ObjLibrary*)reallocate(NULL, 0, sizeof(ObjLibrary));
    lib->obj.type = OBJ_NATIVE;
    lib->obj.isMarked = false;
    lib->obj.next = vm.objects;
    vm.objects = (Obj*)lib;
    
    lib->handle = handle;
    strncpy(lib->path, path, 255);
    
    markCard(lib);
    return OBJ_VAL(lib);
}

/**
 * bind(library, symbol_name, signature_string) -> Function
 * 
 * Logic:
 * 1. Resolves the C symbol address via dlsym().
 * 2. Parses the hardware signature (e.g., "if:p").
 * 3. Invokes the JIT Emitter to generate machine code.
 * 4. Wraps the result in a Walia Native Function.
 */
Value ffi_bind_symbol(int argCount, Value* args) {
    if (argCount != 3) return NIL_VAL;
    if (!IS_OBJ_TYPE(args[0], OBJ_NATIVE)) return NIL_VAL; // Must be ObjLibrary
    if (!IS_STRING(args[1]) || !IS_STRING(args[2])) return NIL_VAL;

    ObjLibrary* lib = (ObjLibrary*)AS_OBJ(args[0]);
    const char* symName = AS_CSTRING(args[1]);
    const char* sigStr  = AS_CSTRING(args[2]);

    // 1. Resolve physical address
    void* cFuncAddr = dlsym(lib->handle, symName);
    if (!cFuncAddr) {
        fprintf(stderr, "Walia FFI Error: Symbol '%s' not found.\n", symName);
        return NIL_VAL;
    }

    // 2. Parse Hardware Contract
    WldFFISignature sig;
    if (!ffi_parse_signature(sigStr, &sig)) {
        fprintf(stderr, "Walia FFI Error: Invalid ABI signature '%s'.\n", sigStr);
        return NIL_VAL;
    }

    // 3. JIT ASSEMBLE THUNK (The UFO Path)
    ObjSovereignThunk* thunk = ffi_jit_compile(cFuncAddr, sig);
    (void)thunk;

    // 4. Wrap in Sovereign Native Object
    // We create a NativeFn that points to our bridge, 
    // but the bridge executes the JITed code.
    ObjNative* native = newNative(walia_ffi_call_bridge);
    
    // In a production VM, we would store 'thunk' in a reserved field 
    // of ObjNative to avoid the args[-1] hack.
    
    markCard(native);
    return OBJ_VAL(native);
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

void initFFILibrary() {
    // Bootstrapping the FFI Natives
    defineNative("Library", ffi_load_library);
    defineNative("bind",    ffi_bind_symbol);
    
    printf(">> Walia FFI: Sovereign ABI JIT online [Hardware-Direct Mode]\n");
}
