#ifndef WALIA_SYS_ASSEMBLER_H
#define WALIA_SYS_ASSEMBLER_H

#include "core/common.h"
#include "core/object.h"

// ==========================================
// CODE BLOB SPECIFICATION
// ==========================================

/**
 * @brief A container for assembled machine code.
 * Stored in the persistent heap and marked as executable.
 */
// ObjCodeBlob is defined in core/object.h

// ==========================================
// ASSEMBLER API
// ==========================================

/**
 * @brief Translates an assembly string into executable binary.
 * 
 * Logic:
 * 1. Tokenizes mnemonics (MOV, PUSH, SYSCALL).
 * 2. Maps to architecture-specific opcodes (x86_64 vs ARM64).
 * 3. Allocates persistent memory and applies PROT_EXEC.
 * 
 * @param source The raw string from an asm {} block.
 * @return ObjCodeBlob* Pointer to the executable logic.
 */
ObjCodeBlob* sys_assemble(const char* source);

/**
 * @brief Low-level execution hook.
 * Directly jumps the CPU to the start of the code blob.
 */
void sys_asm_execute(ObjCodeBlob* blob);

#endif // WALIA_SYS_ASSEMBLER_H
