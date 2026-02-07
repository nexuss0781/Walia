#ifndef WALIA_TABLE_H
#define WALIA_TABLE_H

#include "common.h"
#include "value.h"

// Forward declaration of the struct tag only.
// The actual typedef belongs to object.h.
struct ObjString;

typedef struct {
    struct ObjString* key; // Use the struct tag
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, struct ObjString* key, Value* value);
bool tableSet(Table* table, struct ObjString* key, Value value);
bool tableDelete(Table* table, struct ObjString* key);
void tableAddAll(Table* from, Table* to);
struct ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);

void tableMark(Table* table);
void tableRemoveWhite(Table* table);

#endif // WALIA_TABLE_H
