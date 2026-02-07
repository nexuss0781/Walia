#ifndef WALIA_SYS_FFI_TYPES_H
#define WALIA_SYS_FFI_TYPES_H

#include "core/common.h"

// ==========================================
// FFI TYPE SPECIFICATIONS
// ==========================================

typedef enum {
    FFI_TYPE_VOID,
    FFI_TYPE_INT32,
    FFI_TYPE_UINT32,
    FFI_TYPE_INT64,
    FFI_TYPE_UINT64,
    FFI_TYPE_FLOAT,
    FFI_TYPE_DOUBLE,
    FFI_TYPE_POINTER
} WldFFIType;

typedef enum {
    CLASS_INTEGER, // Maps to RAX, RDI, RSI, etc.
    CLASS_FLOAT    // Maps to XMM0, XMM1, etc.
} WldTypeClass;

/**
 * @brief Hardware-aligned metadata for a specific ABI type.
 */
typedef struct {
    WldFFIType   type;
    WldTypeClass regClass;
    size_t       size;
    size_t       alignment;
} WldTypeMetadata;

/**
 * @brief Container for a full function signature.
 */
typedef struct {
    WldTypeMetadata returnType;
    WldTypeMetadata args[16]; // Max 16 arguments per FFI call
    int             argCount;
} WldFFISignature;

// ================= =========================
// SIGNATURE PARSER API
// ==========================================

/**
 * @brief Translates a signature string into a Hardware Contract.
 * 
 * Syntax: "args:return" 
 * Example: "ifp:v" -> (int, float, pointer) returns void
 * 
 * @param sigStr The raw signature string.
 * @param outSig Pointer to the resulting signature structure.
 * @return bool True if the signature is valid for the current ABI.
 */
bool ffi_parse_signature(const char* sigStr, WldFFISignature* outSig);

#endif // WALIA_SYS_FFI_TYPES_H
