#ifndef WALIA_SYS_FFI_JIT_H
#define WALIA_SYS_FFI_JIT_H

#include "core/common.h"
#include "core/object.h"
#include "sys/sys_ffi_types.h"

// ==========================================
// SOVEREIGN THUNK STRUCTURE
// ==========================================

/**
 * @brief An executable Walia object representing a C-function thunk.
 */
typedef struct {
    struct Obj obj;
    void*      cFuncPtr;     // Physical address of the C library symbol
    void*      thunkCode;    // JIT-compiled entry point
    size_t     thunkSize;    // Size of the machine code block
    WldFFISignature signature; 
} ObjSovereignThunk;

// ==========================================
// JIT COMPILER API
// ==========================================

/**
 * @brief JIT-compiles a machine code thunk for a specific C signature.
 * 
 * @param cFunc Actual address of the target C function.
 * @param sig   The hardware contract (from Phase 13.4.1).
 * @return ObjSovereignThunk* A persistent Walia object ready for execution.
 */
ObjSovereignThunk* ffi_jit_compile(void* cFunc, WldFFISignature sig);

/**
 * @brief Executes a JIT-compiled thunk.
 * 
 * Logic:
 * 1. Prepares the register window.
 * 2. Jumps to the thunk address.
 */
Value ffi_jit_execute(ObjSovereignThunk* thunk, int argCount, Value* args);

#endif // WALIA_SYS_FFI_JIT_H
