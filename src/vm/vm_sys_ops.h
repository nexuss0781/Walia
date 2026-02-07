#ifndef WALIA_VM_SYS_OPS_H
#define WALIA_VM_SYS_OPS_H

#include "core/common.h"
#include "core/vm.h"
#include "core/value.h"
#include "core/opcode.h"    // Fixed: Pulls master ISA definitions
#include "sys/sys_types.h"

// ==========================================
// RAW MEMORY API
// ==========================================

uint8_t sys_vm_read_u8(Value* slot, uint8_t offset);
void sys_vm_write_u8(Value* slot, uint8_t offset, uint8_t data);

/**
 * @brief Performs a hardware cast between system types.
 */
Value sys_vm_cast(Value input, WaliaSysType targetType);

/**
 * @brief Core bitwise logic for systems programming.
 * Updated: Uses master OpCode enum for the operation selector.
 */
Value sys_vm_bit_op(Value a, Value b, OpCode op);

#endif // WALIA_VM_SYS_OPS_H
