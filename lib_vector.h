#ifndef WALIA_LIB_VECTOR_H
#define WALIA_LIB_VECTOR_H

#include "common.h"
#include "object.h"

// ==========================================
// NEURAL VECTOR SPECIFICATION
// ==========================================

/**
 * @brief SIMD-Aligned Neural Vector.
 * 
 * Architecture:
 * Every vector is aligned to a 64-byte boundary to saturate AVX-512.
 * Magnitude is pre-calculated to enable UFO-speed Cosine Similarity.
 */
typedef struct {
    Obj      obj;
    uint32_t dimensions;
    float    magnitude;    // Euclidean Norm for O(1) similarity normalization
    
    // Flexible array member for physical neural weights.
    // Must be allocated with sys_get_optimal_alignment() (64 bytes).
    float    data[] __attribute__((aligned(64))); 
} ObjVector;

// ==========================================
// VECTOR SUBSTRATE API
// ==========================================

/**
 * @brief Allocates a persistent, SIMD-aligned vector in the sovereign heap.
 */
ObjVector* walia_vector_new(uint32_t dimensions);

/**
 * @brief Performs a hardware-direct copy of neural weights.
 */
void walia_vector_copy(ObjVector* dest, ObjVector* src);

/**
 * @brief Implements Gaussian Mutation for Genetic Evolution (Phase 17.5).
 */
void walia_vector_mutate(ObjVector* v, float rate);

/**
 * @brief Refreshes the magnitude metadata after a weight mutation.
 */
void walia_vector_update_metadata(ObjVector* v);

#endif // WALIA_LIB_VECTOR_H
