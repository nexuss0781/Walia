#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vm_gene.h"
#include "memory.h"
#include "vm.h"

// ==========================================
// GENE BLUEPRINT LIFECYCLE
// ==========================================

ObjGene* walia_gene_new(ObjString* name) {
    ObjGene* gene = (ObjGene*)reallocate(NULL, 0, sizeof(ObjGene));
    
    // Initialize Header
    gene->obj.type = OBJ_NATIVE; 
    gene->obj.isMarked = false;
    gene->obj.next = vm.objects;
    vm.objects = (struct Obj*)gene;

    gene->name = name;
    gene->alleleCount = 0;
    initTable(&gene->methods);

    markCard(gene);
    return gene;
}

void walia_gene_add_trait(ObjGene* gene, ObjString* name, double min, double max) {
    if (gene->alleleCount >= MAX_ALLELES) return;

    WldAlleleRange* r = &gene->ranges[gene->alleleCount++];
    r->name = name;
    r->min = min;
    r->max = max;

    markCard(gene);
}

// ==========================================
// GENOME SPAWNING (The Primordial Soup)
// ==========================================

/**
 * @brief Internal helper to generate random double in range.
 */
static double rand_range(double min, double max) {
    double scale = (double)rand() / (double)RAND_MAX;
    return min + scale * (max - min);
}

ObjGenome* walia_genome_spawn(ObjGene* schema) {
    ObjGenome* genome = (ObjGenome*)reallocate(NULL, 0, sizeof(ObjGenome));
    
    genome->obj.type = OBJ_NATIVE;
    genome->obj.isMarked = false;
    genome->obj.next = vm.objects;
    vm.objects = (struct Obj*)genome;

    genome->schema = schema;
    genome->generation = 0;
    genome->fitnessScore = 0.0;
    memset(genome->parent1Hash, 0, 32);
    memset(genome->parent2Hash, 0, 32);

    // INITIAL MUTATION: Populate DNA based on the schema's ranges
    for (int i = 0; i < schema->alleleCount; i++) {
        genome->alleles[i] = rand_range(schema->ranges[i].min, schema->ranges[i].max);
    }

    markCard(genome);
    
    printf(">> Walia Biology: Spawned individual of gene '%s' [Gen 0]\n", schema->name->chars);
    return genome;
}