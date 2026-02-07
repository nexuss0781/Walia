#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "table.h"
#include "effect.h"

void markObject(struct Obj* object) {
    if (object == NULL || object->isMarked) return;
    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = (struct Obj**)realloc(vm.grayStack, sizeof(struct Obj*) * vm.grayCapacity);
        if (vm.grayStack == NULL) exit(1);
    }
    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) markValue(array->values[i]);
}

static void blackenObject(struct Obj* object) {
    switch (object->type) {
        case OBJ_UPVALUE: markValue(((ObjUpvalue*)object)->closed); break;
        case OBJ_FUNCTION: {
            ObjFunction* f = (ObjFunction*)object;
            markObject((struct Obj*)f->name);
            markArray(&f->chunk.constants);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* c = (ObjClosure*)object;
            markObject((struct Obj*)c->function);
            for (int i = 0; i < c->upvalueCount; i++) markObject((struct Obj*)c->upvalues[i]);
            break;
        }
        case OBJ_CONTINUATION: {
            markContinuation((ObjContinuation*)object);
            break;
        }
        default: break;
    }
}

static void sweep() {
    struct Obj* previous = NULL;
    struct Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked) {
            object->isMarked = false;
            previous = object;
            object = object->next;
        } else {
            struct Obj* unreached = object;
            object = object->next;
            if (previous != NULL) previous->next = object;
            else vm.objects = object;

            // Walia Persistent Reclaim: 0 size frees the object in the mmap heap
            reallocate(unreached, 0, 0); 
        }
    }
}

void collectGarbage() {
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) markValue(*slot);
    for (int i = 0; i < vm.frameCount; i++) markObject((struct Obj*)vm.frames[i].closure);
    for (ObjUpvalue* uv = vm.openUpvalues; uv != NULL; uv = uv->next) markObject((struct Obj*)uv);

    tableMark(&vm.globals);
    tableMark(&vm.strings);

    while (vm.grayCount > 0) blackenObject(vm.grayStack[--vm.grayCount]);
    tableRemoveWhite(&vm.strings);
    sweep();
}
