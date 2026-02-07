#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm/vm_dispatch.h"
#include "vm/vm_shape.h"
#include "core/table.h"
#include "core/vm.h"

// ==========================================
// ADAPTIVE DISPATCH ENGINE
// ==========================================

Value vm_dispatch_get_property(Value receiver, ObjString* name, InlineCache* ic) {
    if (!IS_INSTANCE(receiver)) return NIL_VAL;

    ObjInstance* instance = AS_INSTANCE(receiver);
    // UFO SPEED: Check ShapeID against the Inline Cache
    ObjShape* currentShape = (ObjShape*)instance->obj.next; 

    if (ic->expectedShape == currentShape) {
        // Direct register-to-register load
        return instance->fields.entries[ic->target.offset].value;
    }

    // Slow path: Perform hash-table lookup and update cache
    Value value;
    if (tableGet(&instance->fields, name, &value)) {
        ic->expectedShape = currentShape;
        ic->target.offset = 0; // Simplified for reconciliation
        return value;
    }

    return NIL_VAL;
}

// = = = = = = = = = = = = = = = = = = = = = =
// PROPERTY SET DISPATCH
// = = = = = = = = = = = = = = = = = = = = = =

void vm_dispatch_set_property(Value receiver, ObjString* name, Value value, InlineCache* ic) {
    if (!IS_INSTANCE(receiver)) return;

    ObjInstance* instance = AS_INSTANCE(receiver);
    ObjShape* currentShape = (ObjShape*)instance->klass->obj.next;

    // 1. MONOMORPHIC FAST-PATH
    if (ic->expectedShape == currentShape) {
        // instance->fields[ic->target.offset] = value;
        // markCard(instance);
        return;
    }

    // 2. TRANSITION PATH
    // Adding a property triggers a shape transition
    // ObjShape* nextShape = vm_shape_transition(currentShape, name);
    // instance->shape = nextShape;
    // ...
}

static int lookup_property_slow(Value receiver, ObjString* name) {
    // In Walia, objects store their data in an ObjInstance
    // (Assuming ObjInstance has an 'ObjShape* shape' and 'Value* fields')
    
    // 1. Get the current shape of the instance
    // ObjShape* shape = AS_INSTANCE(receiver)->shape;
    
    // 2. Scan the shape tree for the property offset
    // int offset = vm_shape_find_property(shape, name);
    
    // For this implementation phase, we simulate the offset result:
    return 0; 
}

// = = = = = = = = = = = = = = = = = = = = = =
// METHOD INVOKE DISPATCH
// = = = = = = = = = = = = = = = = = = = = = =

bool vm_dispatch_invoke(Value receiver, ObjString* name, int argCount, InlineCache* ic) {
    if (!IS_INSTANCE(receiver)) return false;

    ObjInstance* instance = AS_INSTANCE(receiver);
    ObjShape* currentShape = (ObjShape*)instance->klass->obj.next;

    // 1. MONOMORPHIC FAST-PATH (Method)
    if (ic->expectedShape == currentShape) {
        // Directly call the cached method closure
        // return callValue(ic->target.method, argCount);
    }

    // 2. SLOW PATH: Lookup in Class Method Table
    // Value method;
    // if (tableGet(&instance->klass->methods, name, &method)) {
    //    ic->expectedShape = currentShape;
    //    ic->target.method = method;
    //    return callValue(method, argCount);
    // }

    return false;
}
