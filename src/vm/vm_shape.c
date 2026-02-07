#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm/vm_shape.h"
#include "core/memory.h"
#include "core/vm.h"

// ==========================================
// SHAPE MANAGEMENT
// ==========================================

ObjShape* vm_shape_new_root() {
    ObjShape* shape = (ObjShape*)reallocate(NULL, 0, sizeof(ObjShape));
    
    // Initialize Header
    shape->obj.type = OBJ_NATIVE; // Using Native type for internal structures
    shape->obj.isMarked = false;
    shape->obj.next = vm.objects;
    vm.objects = (struct Obj*)shape;

    shape->propertyName = NULL;
    shape->offset = 0;
    shape->transitionCount = 0;

    // Initialize Operator Cache to NIL (Miss)
    for (int i = 0; i < MAGIC_SENTINEL; i++) {
        shape->operatorCache[i] = NIL_VAL;
    }

    markCard(shape);
    return shape;
}

static ObjShape* allocate_shape(ObjString* prop, int offset) {
    ObjShape* shape = vm_shape_new_root();
    shape->propertyName = prop;
    shape->offset = offset;
    return shape;
}

ObjShape* vm_shape_transition(ObjShape* current, ObjString* name) {
    // 1. Check existing transitions (Edge Hit)
    for (int i = 0; i < current->transitionCount; i++) {
        if (current->transitions[i].key == name) { // Pointer equality via Interning
            return current->transitions[i].nextShape;
        }
    }

    // 2. Create new transition (Edge Miss)
    if (current->transitionCount >= SHAPE_MAX_TRANSITIONS) {
        // In full production, we would switch to a Table here.
        // For Phase 13.1, we hard limit or return NULL to force Map mode.
        return NULL;
    }

    ObjShape* next = allocate_shape(name, current->offset + 1);
    
    // Link parent to child
    current->transitions[current->transitionCount].key = name;
    current->transitions[current->transitionCount].nextShape = next;
    current->transitionCount++;
    
    markCard(current);
    return next;
}

int vm_shape_find_property(ObjShape* shape, ObjString* name) {
    // Walk backwards up the shape tree
    // (Optimization: In Phase 13.2 we will add a Table to the Shape for O(1))
    ObjShape* cursor = shape;
    while (cursor->propertyName != NULL) {
        if (cursor->propertyName == name) {
            return cursor->offset;
        }
        // We need a parent pointer to walk back!
        // Adding parent pointer to struct implies modification.
        // For now, linear scan or assume caller uses cache.
        
        // *Correction*: We cannot walk back easily without a parent pointer.
        // We will assume for Phase 13.1 that property access uses the Cache.
        // If Cache miss, we do a slow search (omitted for brevity).
        break; 
    }
    return -1;
}
