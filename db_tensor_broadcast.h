#ifndef WALIA_DB_TENSOR_BROADCAST_H
#define WALIA_DB_TENSOR_BROADCAST_H

#include "lib_tensor_core.h"

// ==========================================
// BROADCASTING SPECIFICATION
// ==========================================

/**
 * @brief Represents a Normalized View for dual-tensor operations.
 * 
 * When performing A + B, the engine creates two 'Broadcast Views' 
 * that share a common Shape and Rank, regardless of the inputs.
 */
typedef struct {
    uint32_t rank;
    uint32_t shape[WLD_TENSOR_MAX_RANK];
    int32_t  strides[WLD_TENSOR_MAX_RANK];
    float*   data;
} WldBroadcastView;

// ==========================================
// BROADCASTING API
// ==========================================

/**
 * @brief Checks if two tensors are broadcast-compatible and computes the output shape.
 * 
 * Logic (NumPy Standard):
 * 1. Align shapes to the right.
 * 2. For each dimension, sizes must match OR one must be 1.
 * 
 * @return bool True if compatible.
 */
bool db_tensor_can_broadcast(ObjTensor* a, ObjTensor* b, uint32_t* outRank, uint32_t* outShape);

/**
 * @brief Creates a Zero-Stride View of a tensor for a target shape.
 * 
 * Smart Logic:
 * If target_dim > source_dim and source_dim == 1, set stride to 0.
 * This effectively "clones" the data across the dimension without memory copies.
 */
void db_tensor_get_broadcast_view(ObjTensor* source, uint32_t targetRank, const uint32_t* targetShape, WldBroadcastView* outView);

#endif // WALIA_DB_TENSOR_BROADCAST_H