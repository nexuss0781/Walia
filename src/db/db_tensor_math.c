#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "db_tensor_math.h"
#include "../lib/lib_tensor_core.h"
#include "db_vector_math.h"
#include "../core/memory.h"
#include "../tooling/tooling_nexus_probes.h"

// Hardware Intrinsics
#if defined(__x86_64__)
    #include <immintrin.h>
#elif defined(__aarch64__)
    #include <arm_neon.h>
#endif

// ==========================================
// BROADCASTING LOGIC (Dimension Alignment)
// ==========================================

bool db_tensor_reconcile_broadcast(ObjTensor* a, ObjTensor* b, 
                                  uint32_t* outRank, uint32_t* outShape) {
    uint32_t rankA = a->rank;
    uint32_t rankB = b->rank;
    uint32_t maxRank = (rankA > rankB) ? rankA : rankB;
    *outRank = maxRank;

    for (int i = 0; i < (int)maxRank; i++) {
        // Align shapes to the right (trailing dimensions)
        int idxA = (int)rankA - 1 - i;
        int idxB = (int)rankB - 1 - i;
        int idxOut = (int)maxRank - 1 - i;

        uint32_t dimA = (idxA >= 0) ? a->shape[idxA] : 1;
        uint32_t dimB = (idxB >= 0) ? b->shape[idxB] : 1;

        if (dimA == dimB) {
            outShape[idxOut] = dimA;
        } else if (dimA == 1) {
            outShape[idxOut] = dimB;
        } else if (dimB == 1) {
            outShape[idxOut] = dimA;
        } else {
            return false; // Incompatible dimensions (e.g. 3 vs 5)
        }
    }
    return true;
}

// ==========================================
// N-DIMENSIONAL RECURSIVE KERNEL (The Stride-Walker)
// ==========================================

/**
 * @brief High-Velocity N-D Stride Iterator.
 * 
 * Logic:
 * This kernel descends through the tensor dimensions. When it reaches 
 * the innermost dimension, it fires the SIMD engine.
 * 
 * UFO SPEED: If it detects zero-strides (broadcasting), it stays in 
 * the L1 cache. If it detects contiguous memory, it uses Aligned Loads.
 */
static void tensor_op_recursive(uint32_t dim, uint32_t rank, uint32_t* outShape,
                               float* ptrA, int32_t* stridesA,
                               float* ptrB, int32_t* stridesB,
                               float* ptrR, int32_t* stridesR,
                               OpCode op) {
    if (dim == rank - 1) {
        // --- INNERMOST SIMD LOOP ---
        uint32_t count = outShape[dim];
        int32_t sA = stridesA[dim];
        int32_t sB = stridesB[dim];
        int32_t sR = stridesR[dim];

        // OPTIMIZATION: Check for Contiguous SIMD Path
        if (sA == 4 && sB == 4 && sR == 4) {
            // Physical Hardware Saturation (Linear SIMD)
            uint32_t i = 0;
#ifdef __AVX512F__
            for (; i <= count - 16; i += 16) {
                __m512 va = _mm512_loadu_ps(&ptrA[i]);
                __m512 vb = _mm512_loadu_ps(&ptrB[i]);
                __m512 vr = (op == OP_ADD) ? _mm512_add_ps(va, vb) : _mm512_mul_ps(va, vb);
                _mm512_storeu_ps(&ptrR[i], vr);
            }
#endif
            for (; i < count; i++) {
                ptrR[i] = (op == OP_ADD) ? (ptrA[i] + ptrB[i]) : (ptrA[i] * ptrB[i]);
            }
        } else {
            // General Strided/Broadcast Path
            for (uint32_t i = 0; i < count; i++) {
                *ptrR = (op == OP_ADD) ? (*ptrA + *ptrB) : (*ptrA * *ptrB);
                ptrA = (float*)((uint8_t*)ptrA + sA);
                ptrB = (float*)((uint8_t*)ptrB + sB);
                ptrR = (float*)((uint8_t*)ptrR + sR);
            }
        }
        return;
    }

    // --- OUTER DIMENSION RECURSION ---
    for (uint32_t i = 0; i < outShape[dim]; i++) {
        float* nextA = (float*)((uint8_t*)ptrA + (i * stridesA[dim]));
        float* nextB = (float*)((uint8_t*)ptrB + (i * stridesB[dim]));
        float* nextR = (float*)((uint8_t*)ptrR + (i * stridesR[dim]));
        
        tensor_op_recursive(dim + 1, rank, outShape, 
                            nextA, stridesA, nextB, stridesB, nextR, stridesR, op);
    }
}

