#ifndef WALIA_VM_SHAPE_H
#define WALIA_VM_SHAPE_H

#include "core/common.h"
#include "core/value.h"
#include "core/object.h"
#include "core/table.h"
#include "vm/vm_magic_names.h"

// ==========================================
// HIDDEN CLASS (SHAPE) STRUCTURE
// ==========================================

#define SHAPE_MAX_TRANSITIONS 8

/**
 * @brief Represents the memory layout of an object.
 * Objects with the same properties in the same order share a Shape.
 */
typedef struct ObjShape {
    struct Obj obj;
    
    // The property introduced by this transition (e.g., "x")
    // NULL if this is the Root Shape.
    ObjString* propertyName;
    
    // The index in the object's field array where this property lives.
    int offset;
    
    // Transition Tree: Maps a property name to the next Shape.
    // Small array for speed; upgrades to Hash Map if too many transitions.
    struct {
        ObjString* key;
        struct ObjShape* nextShape;
    } transitions[SHAPE_MAX_TRANSITIONS];
    int transitionCount;

    // Operator Cache: Direct pointers to magic methods for this shape.
    // If NIL, we must lookup. If set, we jump.
    Value operatorCache[MAGIC_SENTINEL];
} ObjShape;

// ==========================================
// SHAPE API
// ==========================================

/**
 * @brief Creates the empty Root Shape.
 */
ObjShape* vm_shape_new_root();

/**
 * @brief Transitions from current shape to next shape based on property addition.
 * 
 * Logic:
 * 1. Check 'transitions' array for 'name'.
 * 2. If found, return that shape.
 * 3. If not, create new Shape, link it, and return.
 */
ObjShape* vm_shape_transition(ObjShape* current, ObjString* name);

/**
 * @brief Finds the offset of a property within a Shape.
 * Walks up the parent chain (or uses an internal index in production).
 * Returns -1 if not found.
 */
int vm_shape_find_property(ObjShape* shape, ObjString* name);

#endif // WALIA_VM_SHAPE_H
