#ifndef WALIA_LIB_COLLECTIONS_H
#define WALIA_LIB_COLLECTIONS_H

#include "../core/common.h"
#include "../core/object.h"
#include "../core/value.h"

// ==========================================
// SOVEREIGN LIST (Dynamic Persistent Array)
// ==========================================

typedef struct ObjList {
    Obj        obj;
    ValueArray array;
} ObjList;

// ==========================================
// SOVEREIGN MAP (Persistent Hash Table)
// ==========================================

typedef struct ObjMap {
    Obj   obj;
    Table table;
} ObjMap;

// ==========================================
// COLLECTIONS API
// ==========================================

/**
 * @brief Creates a new persistent list.
 */
ObjList* walia_list_new();
void     walia_list_add(ObjList* list, Value value);
Value    walia_list_get(Value list, int index);
void     walia_list_set(ObjList* list, int index, Value value);
Value    walia_list_new_from_array(int count, Value* elements);

/**
 * @brief Creates a new persistent hash map.
 */
ObjMap*  walia_map_new();
void     walia_map_set(ObjMap* map, ObjString* key, Value value);
bool     walia_map_get(ObjMap* map, ObjString* key, Value* result);

// Internal helpers for database layer
void walia_map_set_internal(ObjMap* map, ObjString* key, Value val);
bool walia_map_get_internal(ObjMap* map, ObjString* key, Value* val);

// Internal list helpers
int  walia_list_len(Value list);
int  walia_list_len_internal(ObjList* list);
Value walia_list_get_internal(ObjList* list, int index);
void walia_list_set_internal(ObjList* list, int index, Value value);
void walia_list_add_internal(ObjList* list, Value value);

/**
 * @brief Bootstraps the Standard Collections for the VM.
 */
void initWaliaCollections();

#endif // WALIA_LIB_COLLECTIONS_H
