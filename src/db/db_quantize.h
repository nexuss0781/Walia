#ifndef WALIA_DB_QUANTIZE_H
#define WALIA_DB_QUANTIZE_H

#include "db_vector.h"

// ==========================================
// QUANTIZATION SPECIFICATIONS
// ==========================================

#define WLD_PQ_SUBVECTORS 64   // Split 1536 dimensions into 64 sub-spaces
#define WLD_CODEBOOK_SIZE 256  // 8-bit index per sub-space

/**
 * @brief Persistent Codebook for Product Quantization.
 * Stores the centroids (clusters) for each sub-vector space.
 */
typedef struct {
    struct Obj obj;
    uint32_t   subVectorDim;
    float      centroids[WLD_PQ_SUBVECTORS][WLD_CODEBOOK_SIZE]; 
} WldCodebook;

/**
 * @brief The Compressed Neural Object.
 * Replaces ObjVector for multi-trillion scale storage.
 */
typedef struct {
    struct Obj obj;
    uint32_t   originalDims;
    float      min;            // For SQ8 scaling
    float      max;            // For SQ8 scaling
    uint8_t    payload[];      // Compressed byte stream
} ObjQuantizedVector;

// ==========================================
// QUANTIZATION API
// ==========================================

/**
 * @brief Compresses a high-precision vector using SQ8.
 * Reduces 32-bit floats to 8-bit integers via linear scaling.
 */
ObjQuantizedVector* db_vector_quantize_sq8(ObjVector* vec);

/**
 * @brief Calculates distance between a high-precision Query 
 * and a Quantized Vector without full decompression (UFO Speed).
 */
float db_vector_distance_adc(ObjVector* query, ObjQuantizedVector* target);

/**
 * @brief Generates a persistent codebook based on a sample set.
 * Required for Product Quantization (PQ).
 */
WldCodebook* db_codebook_new(uint32_t subVectorDim);

#endif // WALIA_DB_QUANTIZE_H