// ==========================================
// FUSED KERNEL ADDITION
// ==========================================

void db_tensor_add_fused(ObjTensor* a, ObjTensor* b, ObjTensor* res) {
    uint32_t outRank;
    uint32_t outShape[8];
    
    if (!db_tensor_reconcile_broadcast(a, b, &outRank, outShape)) return;

    // Normalizing Strides for Broadcasting (Zero-Stride Trick)
    int32_t normStridesA[8], normStridesB[8], normStridesR[8];
    
    for (int i = 0; i < (int)outRank; i++) {
        int idxA = (int)a->rank - 1 - ((int)outRank - 1 - i);
        int idxB = (int)b->rank - 1 - ((int)outRank - 1 - i);
        
        // If dimension is broadcasted, set physical stride to 0
        normStridesA[i] = (idxA >= 0 && a->shape[idxA] != 1) ? a->strides[idxA] : 0;
        normStridesB[i] = (idxB >= 0 && b->shape[idxB] != 1) ? b->strides[idxB] : 0;
        normStridesR[i] = res->strides[i];
    }

    tensor_op_recursive(0, outRank, outShape, 
                        a->data, normStridesA, 
                        b->data, normStridesB, 
                        res->data, normStridesR, OP_ADD);

    PROBE_NEURAL_SIMD(res->totalElements);
}

// ==========================================
// TILED MATRIX MULTIPLICATION (L3 SATURATION)
// ==========================================

void db_tensor_matmul(ObjTensor* a, ObjTensor* b, ObjTensor* res) {
    // 1. Geometry Analysis
    uint32_t M = a->shape[0];
    uint32_t K = a->shape[1]; // Must equal b->shape[0]
    uint32_t N = b->shape[1];

    // 2. Tiling (64x64 blocks to fit in L2 Cache)
    const uint32_t TILE_SIZE = 64;

    for (uint32_t i = 0; i < M; i += TILE_SIZE) {
        for (uint32_t j = 0; j < N; j += TILE_SIZE) {
            for (uint32_t k = 0; k < K; k += TILE_SIZE) {
                
                // 3. INNER BLOCK KERNEL
                // Performs optimized FMA on tiles
                for (uint32_t ii = i; ii < i + TILE_SIZE && ii < M; ii++) {
                    for (uint32_t jj = j; jj < j + TILE_SIZE && jj < N; jj++) {
                        float sum = (k == 0) ? 0.0f : *(res->data + (ii * res->strides[0]) + (jj * res->strides[1]));
                        
                        // Hardware-direct Dot Product over the K-dimension tile
                        float* vecA = a->data + (ii * a->strides[0]) + (k * a->strides[1]);
                        float* vecB = b->data + (k * b->strides[0]) + (jj * b->strides[1]);
                        
                        // We use the db_vector_dot_product from Batch 4
                        sum += db_vector_dot_product(vecA, vecB, (k + TILE_SIZE > K) ? K - k : TILE_SIZE);
                        
                        *(res->data + (ii * res->strides[0]) + (jj * res->strides[1])) = sum;
                    }
                }
            }
        }
    }
    
    markCard(res->data); // Atomic Persistence Checkpoint
}
