#include <stdio.h>
#include <string.h>

#include "lib_collections.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"
#include "native.h"

// ==========================================
// MAP IMPLEMENTATION
// ==========================================

ObjMap* walia_map_new() {
    ObjMap* map = (ObjMap*)reallocate(NULL, 0, sizeof(ObjMap));
    map->obj.type = OBJ_MAP; 
    map->obj.isMarked = false;
    map->obj.next = vm.objects;
    vm.objects = (Obj*)map;
    initTable(&map->table);
    markCard(map);
    return map;
}

Value walia_native_map_new(int argCount, Value* args) {
    (void)argCount; (void)args;
    return OBJ_VAL(walia_map_new());
}

void walia_map_set_internal(ObjMap* map, ObjString* key, Value val) {
    tableSet(&map->table, key, val);
    markCard(map);
}

bool walia_map_get_internal(ObjMap* map, ObjString* key, Value* val) {
    return tableGet(&map->table, key, val);
}

// ==========================================
// LIST IMPLEMENTATION
// ==========================================

ObjList* walia_list_new() {
    ObjList* list = (ObjList*)reallocate(NULL, 0, sizeof(ObjList));
    list->obj.type = OBJ_LIST;
    list->obj.isMarked = false;
    list->obj.next = vm.objects;
    vm.objects = (Obj*)list;
    initValueArray(&list->array);
    markCard(list);
    return list;
}

Value walia_native_list_new(int argCount, Value* args) {
    (void)argCount; (void)args;
    return OBJ_VAL(walia_list_new());
}

Value walia_list_new_from_array(int count, Value* elements) {
    ObjList* list = walia_list_new();
    if (count > 0 && elements != NULL) {
        list->array.values = ALLOCATE(Value, count);
        list->array.capacity = count;
        list->array.count = count;
        memcpy(list->array.values, elements, sizeof(Value) * count);
    }
    markCard(list);
    return OBJ_VAL(list);
}

// ==========================================
// LIST ACCESSORS
// ==========================================

int walia_list_len_internal(ObjList* list) {
    if (list == NULL) return 0;
    return list->array.count;
}

Value walia_list_get_internal(ObjList* list, int index) {
    if (list == NULL || index < 0 || index >= list->array.count) {
        return NIL_VAL;
    }
    return list->array.values[index];
}

// Wrapper functions for external use (Value-based API)
int walia_list_len(Value list) {
    if (!IS_OBJ(list)) return 0;
    Obj* obj = AS_OBJ(list);
    if (obj->type != OBJ_LIST) return 0;
    return walia_list_len_internal((ObjList*)obj);
}

Value walia_list_get(Value list, int index) {
    if (!IS_OBJ(list)) return NIL_VAL;
    Obj* obj = AS_OBJ(list);
    if (obj->type != OBJ_LIST) return NIL_VAL;
    return walia_list_get_internal((ObjList*)obj, index);
}

// ==========================================
// BOOTSTRAP
// ==========================================

void initWaliaCollections() {
    defineNative("Map", walia_native_map_new);
    defineNative("List", walia_native_list_new);
    printf(">> Walia Collections: Native substrate online.\n");
}