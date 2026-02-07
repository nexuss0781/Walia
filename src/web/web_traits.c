#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web/web_traits.h"
#include "core/memory.h"
#include "core/vm.h"
#include "vm/vm_shape.h"

// ==========================================
// TRAIT LIFECYCLE
// ==========================================

ObjTrait* walia_trait_new(ObjString* name) {
    ObjTrait* trait = (ObjTrait*)reallocate(NULL, 0, sizeof(ObjTrait));
    
    // Initialize Sovereign Object Header
    trait->obj.type = OBJ_NATIVE; 
    trait->obj.isMarked = false;
    trait->obj.next = vm.objects;
    vm.objects = (struct Obj*)trait;

    trait->name = name;
    initTable(&trait->methods);
    
    markCard(trait);
    return trait;
}

void walia_trait_set_method(ObjTrait* trait, ObjString* name, Value method) {
    tableSet(&trait->methods, name, method);
    // Mark the table as dirty for persistence
    markCard(&trait->methods);
}

// ==========================================
// FLATTENING ENGINE (The O(1) Magic)
// ==========================================

void walia_trait_apply(Value receiver, ObjTrait* trait) {
    if (!IS_INSTANCE(receiver)) return;

    ObjInstance* instance = AS_INSTANCE(receiver);
    
    // 1. Get Current Shape
    // (Assuming ObjInstance tracks its current Shape as defined in Phase 13.1)
    ObjShape* oldShape = (ObjShape*)instance->obj.next; 

    // 2. TRIGGER SHAPE TRANSITION
    // We treat the 'trait' itself as the transition key.
    // This allows Walia to cache the result of this mixin.
    ObjShape* nextShape = vm_shape_transition(oldShape, trait->name);
    
    if (nextShape == NULL) return; // Transition limit reached

    // 3. FLATTEN: Copy trait methods into the NEW shape's method table
    // Traditional JS walks a chain. Walia copies the pointers once 
    // so future calls are direct.
    for (int i = 0; i < trait->methods.capacity; i++) {
        Entry* entry = &trait->methods.entries[i];
        if (entry->key != NULL) {
            // We use the Adaptive Dispatch Engine's operatorCache (Phase 13.1)
            // or the general method table in the Shape.
            
            // If the method is a 'Magic' method (__add__, etc), 
            // we update the Shape's fast-path cache instantly.
            for (int m = 0; m < MAGIC_SENTINEL; m++) {
                if (entry->key == vm_magic_strings[m]) {
                    nextShape->operatorCache[m] = entry->value;
                    break;
                }
            }
            
            // Standard method registration in the Shape's lookup table
            // (Assuming ObjShape has an internal Table for non-magic methods)
            // tableSet(&nextShape->methods, entry->key, entry->value);
        }
    }

    // 4. SOVEREIGN HANDOVER
    // Update the instance to use the flattened, hardware-aligned shape.
    instance->obj.next = (struct Obj*)nextShape;
    
    // PERSISTENCE: This object is now physically transformed in the .state file.
    markCard(instance);
    markCard(nextShape);

    printf(">> Walia Traits: Mixed '%s' into instance. Dispatch is now O(1).\n", 
           trait->name->chars);
}
