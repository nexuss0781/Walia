#ifndef WALIA_DB_TENSOR_MATH_H
#define WALIA_DB_TENSOR_MATH_H

#include "lib_tensor_core.h"

// ================= =========================
// TENSOR KERNEL SPECIFICATIONS
// ==========================================

/**
 * @brief Performs element-wise addition with broadcasting.
 * Logic: res = a + b
 */
void db_tensor_add_fused(struct ObjTensor* a, struct ObjTensor* b, struct ObjTensor* res);

/**
 * @brief Performs element-wise multiplication with broadcasting.
 */
void db_tensor_mul_fused(struct ObjTensor* a, struct ObjTensor* b, struct ObjTensor* res);

/**
 * @brief High-performance Matrix Multiplication (GEMM).
 * Utilizes L3 cache tiling to prevent memory-wall bottlenecks.
 */
void db_tensor_matmul(struct ObjTensor* a, struct ObjTensor* b, struct ObjTensor* res);

/**
 * @brief Calculates the resulting shape and strides for a broadcast operation.
 * Fulfills the 'NumPy-Killer' requirement for automated dimension expansion.
 */
bool db_tensor_reconcile_broadcast(struct ObjTensor* a, struct ObjTensor* b, 
                                  uint32_t* outRank, uint32_t* outShape);

#endif // WALIA_DB_TENSOR_MATH_H
