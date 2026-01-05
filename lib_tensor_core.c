#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_tensor.h"
#include "memory.h"
#include "vm.h"

// ==========================================
// INTERNAL HELPERS
// ==========================================

/**
 * @brief Computes Row-Major strides for a given shape.
 */
static void compute_row_major_strides(uint32_t rank, const uint32_t* shape, int32_t* strides) {
    int32_t currentStride = sizeof(float);
    for (int i = (int)rank - 1; i >= 0; i--) {
        strides[i] = currentStride;
        currentStride *= shape[i];
    }
}

// ==========================================
// CORE LIFECYCLE
// ==========================================

ObjTensor* walia_tensor_new(uint32_t rank, const uint32_t* shape) {
    if (rank > WLD_TENSOR_MAX_RANK) return NULL;

    // 1. Calculate Total Memory Requirements
    uint64_t elements = 1;
    for (uint32_t i = 0; i < rank; i++) elements *= shape[i];
    size_t dataSize = elements * sizeof(float);

    // 2. Allocate Descriptor and Aligned Data Buffer
    // We utilize the persistent reallocate() substrate
    ObjTensor* tensor = (ObjTensor*)reallocate(NULL, 0, sizeof(ObjTensor));
    
    // Initialize standard object header
    tensor->obj.type = OBJ_NATIVE; 
    tensor->obj.isMarked = false;
    tensor->obj.next = vm.objects;
    vm.objects = (struct Obj*)tensor;

    // SIMD Alignment (512-bit)
    size_t paddedSize = dataSize + WLD_SIMD_ALIGNMENT;
    uint8_t* rawBuffer = (uint8_t*)reallocate(NULL, 0, paddedSize);
    tensor->data = (float*)(((uintptr_t)rawBuffer + (WLD_SIMD_ALIGNMENT - 1)) & ~(uintptr_t)(WLD_SIMD_ALIGNMENT - 1));

    // 3. Metadata Population
    tensor->rank = rank;
    tensor->totalElements = elements;
    tensor->bufferOffset = 0;
    tensor->isOwner = true;
    memcpy(tensor->shape, shape, sizeof(uint32_t) * rank);
    compute_row_major_strides(rank, shape, tensor->strides);

    // 4. Persistence Sentry
    markCard(tensor);
    markCard(tensor->data);

    return tensor;
}

ObjTensor* walia_tensor_transpose(ObjTensor* source, uint32_t dim1, uint32_t dim2) {
    if (dim1 >= source->rank || dim2 >= source->rank) return NULL;

    // Create a new descriptor pointing to the same data
    ObjTensor* view = (ObjTensor*)reallocate(NULL, 0, sizeof(ObjTensor));
    memcpy(view, source, sizeof(ObjTensor));
    
    // Reset object header for GC
    view->obj.next = vm.objects;
    vm.objects = (struct Obj*)view;
    view->isOwner = false;

    // UFO SPEED: Swap strides and shapes
    uint32_t tempShape = view->shape[dim1];
    view->shape[dim1] = view->shape[dim2];
    view->shape[dim2] = tempShape;

    int32_t tempStride = view->strides[dim1];
    view->strides[dim1] = view->strides[dim2];
    view->strides[dim2] = tempStride;

    markCard(view);
    return view;
}

ObjTensor* walia_tensor_reshape(ObjTensor* source, uint32_t newRank, const uint32_t* newShape) {
    // 1. Verify Logical Integrity
    uint64_t newElements = 1;
    for (uint32_t i = 0; i < newRank; i++) newElements *= newShape[i];

    if (newElements != source->totalElements) {
        // Fixed: Use %llu for uint64_t values
        fprintf(stderr, "Walia Tensor Error: Reshape size mismatch (%llu != %llu)\n",
                (unsigned long long)newElements, (unsigned long long)source->totalElements);
        return NULL;
    }

    // 2. Create View
    ObjTensor* view = (ObjTensor*)reallocate(NULL, 0, sizeof(ObjTensor));
    memcpy(view, source, sizeof(ObjTensor));
    view->obj.next = vm.objects;
    vm.objects = (struct Obj*)view;
    view->isOwner = false;

    // 3. Re-calculate Metadata
    view->rank = newRank;
    memcpy(view->shape, newShape, sizeof(uint32_t) * newRank);
    compute_row_major_strides(newRank, newShape, view->strides);

    markCard(view);
    return view;
}