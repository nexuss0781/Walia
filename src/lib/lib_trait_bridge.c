#include <stdio.h>
#include <string.h>

#include "core/common.h"
#include "core/object.h"
#include "core/value.h"
#include "core/vm.h"
#include "native.h"
#include "web/web_traits.h" // Phase 15.4 substrate

// ==========================================
// DYNAMIC COMPOSITION API
// ==========================================

/**
 * mixin(object, trait) -> object
 * 
 * UFO-GRADE LOGIC:
 * 1. Identifies the physical memory layout (Shape) of the object.
 * 2. Inlines the Trait's method table into the object's virtual table.
 * 3. Performs a Shape Transition to maintain O(1) property access.
 */
// Forward declaration for enterprise error handling
extern void runtimeError(const char* format, ...);

static Value walia_native_mixin(int argCount, Value* args) {
    if (argCount != 2) return NIL_VAL;
    
    Value receiver = args[0];
    Value traitVal = args[1];
    
    // 1. Target Validation: Must be a concrete Instance
    if (!IS_INSTANCE(receiver)) {
        runtimeError("Mixin error: Target must be a class instance.");
        return NIL_VAL;
    }

    // 2. Trait Validation: Must be an ObjTrait (OBJ_NATIVE type)
    if (!IS_OBJ(traitVal) || AS_OBJ(traitVal)->type != OBJ_NATIVE) {
        runtimeError("Mixin error: Second argument must be a Trait.");
        return NIL_VAL;
    }

    // 3. Invoke Flattening Engine: Inlines behavior into the Hidden Class
    walia_trait_apply(receiver, (ObjTrait*)AS_OBJ(traitVal));

    return receiver;
}
// ==========================================
// MASTER INITIALIZATION
// ==========================================

void initTraitLibrary() {
    defineNative("mixin", walia_native_mixin);
    printf(">> Walia Composition: Trait Linker active [Flattening Mode]\n");
}
