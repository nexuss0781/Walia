#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __x86_64__
    #include <immintrin.h> // AVX-512
#elif defined(__aarch64__)
    #include <arm_neon.h>  // NEON
#endif

#include "web/web_trans_lexer.h"
#include "web/web_trans_common.h"
#include "tooling/tooling_nexus_probes.h"

// ==========================================
// AVX-512 HARDWARE KERNEL
// ==========================================

/**
 * @brief High-Speed structural character discovery.
 * 
 * UFO-GRADE LOGIC:
 * Loads 64 bytes of JSON. Compares the whole block against 
 * all 8 structural delimiters simultaneously.
 */
#ifdef __AVX512F__
__attribute__((target("avx512f,avx512bw")))
static uint64_t scan_64_bytes_avx512(const uint8_t* data) {
    // 1. Load 64 bytes into ZMM register
    __m512i v_data = _mm512_loadu_si512((__m512i*)data);

    // 2. Parallel Comparison
    // We create a bitmask identifying all 'special' characters
    __mmask64 m_br_open  = _mm512_cmpeq_epu8_mask(v_data, _mm512_set1_epi8('{'));
    __mmask64 m_br_close = _mm512_cmpeq_epu8_mask(v_data, _mm512_set1_epi8('}'));
    __mmask64 m_col      = _mm512_cmpeq_epu8_mask(v_data, _mm512_set1_epi8(':'));
    __mmask64 m_comma    = _mm512_cmpeq_epu8_mask(v_data, _mm512_set1_epi8(','));
    __mmask64 m_quote    = _mm512_cmpeq_epu8_mask(v_data, _mm512_set1_epi8('"'));

    // 3. Unify into a single structural mask
    return m_br_open | m_br_close | m_col | m_comma | m_quote;
}
#endif

// ==========================================
// SCALAR FALLBACK (ARM NEON / GENERIC)
// ==========================================

static uint64_t scan_64_bytes_scalar(const uint8_t* data) {
    uint64_t mask = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t c = data[i];
        if (c == '{' || c == '}' || c == ':' || c == ',' || c == '"') {
            mask |= (1ULL << i);
        }
    }
    return mask;
}

// ==========================================
// MASTER TRANCODER SCANNER
// ==========================================

/**
 * @brief Performs a hardware-limit pass over raw JSON.
 * 
 * @param source Pointer to the memory-mapped JSON text.
 * @param length Length of the buffer.
 * @param outMasks Array to store the generated 64-bit jump masks.
 */
void walia_json_scan_structural(const uint8_t* source, size_t length, uint64_t* outMasks) {
    size_t i = 0;
    int maskIdx = 0;

    // UFO SPEED: Advance in 64-byte strides
    for (; i <= length - 64; i += 64) {
#ifdef __AVX512F__
        outMasks[maskIdx++] = scan_64_bytes_avx512(&source[i]);
#else
        outMasks[maskIdx++] = scan_64_bytes_scalar(&source[i]);
#endif
        
        // Telemetry: Record 64-byte ingest cycle
        PROBE_NEURAL_SIMD(64); 
    }

    // Handle remaining bytes
    if (i < length) {
        uint8_t tail[64] = {0};
        memcpy(tail, &source[i], length - i);
        outMasks[maskIdx++] = scan_64_bytes_scalar(tail);
    }

    printf(">> Walia Transcoder: Structural scan complete. Generated %d jump-masks.\n", maskIdx);
}
