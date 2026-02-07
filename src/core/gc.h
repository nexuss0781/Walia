#ifndef WALIA_GC_H
#define WALIA_GC_H

#include "common.h"
#include "value.h"

// Forward Declaration
struct Obj;

/**
 * @brief Main GC Entry Point.
 */
void collectGarbage();

/**
 * @brief Marks a NaN-boxed value as reachable.
 */
void markValue(Value value);

/**
 * @brief Marks a persistent heap object as reachable.
 */
void markObject(struct Obj* object);

#endif // WALIA_GC_H
