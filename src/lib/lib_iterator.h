#ifndef WALIA_LIB_ITERATOR_H
#define WALIA_LIB_ITERATOR_H

#include "core/common.h"
#include "core/value.h"
#include "core/object.h"

// ==========================================
// NATIVE ITERATOR STRUCTURES
// ==========================================

/**
 * @brief High-performance Iterator for Walia Lists.
 */
typedef struct {
    struct Obj obj;
    struct ObjList* list; // The target list
    int index;            // Current position
} ObjListIterator;

// ==========================================
// ITERATOR API
// ==========================================

/**
 * @brief Creates a new List Iterator.
 */
ObjListIterator* newListIterator(struct ObjList* list);

/**
 * @brief Advances the iterator and returns the next value.
 * @return Value The next item, or NIL_VAL if done.
 * @param done Output flag set to true if iteration is complete.
 */
Value listIteratorNext(ObjListIterator* iter, bool* done);

/**
 * @brief Initializes the Iterator Library.
 */
void initIteratorLibrary();

#endif // WALIA_LIB_ITERATOR_H
