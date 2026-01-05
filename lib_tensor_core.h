#ifndef WALIA_LIB_TENSOR_CORE_H
#define WALIA_LIB_TENSOR_CORE_H

#include "common.h"
#include "object.h"

// ==========================================
// TENSOR ARCHITECTURAL LIMITS
// ==========================================
#define WLD_TENSOR_MAX_RANK 8
#define WLD_SIMD_ALIGNMENT  64 // 512-bit alignment for AVX-512

/**
 * @brief The Sovereign Tensor Object.
 * 
 * Architecture:
 * We decouple the "Metadata" (Shape/Strides) from the "Buffer" (Data).
 * This allows multiple ObjTensor objects to point to the same memory 
 * with different views (e.g. Transposed or Sliced).
 */
typedef struct {
    struct Obj obj;
    
    // Geometry
    uint32_t rank;
    uint32_t shape[WLD_TENSOR_MAX_RANK];
    int32_t  strides[WLD_TENSOR_MAX_RANK]; // Bytes to skip to move 1 unit in this dim
    
    // physical Storage
    float*   data;           // Pointer to the start of the buffer
    uint64_t totalElements;  // Logical count of elements
    uint32_t bufferOffset;   // Starting byte offset for this view
    
    // Resource Management
    bool     isOwner;        // True if this object should free 'data'
} ObjTensor;

// ==========================================
// HYPER-CORE API
// ==========================================

/**
 * @brief Allocates a new contiguous Tensor in the persistent heap.
 * Aligned to 64-byte boundaries for UFO-grade SIMD throughput.
 */
ObjTensor* walia_tensor_new(uint32_t rank, const uint32_t* shape);

/**
 * @brief Creates a new view of the data with swapped strides.
 * Zero-Copy Transpose: Complexity O(1).
 */
ObjTensor* walia_tensor_transpose(ObjTensor* source, uint32_t dim1, uint32_t dim2);

/**
 * @brief Changes the logical shape without moving data.
 * Zero-Copy Reshape: Complexity O(1).
 */
ObjTensor* walia_tensor_reshape(ObjTensor* source, uint32_t newRank, const uint32_t* newShape);

/**
 * @brief Calculates the physical memory address of an element.
 */
static inline float* walia_tensor_at(ObjTensor* t, const uint32_t* indices) {
    uint8_t* ptr = (uint8_t*)t->data + t->bufferOffset;
    for (uint32_t i = 0; i < t->rank; i++) {
        ptr += indices[i] * t->strides[i];
    }
    return (float*)ptr;
}

#endif // WALIA_LIB_TENSOR_CORE_H