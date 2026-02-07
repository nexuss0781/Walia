#include <math.h>
#include <stdint.h>
#include "db_vector_math.h"
#include "lib/lib_vector.h"
#include "tooling/tooling_nexus_probes.h"

// Hardware Intrinsics Detection
#if defined(__x86_64__)
    #include <immintrin.h>
#elif defined(__aarch64__)
    #include <arm_neon.h>
#endif

// ==========================================
// SIMD DOT PRODUCT KERNELS
// ==========================================

float db_vector_dot_product(const float* a, const float* b, uint32_t count) {
    float sum = 0.0f;
    uint32_t i = 0;

    // 1. INTEL/AMD UFO-PATH: AVX-512 (16 Floats per cycle)
#ifdef __AVX512F__
    __m512 v_sum = _mm512_setzero_ps();
    for (; i <= count - 16; i += 16) {
        // Aligned loads (vmovaps) require 64-byte boundary
        __m512 va = _mm512_loadu_ps(&a[i]); // Use loadu for safety unless we guarantee alignment
        __m512 vb = _mm512_loadu_ps(&b[i]);
        // Fused Multiply-Add: v_sum = (va * vb) + v_sum
        v_sum = _mm512_fmadd_ps(va, vb, v_sum);
    }
    sum = _mm512_reduce_add_ps(v_sum);

    // 2. ARM/APPLE/MOBILE PATH: NEON (4 Floats per cycle)
#elif defined(__aarch64__)
    float32x4_t v_sum_vec = vdupq_n_f32(0.0f);
    for (; i <= count - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        v_sum_vec = vmlaq_f32(v_sum_vec, va, vb); // Fused Multiply-Add supported on ARMv8
    }
    sum = vaddvq_f32(v_sum_vec);

    // 3. INTEL/AMD LEGACY PATH: AVX2 (8 Floats per cycle)
#elif defined(__AVX2__)
    __m256 v_sum_avx2 = _mm256_setzero_ps();
    for (; i <= count - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        v_sum_avx2 = _mm256_fmadd_ps(va, vb, v_sum_avx2);
    }
    // Horizontal sum for AVX2
    __m128 v_low = _mm256_castps256_ps128(v_sum_avx2);
    __m128 v_high = _mm256_extractf128_ps(v_sum_avx2, 1);
    __m128 v_sum_128 = _mm_add_ps(v_low, v_high);
    v_sum_128 = _mm_hadd_ps(v_sum_128, v_sum_128);
    v_sum_128 = _mm_hadd_ps(v_sum_128, v_sum_128);
    sum = _mm_cvtss_f32(v_sum_128);
#endif

    // 4. SCALAR CLEANUP (Remaining dimensions)
    for (; i < count; i++) {
        sum += a[i] * b[i];
    }

    // Telemetry Pulse: Record neural math cycles for Command Nexus HUD
    PROBE_NEURAL_SIMD(count);

    return sum;
}

// ==========================================
// SIMD DISTANCE METRICS
// ==========================================

float db_vector_cosine_similarity(ObjVector* a, ObjVector* b) {
    if (a->dimensions != b->dimensions) return 0.0f;

    // UFO Speed: Magnitudes are pre-computed in Phase 4 
    // during vector allocation to keep similarity check O(dim/16).
    float dot = db_vector_dot_product(a->data, b->data, a->dimensions);
    float denom = a->magnitude * b->magnitude;

    return (denom < 1e-9f) ? 0.0f : (dot / denom);
}

