#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h> // Fixed: Added for sqrtf definition

#include "db_quantize.h"
#include "memory.h"
#include "object.h"

// ==========================================
// SCALAR QUANTIZATION (SQ8)
// ==========================================

/**
 * @brief Compresses a high-precision vector (F32) to 8-bit precision (SQ8).
 * 
 * Logic:
 * 1. Analyzes the value range (Min/Max) of the input neurons.
 * 2. Maps the floating-point values into the 0-255 integer space.
 * 3. Allocates a persistent object in the .wld heap to store the result.
 */
ObjQuantizedVector* db_vector_quantize_sq8(ObjVector* vec) {
    float min = FLT_MAX;
    float max = -FLT_MAX;
    float* raw = db_vector_raw(vec);

    // 1. Range Analysis
    for (uint32_t i = 0; i < vec->dimensions; i++) {
        if (raw[i] < min) min = raw[i];
        if (raw[i] > max) max = raw[i];
    }

    // 2. Allocation in the Sovereign Persistent Heap
    size_t dataSize = vec->dimensions;
    ObjQuantizedVector* qvec = (ObjQuantizedVector*)reallocate(NULL, 0, 
                                sizeof(ObjQuantizedVector) + dataSize);
    
    // Initialize Object Metadata
    qvec->obj.type = OBJ_CLOSURE; // Mapped as complex type
    qvec->obj.isMarked = false;
    qvec->obj.next = NULL; // Managed by GC trace
    
    qvec->originalDims = vec->dimensions;
    qvec->min = min;
    qvec->max = max;

    // 3. Scaling Transformation
    float range = max - min;
    if (range == 0.0f) range = 1.0f; // Prevent division by zero for uniform vectors

    for (uint32_t i = 0; i < vec->dimensions; i++) {
        // Map [min, max] -> [0, 255]
        qvec->payload[i] = (uint8_t)(((raw[i] - min) / range) * 255.0f);
    }

    // 4. Persistence Guarantee
    markCard(qvec);
    
    return qvec;
}

// ==========================================
// ASYMMETRIC DISTANCE COMPUTATION (ADC)
// ==========================================

/**
 * @brief Similarity calculation for Trillion-Scale search.
 * 
 * UFO Speed Path: We de-quantize the target values inside the CPU 
 * register during the math loop, avoiding any RAM-to-RAM copies.
 */
float db_vector_distance_adc(ObjVector* query, ObjQuantizedVector* target) {
    float sum = 0.0f;
    float* q_data = db_vector_raw(query);
    
    float min = target->min;
    float range = target->max - target->min;
    float step = range / 255.0f;

    for (uint32_t i = 0; i < query->dimensions; i++) {
        // Linear reconstruction: 8-bit int -> 32-bit float
        float target_val = min + (target->payload[i] * step);
        
        float diff = q_data[i] - target_val;
        sum += diff * diff;
    }

    // sqrtf is now properly declared via <math.h>
    return sqrtf(sum);
}

// ==========================================
// PRODUCT QUANTIZATION (PQ) PREP
// ==========================================

WldCodebook* db_codebook_new(uint32_t subVectorDim) {
    WldCodebook* cb = (WldCodebook*)reallocate(NULL, 0, sizeof(WldCodebook));
    
    cb->obj.type = OBJ_CLOSURE;
    cb->obj.isMarked = false;
    cb->subVectorDim = subVectorDim;
    
    // Clear centroids for fresh training
    memset(cb->centroids, 0, sizeof(cb->centroids));
    
    markCard(cb);
    return cb;
}
