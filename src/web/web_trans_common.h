#ifndef WALIA_WEB_TRANS_COMMON_H
#define WALIA_WEB_TRANS_COMMON_H

#include "core/common.h"

// ==========================================
// TRANSCODER SPECIFICATIONS
// ==========================================

#define WLD_JSON_MAX_DEPTH 128
#define WLD_BIN_MAGIC      0x5742494E // "WBIN"

/**
 * @brief Structural Token Types.
 * Represented as bit-flags for SIMD register comparison.
 */
typedef enum {
    JSON_BRACE_OPEN   = '{',
    JSON_BRACE_CLOSE  = '}',
    JSON_BRACKET_OPEN = '[',
    JSON_BRACKET_CLOSE= ']',
    JSON_COLON        = ':',
    JSON_COMMA        = ',',
    JSON_QUOTE        = '"',
    JSON_BACKSLASH    = '\\'
} WldJsonDelimiter;

/**
 * @brief Represents a block of structural metadata.
 * A single 64-bit mask identifies the locations of delimiters 
 * within a 64-byte segment of raw text.
 */
typedef struct {
    uint64_t structural_mask; // Bit N set if Byte N is a delimiter
    uint64_t quote_mask;      // Identifies bytes inside strings
    uint64_t whitespace_mask; // Identifies ignorable bytes
} WldStructuralResult;

#endif // WALIA_WEB_TRANS_COMMON_H
