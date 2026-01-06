
#ifndef WALIA_DB_VECTOR_MATCH_H
#define WALIA_DB_VECTOR_MATCH_H

#include "common.h"
#include "lib_vector.h"

// ==========================================
// COGNITIVE BRANCHING STRUCTURES
// ==========================================

/**
 * @brief Represents a single neural branch in a 'match' block.
 * Maps a target vector pattern to a bytecode jump offset.
 */
typedef struct {
    struct ObjVector* pattern; // The 'Thought' to match against
    int               jumpOffset; // Bytecode destination
} WldNeuralBranch;

// ==========================================
// DECISION ENGINE API
// ==========================================

/**
 * @brief Evaluates an array of neural patterns and selects the winner.
 * 
 * Logic:
 * 1. Performs SIMD Cosine Similarity across all patterns.
 * 2. Selects the branch with the highest proximity.
 * 3. Returns the winning index or -1 if below threshold.
 */
int db_vector_match_best(struct ObjVector* target, WldNeuralBranch* branches, int count);

/**
 * @brief Low-level HNSW search entry point.
 * Performs a logarithmic descent through the neural persistent heap.
 */
uint32_t db_vector_hnsw_search(const char* indexName, struct ObjVector* query, int k);

#endif // WALIA_DB_VECTOR_MATCH_H
