#ifndef WALIA_SYS_KERNEL_H
#define WALIA_SYS_KERNEL_H

#include "core/common.h"
#include "core/value.h"

// ==========================================
// SYSTEM CALL INTERFACE
// ==========================================

/**
 * @brief Executes a raw kernel system call.
 * 
 * Logic:
 * 1. Maps arguments to architecture-specific registers.
 * 2. Executes the TRAPE/SYSCALL instruction.
 * 3. Returns the kernel's response.
 * 
 * @param id The Syscall Number (e.g., SYS_write, SYS_open).
 * @param argCount Number of arguments (0-6).
 * @param args Pointer to the array of arguments.
 * @return Value The result returned by the kernel.
 */
Value sys_kernel_call(int id, int argCount, Value* args);

// ==========================================
// HARDWARE REGISTER API
// ==========================================

/**
 * @brief Reads the current value of a physical CPU register.
 * 
 * @param regName The name of the register (e.g., "rax", "x0").
 * @return Value The 64-bit content of the register.
 */
Value sys_read_register(const char* regName);

/**
 * @brief Writes a value directly into a physical CPU register.
 */
void sys_write_register(const char* regName, Value val);

/**
 * @brief Bootstraps the Kernel Party.
 */
void initKernelLibrary();

#endif // WALIA_SYS_KERNEL_H
