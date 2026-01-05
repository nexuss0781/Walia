#ifndef WALIA_WEB_TRAITS_H
#define WALIA_WEB_TRAITS_H

#include "common.h"
#include "object.h"
#include "table.h"
#include "vm_shape.h"

// ==========================================
// TRAIT SPECIFICATION
// ==========================================

/**
 * @brief A Trait is a persistent bundle of behavior.
 * 
 * Unlike a Class, a Trait is designed to be "Mixed In" to existing 
 * objects at runtime to extend their functionality.
 */
typedef struct {
    struct Obj obj;
    ObjString* name;
    Table      methods; // Persistent table of ObjClosure/ObjNative
} ObjTrait;

// ==========================================
// TRAIT ENGINE API
// ==========================================

/**
 * @brief Allocates a new Trait in the persistent heap.
 */
ObjTrait* walia_trait_new(ObjString* name);

/**
 * @brief Adds a method implementation to a trait.
 */
void walia_trait_set_method(ObjTrait* trait, ObjString* name, Value method);

/**
 * @brief UFO-GRADE MIXIN: Applies a trait to a live object.
 * 
 * Logic:
 * 1. Identifies the object's current ObjShape.
 * 2. Generates a new "Transitioned Shape" representing the union 
 *     of the object and the trait.
 * 3. FLATTENING: Copies all trait methods into the new Shape's method table.
 * 4. Updates the instance to point to the new O(1) shape.
 * 
 * @param receiver The Walia object to extend.
 * @param trait    The trait containing the new behavior.
 */
void walia_trait_apply(Value receiver, ObjTrait* trait);

#endif // WALIA_WEB_TRAITS_H