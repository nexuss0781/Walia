#ifndef WALIA_WEB_TRANS_LEXER_H
#define WALIA_WEB_TRANS_LEXER_H

#include "common.h"
#include "web_trans_common.h"

// ==========================================
// SIMD LEXER API
// ==========================================

/**
 * @brief High-Speed structural scan of raw JSON text.
 * 
 * Logic:
 * Uses SIMD unrolling to identify structural delimiters ({}[]:,")
 * and produces a sequence of 64-bit jump-bitmasks.
 * 
 * @param source   The memory-mapped JSON source string.
 * @param length   Byte length of the source.
 * @param outMasks Buffer to store 64-bit masks (one per 64-byte block).
 */
void walia_json_scan_structural(const uint8_t* source, size_t length, uint64_t* outMasks);

#endif // WALIA_WEB_TRANS_LEXER_H