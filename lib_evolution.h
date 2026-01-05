#ifndef WALIA_LIB_EVOLUTION_H
#define WALIA_LIB_EVOLUTION_H

#include "vm_gene.h"
#include "lib_collections.h"

// ==========================================
// EVOLUTIONARY ENGINE API
// ==========================================

/**
 * @brief Performs Genetic Recombination on two parent genomes.
 * 
 * UFO-GRADE LOGIC:
 * 1. Uses SIMD bitmasks to randomly select alleles from Parent A or B.
 * 2. Processes 8 alleles (512 bits) per clock cycle.
 * 3. Injects Gaussian Mutation based on the current mutation rate.
 * 
 * @param p1 Source Genome Alpha
 * @param p2 Source Genome Beta
 * @return ObjGenome* The resulting biological descendant.
 */
ObjGenome* walia_evolve_breed(ObjGenome* p1, ObjGenome* p2, float mutationRate);

/**
 * @brief Executes the 'Natural Selection' pass on a population.
 * 
 * Logic:
 * 1. Sorts the population by fitnessScore.
 * 2. Culls the bottom (1.0 - survivalRate) percentage.
 * 3. Returns a new list containing only the Sovereign Survivors.
 */
Value walia_evolve_select(Value populationList, float survivalRate);

/**
 * @brief Bootstraps the Evolutionary Standard Library.
 */
void initEvolutionLibrary();

#endif // WALIA_LIB_EVOLUTION_H