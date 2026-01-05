#ifndef WALIA_DB_VECTOR_H
#define WALIA_DB_VECTOR_H

#include "common.h"
#include "object.h"
#include "value.h"

// ==========================================
// NEURAL SPECIFICATIONS
// ==========================================

/**
 * @brief Precision levels for Trillion-Scale scaling.
 */
typedef enum {
    WLD_VECTOR_F32,   // High precision (4 bytes per dim)
    WLD_VECTOR_F16,   // Half precision (2 bytes per dim)
    WLD_VECTOR_SQ8,   // Scalar Quantized (1 byte per dim)
    WLD_VECTOR_BIT    // Binary (1 bit per dim) - For extreme scale
} WldVectorFormat;

/**
 * @brief Distance Metrics for Similarity Search.
 */
typedef enum {
    METRIC_COSINE,
    METRIC_EUCLIDEAN,
    METRIC_INNER_PRODUCT,
    METRIC_HAMMING
} WldDistanceMetric;

/**
 * @brief The Sovereign Vector Object.
 * Designed for AVX-512 / NEON alignment.
 */
typedef struct {
    struct Obj obj;       // Header (Inheritance)
    uint32_t   dimensions; 
    uint16_t   format;     // WldVectorFormat
    uint16_t   unused;     // Padding for 8-byte alignment
    
    // The data segment starts here.
    // It is guaranteed to be 64-byte aligned within the persistent heap.
    float      data[];     
} ObjVector;

// ==========================================
// VECTOR CORE API
// ==========================================

/**
 * @brief Allocates a new high-dimensional vector on the sovereign heap.
 * 
 * @param dims Number of neurons/dimensions.
 * @param format The precision format (F32/SQ8).
 * @return ObjVector* A pointer to the aligned vector object.
 */
ObjVector* db_vector_new(uint32_t dims, WldVectorFormat format);

/**
 * @brief Sets a specific neuron value.
 */
void db_vector_set(ObjVector* vec, uint32_t index, float value);

/**
 * @brief Returns a direct pointer to the math-ready data segment.
 * This is the UFO-Speed path for SIMD intrinsics.
 */
static inline float* db_vector_raw(ObjVector* vec) {
    return vec->data;
}

#endif // WALIA_DB_VECTOR_H
