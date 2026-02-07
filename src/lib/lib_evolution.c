#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif

#include "lib/lib_evolution.h"
#include "core/memory.h"
#include "core/vm.h"
#include "native.h"

// ==========================================
// HARDWARE-ACCELERATED CROSSOVER
// ==========================================

/**
 * @brief SIMD Kernel for Recombination.
 * Fuses two DNA buffers based on a randomized hardware mask.
 */
static void crossover_simd(double* out, const double* a, const double* b, int count) {
    int i = 0;

#ifdef __AVX512F__
    // UFO SPEED: Process 8 double-precision alleles per cycle
    for (; i <= count - 8; i += 8) {
        __m512d v_a = _mm512_loadu_pd(&a[i]);
        __m512d v_b = _mm512_loadu_pd(&b[i]);
        
        // Generate a random 8-bit mask (1 bit per double)
        __mmask8 mask = (__mmask8)rand();
        
        // Atomic Selection: res = (mask_bit) ? v_a : v_b
        __m512d v_res = _mm512_mask_blend_pd(mask, v_b, v_a);
        
        _mm512_storeu_pd(&out[i], v_res);
    }
#endif

    // Scalar Fallback for remaining DNA segments
    for (; i < count; i++) {
        out[i] = (rand() % 2 == 0) ? a[i] : b[i];
    }
}

// ==========================================
// THE BREEDING ENGINE
// ==========================================

ObjGenome* walia_evolve_breed(ObjGenome* p1, ObjGenome* p2, float mutationRate) {
    if (p1->schema != p2->schema) return NULL; // Biological incompatibility

    ObjGenome* child = walia_genome_spawn(p1->schema);
    child->generation = (p1->generation > p2->generation ? p1->generation : p2->generation) + 1;

    // 1. SIMD RECOMBINATION
    crossover_simd(child->alleles, p1->alleles, p2->alleles, p1->schema->alleleCount);

    // 2. STOCHASTIC MUTATION (Gaussian Jitter)
    for (int i = 0; i < p1->schema->alleleCount; i++) {
        if (((float)rand() / (float)RAND_MAX) < mutationRate) {
            double range = p1->schema->ranges[i].max - p1->schema->ranges[i].min;
            double jitter = ((double)rand() / (double)RAND_MAX - 0.5) * range * 0.1;
            child->alleles[i] += jitter;

            // Clamp to Schema Bounds
            if (child->alleles[i] < p1->schema->ranges[i].min) child->alleles[i] = p1->schema->ranges[i].min;
            if (child->alleles[i] > p1->schema->ranges[i].max) child->alleles[i] = p1->schema->ranges[i].max;
        }
    }

    // 3. Lineage Fingerprinting
    // (Logic to copy SHA-256 parent hashes would go here)
    
    markCard(child);
    return child;
}

// ==========================================
// SELECTION LOGIC (MPP Culling)
// ==========================================

static int fitness_comparator(const void* a, const void* b) {
    ObjGenome* gA = (ObjGenome*)AS_OBJ(*(Value*)a);
    ObjGenome* gB = (ObjGenome*)AS_OBJ(*(Value*)b);
    if (gA->fitnessScore < gB->fitnessScore) return 1;
    if (gA->fitnessScore > gB->fitnessScore) return -1;
    return 0;
}

Value walia_evolve_select(Value populationList, float survivalRate) {
    ObjList* list = (ObjList*)AS_OBJ(populationList);
    if (list->array.count == 0) return populationList;

    // 1. UFO-Grade Sorting
    qsort(list->array.values, list->array.count, sizeof(Value), fitness_comparator);

    // 2. Identify Survivors
    int survivorCount = (int)(list->array.count * survivalRate);
    if (survivorCount < 1) survivorCount = 1;

    // 3. Prune the weak logic from the list
    list->array.count = survivorCount;
    
    markCard(list);
    return populationList;
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

static Value native_evolve(int argCount, Value* args) {
    if (argCount < 2) return NIL_VAL;
    ObjGenome* p1 = (ObjGenome*)AS_OBJ(args[0]);
    ObjGenome* p2 = (ObjGenome*)AS_OBJ(args[1]);
    float rate = (argCount == 3) ? (float)AS_NUMBER(args[2]) : 0.01f;
    return OBJ_VAL(walia_evolve_breed(p1, p2, rate));
}

void initEvolutionLibrary() {
    defineNative("evolve", native_evolve);
    printf(">> Walia Biology: Mutator Engine active [AVX-512 Accelerated]\n");
}
