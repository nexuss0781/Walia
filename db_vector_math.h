
#ifndef WALIA_DB_VECTOR_MATH_H
#define WALIA_DB_VECTOR_MATH_H

#include "common.h"
#include "lib_vector.h"

// ==========================================
// SIMD HARDWARE SPECIFICATIONS
// ==========================================

#define WLD_SIMD_WIDTH_BYTES 64  // 512-bit (AVX-512)
#define WLD_ALIGN_SIMD __attribute__((aligned(64)))

// ==========================================
// NEURAL CALCULATION API
// ==========================================

/**
 * @brief Computes the Dot Product of two high-dimensional buffers.
 * UFO SPEED: Processes 16 floats (512 bits) per clock cycle.
 */
float db_vector_dot_product(const float* a, const float* b, uint32_t count);

/**
 * @brief Computes Cosine Similarity (-1.0 to 1.0).
 * Utilizes pre-computed magnitudes from the ObjVector header.
 */
float db_vector_cosine_similarity(struct ObjVector* a, struct ObjVector* b);

/**
 * @brief Computes the squared Euclidean Distance.
 * Optimized for early-exit during HNSW graph traversal.
 */
float db_vector_euclidean_dist_sq(struct ObjVector* a, struct ObjVector* b);

/**
 * @brief Performs in-place SIMD Normalization.
 */
void db_vector_normalize_simd(float* data, uint32_t count);

#endif // WALIA_DB_VECTOR_MATH_H
