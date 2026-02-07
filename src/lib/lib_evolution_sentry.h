#ifndef WALIA_LIB_EVOLUTION_SENTRY_H
#define WALIA_LIB_EVOLUTION_SENTRY_H

#include "core/common.h"
#include "core/value.h"
#include "vm/vm_gene.h"

// ==========================================
// FORWARD DECLARATIONS
// ==========================================
struct ObjList; 

// ==========================================
// SELECTION SENTRY API
// ==========================================

/**
 * @brief Native implementation of 'natural_selection(population, rate)'.
 */
Value walia_natural_selection(int argCount, Value* args);

/**
 * @brief Performs a hardware-level audit of the population health.
 * Signature standardized to struct pointer to resolve conflicting types.
 */
void walia_evolution_audit(struct ObjList* population);

#endif // WALIA_LIB_EVOLUTION_SENTRY_H
