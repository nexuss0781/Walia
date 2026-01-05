#ifndef WALIA_VM_GENE_H
#define WALIA_VM_GENE_H

#include "common.h"
#include "object.h"
#include "table.h"

// ==========================================
// GENETIC SPECIFICATIONS
// ==========================================

#define MAX_ALLELES 64 // Maximum mutable properties per gene

/**
 * @brief Represents the range of potential for a single genetic trait.
 */
typedef struct {
    ObjString* name;
    double min;
    double max;
} WldAlleleRange;

/**
 * @brief The Genetic Blueprint (The Gene).
 * Stored in the persistent heap.
 */
typedef struct {
    struct Obj obj;
    ObjString* name;
    
    // Schema of mutable traits
    WldAlleleRange ranges[MAX_ALLELES];
    int alleleCount;
    
    // Behavioral Methods (Logic)
    Table methods;
} ObjGene;

/**
 * @brief The physical instance of a Gene (The Genome).
 */
typedef struct {
    struct Obj obj;
    ObjGene*   schema;
    
    // The specific DNA values for this individual.
    // Contiguous array for SIMD-accelerated breeding.
    double     alleles[MAX_ALLELES];
    
    // Lineage Tracking (Sovereign Ancestry)
    uint8_t    parent1Hash[32];
    uint8_t    parent2Hash[32];
    uint64_t   generation;
    
    double     fitnessScore;
} ObjGenome;

// ==========================================
// DNA SUBSTRATE API
// ==========================================

/**
 * @brief Allocates a new Gene blueprint.
 */
ObjGene* walia_gene_new(ObjString* name);

/**
 * @brief Adds a mutable range to a gene definition.
 */
void walia_gene_add_trait(ObjGene* gene, ObjString* name, double min, double max);

/**
 * @brief Spawns a new individual (Genome) from a Gene blueprint.
 * Logic: Randomly initializes alleles within the defined ranges.
 */
ObjGenome* walia_genome_spawn(ObjGene* schema);

#endif // WALIA_VM_GENE_H