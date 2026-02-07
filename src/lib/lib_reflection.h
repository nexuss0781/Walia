#ifndef WALIA_LIB_REFLECTION_H
#define WALIA_LIB_REFLECTION_H

#include "core/common.h"
#include "core/value.h"

// ==========================================
// REFLECTION API
// ==========================================

/**
 * @brief Bootstraps the Reflection Library.
 * Registers getattr, setattr, hasattr, type.
 */
void initReflectionLibrary();

#endif // WALIA_LIB_REFLECTION_H
