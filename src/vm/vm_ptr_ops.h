#ifndef WALIA_VM_PTR_OPS_H
#define WALIA_VM_PTR_OPS_H

#include "core/common.h"
#include "core/value.h"

// ==========================================
// POINTER ISA EXTENSIONS
// ==========================================
// Note: Opcodes OP_PTR_READ, OP_PTR_WRITE, OP_ADDRESS_OF are defined in opcode.h

// ==========================================
// MEMORY ACCESS API
// ==========================================

/**
 * @brief Reads a value from a raw memory address.
 * 
 * @param address The 64-bit virtual address (from a ptr type).
 * @return Value The data found at that address (NaN-boxed).
 */
Value ptr_read(uintptr_t address);

/**
 * @brief Writes a value to a raw memory address.
 * 
 * @param address The target 64-bit virtual address.
 * @param val The value to write.
 */
void ptr_write(uintptr_t address, Value val);

/**
 * @brief Calculates the physical address of a register.
 * Used for the '&' operator to get a stack pointer.
 */
uintptr_t ptr_get_stack_addr(Value* stackSlot);

#endif // WALIA_VM_PTR_OPS_H
