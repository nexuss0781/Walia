#ifndef WALIA_LIB_COLLECTIONS_H
#define WALIA_LIB_COLLECTIONS_H

#include "common.h"
#include "object.h"
#include "value.h"

// ==========================================
// SOVEREIGN LIST (Dynamic Persistent Array)
// ==========================================

typedef struct {
    Value* values;
    int    count;
    int    capacity;
} WldValueArray;

typedef struct {
    Obj           obj;
    WldValueArray array;
} ObjList;

// ==========================================
// SOVEREIGN MAP (Persistent Hash Table)
// ==========================================

typedef struct {
    ObjString* key;
    Value      value;
} WldMapEntry;

typedef struct {
    Obj          obj;
    int          count;
    int          capacity;
    WldMapEntry* entries;
} ObjMap;

// ==========================================
// COLLECTIONS API
// ==========================================

/**
 * @brief Creates a new persistent list.
 */
ObjList* walia_list_new();
void     walia_list_add(ObjList* list, Value value);
Value    walia_list_get(ObjList* list, int index);

/**
 * @brief Creates a new persistent hash map.
 */
ObjMap*  walia_map_new();
void     walia_map_set(ObjMap* map, ObjString* key, Value value);
bool     walia_map_get(ObjMap* map, ObjString* key, Value* result);

/**
 * @brief Bootstraps the Standard Collections for the VM.
 */
void initWaliaCollections();

#endif // WALIA_LIB_COLLECTIONS_H
