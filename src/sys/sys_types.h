#ifndef WALIA_SYS_TYPES_H
#define WALIA_SYS_TYPES_H

#include "../core/common.h"

// ==========================================
// SYSTEM TYPE DEFINITIONS
// ==========================================

typedef enum {
    // Standard Managed Type (NaN-Boxed)
    SYS_TYPE_VALUE = 0,

    // Integer Types (Unsigned)
    SYS_TYPE_U8,
    SYS_TYPE_U16,
    SYS_TYPE_U32,
    SYS_TYPE_U64,

    // Integer Types (Signed)
    SYS_TYPE_I8,
    SYS_TYPE_I16,
    SYS_TYPE_I32,
    SYS_TYPE_I64,

    // Floating Point
    SYS_TYPE_F32,
    SYS_TYPE_F64,

    // System Primitives
    SYS_TYPE_BOOL, // 1-byte boolean
    SYS_TYPE_VOID, // No return value
    SYS_TYPE_PTR   // Raw Pointer (void*)
} WaliaSysType;

// ==========================================
// TYPE METADATA
// ==========================================

typedef struct {
    WaliaSysType type;
    const char*  name;
    uint8_t      size;      // Size in bytes (1, 2, 4, 8)
    bool         isSigned;
    bool         isFloat;
} SysTypeInfo;

// ==========================================
// TYPE SYSTEM API
// ==========================================

/**
 * @brief Retrieves metadata for a system type.
 */
const SysTypeInfo* sys_get_type_info(WaliaSysType type);

/**
 * @brief Resolves a string token to a System Type.
 * E.g., "u8" -> SYS_TYPE_U8.
 */
WaliaSysType sys_resolve_type_name(const char* name, int length);

/**
 * @brief Checks if a type fits inside a standard VM register without conversion.
 * (i.e., is it <= 8 bytes?)
 */
bool sys_is_register_compatible(WaliaSysType type);

#endif // WALIA_SYS_TYPES_H
