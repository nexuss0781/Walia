#ifndef WALIA_VM_NATIVE_COMPILER_H
#define WALIA_VM_NATIVE_COMPILER_H

#include "../core/object.h"
#include "../sys/sys_assembler.h"

// ==========================================
// TIER-2 JIT API
// ==========================================

/**
 * @brief Translates Walia Bytecode into Physical Machine Code.
 * 
 * UFO-GRADE OPTIMIZATION:
 * 1. Performs Register Pinning (R0-R15 -> Physical GPRs).
 * 2. Inlines hot sub-routines.
 * 3. Emits raw binary thunk into Executable Heap.
 * 
 * @param func The bytecode function to escalate.
 * @return ObjCodeBlob* The resulting machine-code thunk.
 */
ObjCodeBlob* walia_jit_tier2_compile(ObjFunction* func);

/**
 * @brief Executes a Tier-2 Native Thunk.
 * Switches the CPU from the VM loop to raw native execution.
 */
Value walia_jit_tier2_execute(ObjCodeBlob* thunk, int argCount, Value* args);

#endif // WALIA_VM_NATIVE_COMPILER_H
