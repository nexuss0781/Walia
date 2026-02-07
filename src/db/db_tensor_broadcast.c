#include <stdio.h>
#include <string.h>

#include "db_tensor_broadcast.h"

// ==========================================
// COMPATIBILITY ANALYSIS
// ==========================================

bool db_tensor_can_broadcast(ObjTensor* a, ObjTensor* b, uint32_t* outRank, uint32_t* outShape) {
    uint32_t maxRank = (a->rank > b->rank) ? a->rank : b->rank;
    *outRank = maxRank;

    for (int i = 0; i < (int)maxRank; i++) {
        // Align to the right: compare trailing dimensions
        int idxA = (int)a->rank - 1 - i;
        int idxB = (int)b->rank - 1 - i;
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
            // Incompatible shapes (e.g. 3 and 5)
            return false;
        }
    }

    return true;
}

// ==========================================
// VIRTUAL EXPANSION (The Zero-Stride Trick)
// ==========================================

void db_tensor_get_broadcast_view(ObjTensor* source, uint32_t targetRank, const uint32_t* targetShape, WldBroadcastView* outView) {
    outView->rank = targetRank;
    outView->data = (float*)((uint8_t*)source->data + source->bufferOffset);

    // 1. Initialize result metadata
    for (int i = 0; i < (int)targetRank; i++) {
        outView->shape[i] = targetShape[i];
        
        // 2. Map source dimensions to target (right-aligned)
        int sourceDimIdx = (int)source->rank - 1 - ((int)targetRank - 1 - i);
        
        if (sourceDimIdx >= 0) {
            uint32_t sDim = source->shape[sourceDimIdx];
            uint32_t tDim = targetShape[i];

            if (sDim == tDim) {
                // Dimensions match: use original stride
                outView->strides[i] = source->strides[sourceDimIdx];
            } else {
                // SOVEREIGN OPTIMIZATION: sDim must be 1. 
                // We set stride to 0 to virtually expand the tensor.
                // Every access in this dimension will read the same address.
                outView->strides[i] = 0;
            }
        } else {
            // Source is lower rank: this dimension is implicitly 1. 
            // Stride is 0 to broadcast across the new high-order dimension.
            outView->strides[i] = 0;
        }
    }
}