float db_vector_euclidean_dist_sq(ObjVector* a, ObjVector* b) {
    uint32_t count = a->dimensions;
    const float* da = a->data;
    const float* db = b->data;
    float total_dist = 0.0f;
    uint32_t i = 0;

#ifdef __AVX512F__
    __m512 v_dist = _mm512_setzero_ps();
    for (; i <= count - 16; i += 16) {
        __m512 va = _mm512_loadu_ps(&da[i]);
        __m512 vb = _mm512_loadu_ps(&db[i]);
        __m512 diff = _mm512_sub_ps(va, vb);
        v_dist = _mm512_fmadd_ps(diff, diff, v_dist);
    }
    total_dist = _mm512_reduce_add_ps(v_dist);
#elif defined(__aarch64__)
    float32x4_t v_dist = vdupq_n_f32(0.0f);
    for (; i <= count - 4; i += 4) {
        float32x4_t va = vld1q_f32(&da[i]);
        float32x4_t vb = vld1q_f32(&db[i]);
        float32x4_t diff = vsubq_f32(va, vb);
        v_dist = vfmaq_f32(v_dist, diff, diff);
    }
    total_dist = vaddvq_f32(v_dist);
#elif defined(__AVX2__)
    __m256 v_dist = _mm256_setzero_ps();
    for (; i <= count - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&da[i]);
        __m256 vb = _mm256_loadu_ps(&db[i]);
        __m256 diff = _mm256_sub_ps(va, vb);
        v_dist = _mm256_fmadd_ps(diff, diff, v_dist);
    }
    float temp[8];
    _mm256_storeu_ps(temp, v_dist);
    for (int j = 0; j < 8; j++) total_dist += temp[j];
#endif

    // Scalar cleanup
    for (; i < count; i++) {
        float diff = da[i] - db[i];
        total_dist += diff * diff;
    }

    return total_dist;
}

// ==========================================
// BULK NORMALIZATION
// ==========================================

void db_vector_normalize_simd(float* data, uint32_t count) {
    float mag_sq = 0.0f;
    uint32_t i = 0;

    // 1. Calculate Magnitude (SIMD)
#ifdef __AVX512F__
    __m512 v_mag = _mm512_setzero_ps();
    for (; i <= count - 16; i += 16) {
        __m512 v = _mm512_loadu_ps(&data[i]);
        v_mag = _mm512_fmadd_ps(v, v, v_mag);
    }
    mag_sq = _mm512_reduce_add_ps(v_mag);
#elif defined(__aarch64__)
    float32x4_t v_mag = vdupq_n_f32(0.0f);
    for (; i <= count - 4; i += 4) {
        float32x4_t v = vld1q_f32(&data[i]);
        v_mag = vfmaq_f32(v_mag, v, v);
    }
    mag_sq = vaddvq_f32(v_mag);
#elif defined(__AVX2__)
    __m256 v_mag = _mm256_setzero_ps();
    for (; i <= count - 8; i += 8) {
        __m256 v = _mm256_loadu_ps(&data[i]);
        v_mag = _mm256_fmadd_ps(v, v, v_mag);
    }
    float temp[8];
    _mm256_storeu_ps(temp, v_mag);
    for (int j = 0; j < 8; j++) mag_sq += temp[j];
#endif
    
    // Scalar cleanup for magnitude
    for (; i < count; i++) mag_sq += data[i] * data[i];

    float inv_mag = 1.0f / sqrtf(mag_sq);
    if (isnan(inv_mag) || isinf(inv_mag)) return;

    // 2. Scale Vector (SIMD)
    i = 0;
#ifdef __AVX512F__
    __m512 v_inv = _mm512_set1_ps(inv_mag);
    for (; i <= count - 16; i += 16) {
        __m512 v = _mm512_loadu_ps(&data[i]);
        _mm512_storeu_ps(&data[i], _mm512_mul_ps(v, v_inv));
    }
#elif defined(__aarch64__)
    float32x4_t v_inv = vdupq_n_f32(inv_mag);
    for (; i <= count - 4; i += 4) {
        float32x4_t v = vld1q_f32(&data[i]);
        vst1q_f32(&data[i], vmulq_f32(v, v_inv));
    }
#elif defined(__AVX2__)
    __m256 v_inv_avx = _mm256_set1_ps(inv_mag);
    for (; i <= count - 8; i += 8) {
        __m256 v = _mm256_loadu_ps(&data[i]);
        _mm256_storeu_ps(&data[i], _mm256_mul_ps(v, v_inv_avx));
    }
#endif

    // Scalar cleanup for scaling
    for (; i < count; i++) data[i] *= inv_mag;
}
