#ifndef WALIA_LIB_VECTOR_H
#define WALIA_LIB_VECTOR_H

#include "core/common.h"
#include "core/object.h"

// ==========================================
// NEURAL VECTOR SPECIFICATION
// ==========================================

// Redefinition prevented: ObjVector is defined in core/object.h

// ==========================================
// VECTOR SUBSTRATE API
// ==========================================

void initVectorLibrary();

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
