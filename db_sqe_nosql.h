#ifndef WALIA_DB_SQE_NOSQL_H
#define WALIA_DB_SQE_NOSQL_H

#include "db_common.h"
#include "db_vector.h"
#include "db_quantize.h"
#include "chunk.h"

// ==========================================
// DYNAMIC JIT SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a Compiled Neural Search Plan.
 */
typedef struct {
    struct Obj obj;
    uint8_t    queryHash[32]; // SHA-256 of the vector search criteria
    ObjFunction* jitFunc;     // Optimized SIMD loop
} WldNeuralPlan;

// ==========================================
// SOVEREIGN JIT API
// ==========================================

/**
 * @brief JIT Compiles a high-dimensional similarity search.
 * 
 * Generates an Asymmetric Distance loop that processes 
 * SQ8 quantized neurons using SIMD-unrolling.
 */
WldNeuralPlan* db_sqe_vector_jit(ObjVector* query, WldDistanceMetric metric);

/**
 * @brief JIT Compiles a NoSQL Filter (Document Scan).
 * 
 * Unlike SQL, this JIT handles variable-type Walia Objects 
 * using the VM's internal dynamic-dispatch tags.
 */
ObjFunction* db_sqe_nosql_filter_jit(const char* fieldName, Value targetValue);

#endif // WALIA_DB_SQE_NOSQL_H
