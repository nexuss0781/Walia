#ifndef WALIA_VM_OPERATORS_H
#define WALIA_VM_OPERATORS_H

#include "../core/common.h"
#include "../core/value.h"
#include "../vm/vm_magic_names.h"
#include "../core/vm.h" // For InterpretResult

// ==========================================
// OPERATOR DISPATCH API
// ==========================================

/**
 * @brief Attempts to invoke a user-defined overload for a binary operator.
 * 
 * Logic:
 * 1. Checks if the receiver (a) is an Instance/Object.
 * 2. Looks up the 'method' in the class method table.
 * 3. If found, pushes arguments and calls the method.
 * 
 * @param a The receiver (Left Hand Side).
 * @param b The argument (Right Hand Side).
 * @param methodId The index of the magic method (MAGIC_ADD, etc.).
 * @return bool True if overload was found and called.
 */
bool vm_try_binary_overload(Value a, Value b, MagicMethod methodId);

/**
 * @brief Attempts to invoke a user-defined overload for a unary operator.
 */
bool vm_try_unary_overload(Value a, MagicMethod methodId);

/**
 * @brief Attempts to invoke a user-defined overload for indexing (get/set).
 */
bool vm_try_index_get(Value receiver, Value index);
bool vm_try_index_set(Value receiver, Value index, Value value);

#endif // WALIA_VM_OPERATORS_H
