#include <string.h>
#include <stdio.h>

#include "sys/sys_types.h"

// ==========================================
// TYPE REGISTRY
// ==========================================

static const SysTypeInfo typeRegistry[] = {
    { SYS_TYPE_VALUE, "Value", 8, false, false },
    
    // Unsigned
    { SYS_TYPE_U8,    "u8",    1, false, false },
    { SYS_TYPE_U16,   "u16",   2, false, false },
    { SYS_TYPE_U32,   "u32",   4, false, false },
    { SYS_TYPE_U64,   "u64",   8, false, false },

    // Signed
    { SYS_TYPE_I8,    "i8",    1, true,  false },
    { SYS_TYPE_I16,   "i16",   2, true,  false },
    { SYS_TYPE_I32,   "i32",   4, true,  false },
    { SYS_TYPE_I64,   "i64",   8, true,  false },

    // Float
    { SYS_TYPE_F32,   "f32",   4, true,  true  },
    { SYS_TYPE_F64,   "f64",   8, true,  true  },

    // Special
    { SYS_TYPE_BOOL,  "bool",  1, false, false },
    { SYS_TYPE_VOID,  "void",  0, false, false },
    { SYS_TYPE_PTR,   "ptr",   8, false, false }
};

#define REGISTRY_SIZE (sizeof(typeRegistry) / sizeof(SysTypeInfo))

// ==========================================
// PUBLIC API IMPLEMENTATION
// ==========================================

const SysTypeInfo* sys_get_type_info(WaliaSysType type) {
    if (type >= REGISTRY_SIZE) return &typeRegistry[0]; // Default to Value
    return &typeRegistry[type];
}

WaliaSysType sys_resolve_type_name(const char* name, int length) {
    for (int i = 0; i < (int)REGISTRY_SIZE; i++) {
        const char* tName = typeRegistry[i].name;
        if (strlen(tName) == (size_t)length && memcmp(name, tName, length) == 0) {
            return typeRegistry[i].type;
        }
    }
    return SYS_TYPE_VALUE; // Not a system type (maybe a Class name)
}

bool sys_is_register_compatible(WaliaSysType type) {
    const SysTypeInfo* info = sys_get_type_info(type);
    return info->size <= 8;
}
