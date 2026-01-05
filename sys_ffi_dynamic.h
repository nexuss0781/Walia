#ifndef WALIA_SYS_FFI_DYNAMIC_H
#define WALIA_SYS_FFI_DYNAMIC_H

#include "common.h"
#include "object.h"

// ==========================================
// DYNAMIC LINKER API
// ==========================================

/**
 * @brief Loads a shared library (.so / .dll).
 */
Value ffi_load_library(int argCount, Value* args);

/**
 * @brief Binds a C symbol to a Walia function.
 * bind(lib, symbol_name, return_type, arg_types_list)
 */
Value ffi_bind_symbol(int argCount, Value* args);

/**
 * @brief Bootstraps the FFI module.
 */
void initFFILibrary();

#endif // WALIA_SYS_FFI_DYNAMIC_H