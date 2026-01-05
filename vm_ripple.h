#ifndef WALIA_VM_RIPPLE_H
#define WALIA_VM_RIPPLE_H

#include "common.h"
#include "vm_entangle.h"

// ==========================================
// RIPPLE ENGINE API
// ==========================================

/**
 * @brief Triggers the recalculation of all entangled sinks for a register.
 * 
 * Logic:
 * 1. Checks the Dependency Mask.
 * 2. Identifies all affected ObjEntanglement nodes.
 * 3. Injects Ghost Frames into the VM to update values.
 * 
 * @param sourceReg The register that was just modified.
 */
void walia_ripple_trigger(int sourceReg);

/**
 * @brief Executes a single entanglement thunk.
 * 
 * UFO SPEED: Uses Stack Splicing to perform the update 
 * without creating a new managed closure object.
 */
void walia_ripple_execute_thunk(ObjEntanglement* ent);

#endif // WALIA_VM_RIPPLE_H