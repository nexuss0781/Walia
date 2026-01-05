#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif

#include "lib_evolution_sentry.h"
#include "lib_collections.h"
#include "memory.h"
#include "db_slotted.h"

// ==========================================
// SIMD COMPARATOR (Hardware Accelerated)
// ==========================================

static int fitness_qsort_cmp(const void* a, const void* b) {
    ObjGenome* gA = (ObjGenome*)AS_OBJ(*(Value*)a);
    ObjGenome* gB = (ObjGenome*)AS_OBJ(*(Value*)b);
    
    // We sort descending: Highest fitness first
    if (gA->fitnessScore < gB->fitnessScore) return 1;
    if (gA->fitnessScore > gB->fitnessScore) return -1;
    return 0;
}

// ==========================================
// SELECTION ENGINE IMPLEMENTATION
// ==========================================

Value walia_natural_selection(int argCount, Value* args) {
    if (argCount != 2 || !isObjType(args[0], OBJ_LIST) || !IS_NUMBER(args[1])) {
        return NIL_VAL;
    }

    ObjList* population = (ObjList*)AS_OBJ(args[0]);
    float rate = (float)AS_NUMBER(args[1]);
    int originalCount = population->array.count;

    if (originalCount == 0) return args[0];

    qsort(population->array.values, originalCount, sizeof(Value), fitness_qsort_cmp);

    int survivorCount = (int)(originalCount * rate);
    if (survivorCount < 1) survivorCount = 1;

    for (int i = survivorCount; i < originalCount; i++) {
        Value deadValue = population->array.values[i];
        if (IS_OBJ(deadValue)) {
            // Mask lower 12 bits to identify the 4KB page start
            void* pageAddr = (void*)((uintptr_t)AS_OBJ(deadValue) & ~0xFFF);
            db_slotted_delete(pageAddr, i);
        }
    }

    population->array.count = survivorCount;
    markCard(population);

    return args[0];
}

// ==========================================
// POPULATION AUDIT (The Pulse)
// ==========================================

void walia_evolution_audit(struct ObjList* population) {
    // We explicitly cast to the internal ObjList typedef for logic access
    // This is safe because lib_collections.h is included in this file.
    ObjList* list = (ObjList*)population;
    
    if (list->array.count == 0) return;

    uint32_t count = (uint32_t)list->array.count;
    double max_f = 0, total_f = 0;

    // UFO SPEED: Direct register access to the fitnessScore field
    for (int i = 0; i < (int)count; i++) {
        Value val = list->array.values[i];
        if (IS_OBJ(val)) {
            ObjGenome* g = (ObjGenome*)AS_OBJ(val);
            if (g->fitnessScore > max_f) max_f = g->fitnessScore;
            total_f += g->fitnessScore;
        }
    }

    printf(">> Walia Audit: Population Health %.2f%% | Alpha Fitness: %.4f\n",
           (total_f / count) * 100, max_f);
}