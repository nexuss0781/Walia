#ifndef WALIA_WEB_SURFACE_H
#define WALIA_WEB_SURFACE_H

#include "../web/wld_event_common.h" // Fixed: Added for SerResult definition
#include "../web/wld_surface_common.h"
#include "../core/object.h"
#include "../core/value.h"

// ==========================================
// UI ELEMENT TYPES
// ==========================================

typedef enum {
    UI_RECT,
    UI_TEXT,
    UI_IMAGE,
    UI_CONTAINER
} WldElementType;

/**
 * @brief Represents a logical UI node in the Sovereign heap.
 */
typedef struct {
    struct Obj obj;
    WldElementType type;
    
    // Geometry
    float x, y;
    float width, height;
    
    // Styling
    WldColor color;
    bool     isVisible;
    int      zIndex;

    // Persistence Link
    uint32_t lastKnownPage; 
} ObjElement;

/**
 * @brief The Sovereign Surface (Framebuffer Descriptor).
 */
typedef struct {
    struct Obj obj;
    
    uint32_t width;
    uint32_t height;
    uint32_t stride;      // Bytes per scanline (SIMD-aligned)
    
    WldPixel* pixels;    // Pointer to memory-mapped pixel array
    size_t    bufferSize;
    
    bool      isPersistent; // True if backed by .wld
    bool      isHardware;   // True if mapped to /dev/fb0
} ObjSurface;

// ==========================================
// SURFACE API
// ==========================================

/**
 * @brief Allocates a new Persistent Surface.
 * Aligns the pixel buffer to 64-byte boundaries for SIMD saturation.
 */
ObjSurface* walia_surface_new(uint32_t width, uint32_t height);

/**
 * @brief Maps the surface to a hardware framebuffer device.
 * Used for bare-metal "No-GPU" execution.
 */
SerResult walia_surface_map_hardware(ObjSurface* surface, const char* fbPath);

/**
 * @brief Clears the surface with a specific color.
 * UFO Speed: Uses SIMD unrolling (web_simd_core.c) internally.
 */
void walia_surface_clear(ObjSurface* surface, WldColor color);

/**
 * @brief Creates a high-level UI element.
 */
ObjElement* walia_element_new(WldElementType type);

#endif // WALIA_WEB_SURFACE_H
