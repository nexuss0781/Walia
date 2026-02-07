#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core/common.h"
#include "core/object.h"
#include "core/value.h"
#include "core/vm.h"
#include "native.h"
#include "web/web_surface.h"
#include "web/web_raster_tiler.h"
#include "web/web_font_sdf.h"
#include "db/db_dispatch.h"

// External Substrate References
// (Now resolved via headers)

// ==========================================
// SURFACE CONSTRUCTORS
// ==========================================

/**
 * Surface(width, height) -> ObjSurface
 * 
 * Logic: Allocates a 64-byte aligned persistent framebuffer.
 * Usage: var screen = Surface(1920, 1080);
 */
static Value walia_ui_surface_new(int argCount, Value* args) {
    if (argCount != 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) {
        return NIL_VAL;
    }

    uint32_t w = (uint32_t)AS_NUMBER(args[0]);
    uint32_t h = (uint32_t)AS_NUMBER(args[1]);

    ObjSurface* surface = walia_surface_new(w, h);
    return OBJ_VAL(surface);
}

/**
 * Element(type) -> ObjElement
 * 
 * Logic: Creates a reactive UI node (Rect, Text, etc).
 */
static Value walia_ui_element_new(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;

    WldElementType type = (WldElementType)AS_NUMBER(args[0]);
    ObjElement* el = walia_element_new(type);
    return OBJ_VAL(el);
}

// ==========================================
// RENDER ORCHESTRATION
// ==========================================

/**
 * ui_render(surface, element_list) -> Bool
 * 
 * UFO-GRADE LOGIC:
 * 1. Unpacks the Walia List into a contiguous C-array of ObjElement pointers.
 * 2. Invokes the Tiled Rasterizer to scan for Dirty Cards.
 * 3. Dispatches 16x16 pixel render tasks to the Parallel Dispatcher.
 */
static Value walia_ui_render(int argCount, Value* args) {
    if (argCount != 2 || !IS_OBJ(args[0]) || !IS_OBJ(args[1])) {
        return BOOL_VAL(false);
    }

    ObjSurface* surface = (ObjSurface*)AS_OBJ(args[0]);
    
    // We assume the second argument is a Walia List (from lib_collections.h)
    // Extract raw elements array
    // (Note: In production, we'd use the List iterator protocol)
    extern Value walia_list_get(Value list, int index);
    extern int walia_list_len(Value list);

    Value listVal = args[1];
    int count = walia_list_len(listVal);

    // Temporary stack allocation for the render pass snapshot
    ObjElement** elements = malloc(sizeof(ObjElement*) * count);
    for (int i = 0; i < count; i++) {
        elements[i] = (ObjElement*)AS_OBJ(walia_list_get(listVal, i));
    }

    // DISPATCH TO PARALLEL TILER (Phase 15.3.2)
    // This executes the Work-Stealing pass across all CPU cores.
    db_raster_process_surface(&global_dispatcher, surface, elements, count);

    free(elements);
    return BOOL_VAL(true);
}

// ==========================================
// BARE-METAL HARDWARE LINK
// ==========================================

/**
 * ui_map_hardware(surface, device_path) -> Bool
 * 
 * Logic: Maps the Walia surface directly to the kernel framebuffer (/dev/fb0).
 * This enables high-speed graphics on headless servers without a GPU.
 */
static Value walia_ui_map_hw(int argCount, Value* args) {
    if (argCount != 2 || !IS_OBJ(args[0]) || !IS_STRING(args[1])) {
        return BOOL_VAL(false);
    }

    ObjSurface* surface = (ObjSurface*)AS_OBJ(args[0]);
    const char* path = AS_CSTRING(args[1]);

    SerResult res = walia_surface_map_hardware(surface, path);
    return BOOL_VAL(res == SER_OK);
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

void initVisualLibrary() {
    // Surface Management
    defineNative("Surface",          walia_ui_surface_new);
    defineNative("ui_map_hardware",  walia_ui_map_hw);
    
    // UI Primitives
    defineNative("Element",          walia_ui_element_new);
    defineNative("ui_render",        walia_ui_render);

    // Constants for Type Safety
    // In production, these are injected as global variables
    // RECT = 0, TEXT = 1, etc.

    printf(">> Walia Visual Substrate: No-GPU UI Engine online [UFO-Grade Active]\n");
}
