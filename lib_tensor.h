#ifndef WALIA_LIB_TENSOR_H
#define WALIA_LIB_TENSOR_H

#include "common.h"
#include "object.h"

// ==========================================
// TENSOR ARCHITECTURAL LIMITS
// ==========================================
#define WLD_TENSOR_MAX_RANK 8
#define WLD_SIMD_ALIGNMENT  64 

/**
 * @brief The Sovereign Tensor Object.
 */
typedef struct {
    struct Obj obj;
    
    // Geometry (Hardware Aligned)
    uint32_t rank;
    uint32_t shape[WLD_TENSOR_MAX_RANK];
    int32_t  strides[WLD_TENSOR_MAX_RANK]; 
    uint64_t totalElements;
    
    // Memory View
    float*   data;          
    uint32_t bufferOffset;   
    bool     isOwner;       
} ObjTensor;

// ==========================================
// TENSOR HYPER-CORE API
// ==========================================

/**
 * @brief Allocates a new contiguous Tensor.
 */
ObjTensor* walia_tensor_new(uint32_t rank, const uint32_t* shape);

/**
 * @brief Creates a Zero-Copy View of an existing tensor.
 */
ObjTensor* walia_tensor_view(ObjTensor* source);

/**
 * @brief Zero-Copy Transpose: Complexity O(1).
 */
ObjTensor* walia_tensor_transpose(ObjTensor* source, uint32_t dim1, uint32_t dim2);

/**
 * @brief Zero-Copy Reshape: Complexity O(1).
 */
ObjTensor* walia_tensor_reshape(ObjTensor* source, uint32_t newRank, const uint32_t* newShape);

/**
 * @brief Initializes the Tensor Library.
 */
void initTensorLibrary();

#endif // WALIA_LIB_TENSOR_H