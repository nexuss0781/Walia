#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_vector.h"
#include "../core/memory.h"
#include "../core/vm.h"

// = = = = = = = = = = = = = = = = = = = = = =
// INTERNAL ALIGNMENT LOGIC
// = = = = = = = = = = = = = = = = = = = = = =

/**
 * @brief Custom allocator for SIMD-aligned persistent objects.
 * Trillion-scale neural math requires 64-byte (512-bit) alignment.
 */
static void* allocateAligned(size_t size) {
    // 1. Calculate required size including padding for 64-byte alignment
    size_t totalSize = size + 64; 
    
    // 2. Allocate from the Walia Persistent Heap (mmap-backed)
    uint8_t* raw = (uint8_t*)reallocate(NULL, 0, totalSize);
    
    // 3. Align the pointer
    uint8_t* aligned = (uint8_t*)(((uintptr_t)raw + 63) & ~(uintptr_t)63);
    
    return (void*)aligned;
}

// = = = = = = = = = = = = = = = = = = = = = =
// VECTOR LIFECYCLE
// = = = = = = = = = = = = = = = = = = = = = =

ObjVector* db_vector_new(uint32_t dims, WldVectorFormat format) {
    size_t elementSize = (format == WLD_VECTOR_F32) ? sizeof(float) : 1;
    size_t dataSize = dims * elementSize;
    
    // Allocate the struct + flexible array with SIMD alignment
    ObjVector* vec = (ObjVector*)allocateAligned(sizeof(ObjVector) + dataSize);
    
    // Initialize Walia Object Header
    vec->obj.type = OBJ_CLOSURE; // Vectors are treated as complex types for Phase 4
    vec->obj.isMarked = false;
    vec->obj.next = vm.objects;
    vm.objects = (Obj*)vec;

    vec->dimensions = dims;
    vec->format = (uint16_t)format;

    // Zero-out the neural data
    memset(vec->data, 0, dataSize);

    // WALIA WRITE BARRIER: Ensure this massive neural block is persisted
    markCard(vec);
    
    return vec;
}

void db_vector_set(ObjVector* vec, uint32_t index, float value) {
    if (index >= vec->dimensions) return;

    if (vec->format == WLD_VECTOR_F32) {
        vec->data[index] = value;
    } else {
        // Handle quantized formats (SQ8/BIT) - Logic for Phase 4.2/4.4
    }

    // Mark the card containing this specific neuron as dirty
    markCard(&vec->data[index]);
}

// = = = = = = = = = = = = = = = = = = = = = =
// DEBUG DIAGNOSTICS
// = = = = = = = = = = = = = = = = = = = = = =

void db_vector_print(ObjVector* vec) {
    printf("[Vector dim=%u fmt=%u ptr=%p] ", 
           vec->dimensions, vec->format, (void*)vec->data);
    
    int limit = vec->dimensions > 5 ? 5 : vec->dimensions;
    for (int i = 0; i < limit; i++) {
        printf("%f ", vec->data[i]);
    }
    if (vec->dimensions > 5) printf("...");
    printf("\n");
}
