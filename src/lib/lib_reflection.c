#include <stdio.h>
#include <string.h>

#include "../lib/lib_reflection.h"
#include "../core/object.h"
#include "../core/table.h"
#include "../core/vm.h"
#include "../native.h"

// ==========================================
// INTROSPECTION PRIMITIVES
// ==========================================

/**
 * getattr(obj, name, default=nil) -> Value
 */
static Value native_getattr(int argCount, Value* args) {
    if (argCount < 2) return NIL_VAL;
    Value obj = args[0];
    
    if (!IS_STRING(args[1])) return NIL_VAL;
    ObjString* name = AS_STRING(args[1]);
    
    Value result;
    
    // 1. Instance Field Lookup
    if (IS_INSTANCE(obj)) {
        ObjInstance* inst = AS_INSTANCE(obj);
        if (tableGet(&inst->fields, name, &result)) return result;
        
        // 2. Bound Method Lookup
        Value method;
        if (tableGet(&inst->klass->methods, name, &method)) {
            // Return bound method (Phase 14 logic)
            // return OBJ_VAL(newBoundMethod(obj, AS_CLOSURE(method)));
            // For now, return raw closure
            return method;
        }
    }
    
    // 3. Module/Global Lookup (if obj is nil/global placeholder)
    // (Omitted for brevity)

    // 4. Default Value
    if (argCount == 3) return args[2];
    
    // 5. Error
    // runtimeError("Attribute '%s' not found.", name->chars);
    return NIL_VAL;
}

/**
 * setattr(obj, name, value) -> Bool
 */
static Value native_setattr(int argCount, Value* args) {
    if (argCount != 3) return BOOL_VAL(false);
    Value obj = args[0];
    
    if (!IS_STRING(args[1])) return BOOL_VAL(false);
    ObjString* name = AS_STRING(args[1]);
    Value val = args[2];
    
    if (IS_INSTANCE(obj)) {
        ObjInstance* inst = AS_INSTANCE(obj);
        tableSet(&inst->fields, name, val);
        return BOOL_VAL(true);
    }
    
    return BOOL_VAL(false);
}

/**
 * hasattr(obj, name) -> Bool
 */
static Value native_hasattr(int argCount, Value* args) {
    if (argCount != 2) return BOOL_VAL(false);
    Value obj = args[0];
    if (!IS_STRING(args[1])) return BOOL_VAL(false);
    ObjString* name = AS_STRING(args[1]);
    
    if (IS_INSTANCE(obj)) {
        ObjInstance* inst = AS_INSTANCE(obj);
        Value dummy;
        if (tableGet(&inst->fields, name, &dummy)) return BOOL_VAL(true);
        if (tableGet(&inst->klass->methods, name, &dummy)) return BOOL_VAL(true);
    }
    
    return BOOL_VAL(false);
}

// ==========================================
// BOOTSTRAP
// ==========================================

void initReflectionLibrary() {
    defineNative("getattr", native_getattr);
    defineNative("setattr", native_setattr);
    defineNative("hasattr", native_hasattr);
    printf(">> Walia Reflection: Dynamic introspection online.\n");
}
