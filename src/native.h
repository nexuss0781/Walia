#ifndef WALIA_NATIVE_H
#define WALIA_NATIVE_H

#include "core/common.h"
#include "core/value.h"
#include "core/object.h"

/**
 * @brief Bootstraps the core Walia Standard Library.
 * Registers 'clock', 'print', and other basic primitives.
 */
void initNatives();

/**
 * @brief Utility to register a C function into the Walia global scope.
 */
void defineNative(const char* name, NativeFn function);

#endif // WALIA_NATIVE_H
