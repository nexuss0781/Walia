#include <string.h>
#include <stdio.h>

#include "../sys/sys_ffi_types.h"

// ==========================================
// INTERNAL TYPE MAPPING
// ==========================================

/**
 * @brief Internal lookup to map a character to its hardware metadata.
 */
static bool char_to_metadata(char c, WldTypeMetadata* out) {
    switch (c) {
        case 'v': // Void
            out->type = FFI_TYPE_VOID;
            out->regClass = CLASS_INTEGER;
            out->size = 0;
            out->alignment = 0;
            return true;
        case 'i': // Int 32
            out->type = FFI_TYPE_INT32;
            out->regClass = CLASS_INTEGER;
            out->size = 4;
            out->alignment = 4;
            return true;
        case 'I': // UInt 32
            out->type = FFI_TYPE_UINT32;
            out->regClass = CLASS_INTEGER;
            out->size = 4;
            out->alignment = 4;
            return true;
        case 'l': // Int 64 / Long
            out->type = FFI_TYPE_INT64;
            out->regClass = CLASS_INTEGER;
            out->size = 8;
            out->alignment = 8;
            return true;
        case 'L': // UInt 64 / ULong
            out->type = FFI_TYPE_UINT64;
            out->regClass = CLASS_INTEGER;
            out->size = 8;
            out->alignment = 8;
            return true;
        case 'f': // Float
            out->type = FFI_TYPE_FLOAT;
            out->regClass = CLASS_FLOAT;
            out->size = 4;
            out->alignment = 4;
            return true;
        case 'd': // Double
            out->type = FFI_TYPE_DOUBLE;
            out->regClass = CLASS_FLOAT;
            out->size = 8;
            out->alignment = 8;
            return true;
        case 'p': // Pointer / C-String
            out->type = FFI_TYPE_POINTER;
            out->regClass = CLASS_INTEGER;
            out->size = 8;
            out->alignment = 8;
            return true;
        default:
            return false; // Unknown type identifier
    }
}

// ==========================================
// PUBLIC SIGNATURE ENGINE
// ==========================================

bool ffi_parse_signature(const char* sigStr, WldFFISignature* outSig) {
    if (sigStr == NULL) return false;

    char* colon = strchr(sigStr, ':');
    if (!colon) return false; // Every FFI sig must define a return type

    // 1. Parse Arguments (Everything before the colon)
    int count = 0;
    const char* ptr = sigStr;
    while (ptr < colon) {
        if (count >= 16) return false; // Limit for stack safety
        if (!char_to_metadata(*ptr, &outSig->args[count])) {
            return false;
        }
        count++;
        ptr++;
    }
    outSig->argCount = count;

    // 2. Parse Return Type (Everything after the colon)
    if (!char_to_metadata(*(colon + 1), &outSig->returnType)) {
        return false;
    }

    return true;
}
