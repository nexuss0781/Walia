#include <string.h>
#include <stdio.h>

#include "db_row.h"
#include "../core/object.h"
#include "../core/table.h"
#include "../core/memory.h"
#include "../lib/lib_collections.h"

// ==========================================
// SERIALIZATION (Walia Map -> Binary Row)
// ==========================================

WldResult db_row_pack(WldTableDescriptor* schema, Value object, uint8_t* outRow) {
    // 1. PHYSICAL TYPE VALIDATION
    if (!IS_OBJ(object) || AS_OBJ(object)->type != OBJ_MAP) {
        return WLD_ERR_IO; // Input must be a Sovereign Map
    }
    ObjMap* map = (ObjMap*)AS_OBJ(object);

    // 2. ZERO-COPY MARSHALLING
    // We iterate through the schema and lift values from the map table
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        WldColumn* col = &schema->columns[i];

        // O(1) Interned string lookup
        ObjString* fieldName = copyString(col->name, (int)strlen(col->name));
        Value val = NIL_VAL;
        
        walia_map_get_internal(map, fieldName, &val);

        // UFO SPEED: 1:1 bit pattern imaging
        // No conversion to strings or intermediate formats.
        memcpy(outRow + col->offset, &val, sizeof(Value));
    }

    return WLD_OK;
}

// ==========================================
// DESERIALIZATION (Binary Row -> Walia Map)
// ==========================================

Value db_row_unpack(WldTableDescriptor* schema, const uint8_t* rowData) {
    // 1. Allocate Sovereign Map on the persistent heap
    // Fixed: Signature synchronized with lib_collections.h
    ObjMap* map = walia_map_new();
    
    // 2. HYDRATION PASS
    for (uint32_t i = 0; i < schema->columnCount; i++) {
        WldColumn* col = &schema->columns[i];

        // Lift the raw 8-byte value from the physical disk image
        Value val;
        memcpy(&val, rowData + col->offset, sizeof(Value));

        // Intern the key and store logic in the map table
        ObjString* key = copyString(col->name, (int)strlen(col->name));
        walia_map_set_internal(map, key, val);
    }

    return OBJ_VAL(map);
}

// ==========================================
// HARDWARE-DIRECT ACCESS
// ==========================================

Value db_row_get_column(WldTableDescriptor* schema, const uint8_t* rowData, int colIndex) {
    if (colIndex < 0 || (uint32_t)colIndex >= schema->columnCount) {
        return NIL_VAL;
    }

    WldColumn* col = &schema->columns[colIndex];
    
    // PHYSICAL SPEED: The CPU reads the NaN-boxed value in a single cycle.
    // This is the absolute limit of hardware performance.
    return *(Value*)(rowData + col->offset);
}
