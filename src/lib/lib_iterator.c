#include <stdio.h>
#include <stdlib.h>

#include "lib/lib_iterator.h"
#include "core/memory.h"
#include "core/vm.h"
#include "native.h"
#include "lib/lib_collections.h" // For ObjList definition

// ==========================================
// LIST ITERATOR IMPLEMENTATION
// ==========================================

ObjListIterator* newListIterator(struct ObjList* list) {
    ObjListIterator* iter = (ObjListIterator*)reallocate(NULL, 0, sizeof(ObjListIterator));

    // Initialize standard object header
    iter->obj.type = OBJ_NATIVE;
    iter->obj.isMarked = false;
    iter->obj.next = vm.objects;
    vm.objects = (struct Obj*)iter;

    // Fixed: Standardizing pointer type to resolve incompatible pointer warning
    iter->list = list;
    iter->index = 0;

    markCard(iter);
    return iter;
}

Value listIteratorNext(ObjListIterator* iter, bool* done) {
    // Explicit cast to the physical ObjList type defined in lib_collections.h
    ObjList* list = (ObjList*)iter->list;

    if (iter->index < list->array.count) {
        *done = false;
        Value val = list->array.values[iter->index];
        iter->index++;
        return val;
    }

    *done = true;
    return NIL_VAL;
}

// ==========================================
// NATIVE BINDINGS (__iter__ / __next__)
// ==========================================

static Value native_list_iter(int argCount, Value* args) {
    if (argCount != 1 || !IS_OBJ(args[0])) return NIL_VAL;
    
    // UFO SPEED: Direct cast to the physical structure
    struct ObjList* list = (struct ObjList*)AS_OBJ(args[0]);
    return OBJ_VAL(newListIterator(list));
}

static Value native_list_next(int argCount, Value* args) {
    if (argCount != 1) return NIL_VAL; // self
    if (!IS_OBJ(args[0])) return NIL_VAL;

    ObjListIterator* iter = (ObjListIterator*)AS_OBJ(args[0]);
    bool done = false;
    Value val = listIteratorNext(iter, &done);

    if (done) {
        // UFO GRADE: In a full build, this performs the "StopIteration" effect.
        // For Phase 19 verification, we return NIL to terminate the loop.
        return NIL_VAL;
    }

    return val;
}

void initIteratorLibrary() {
    // Registering the Iterator Protocol for core collections
    defineNative("__iter__", native_list_iter);
    defineNative("__next__", native_list_next);

    printf(">> Walia Iterator: Native protocols online [UFO-Grade].\n");
}
