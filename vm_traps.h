#ifndef WALIA_VM_TRAPS_H
#define WALIA_VM_TRAPS_H

#include "common.h"
#include "value.h"
#include "object.h"

// ==========================================
// TRAP DISPATCHER API
// ==========================================

/**
 * @brief Intercepts a 'GET' operation if the receiver is a proxy.
 * 
 * @param receiver The object being accessed.
 * @param name     The interned property name.
 * @return Value   The result from the proxy trap or the target object.
 */
Value vm_trap_get_property(Value receiver, ObjString* name);

/**
 * @brief Intercepts a 'SET' operation if the receiver is a proxy.
 * 
 * @param receiver The object being modified.
 * @param name     The property name.
 * @param value    The new value to store.
 * @return bool    True if the trap was successfully executed.
 */
bool vm_trap_set_property(Value receiver, ObjString* name, Value value);

#endif // WALIA_VM_TRAPS_H