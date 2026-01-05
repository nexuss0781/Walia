#ifndef WALIA_WEB_FONT_SDF_H
#define WALIA_WEB_FONT_SDF_H

#include "wld_surface_common.h"
#include "web_surface.h"
#include "object.h"

// ==========================================
// FONT SPECIFICATIONS
// ==========================================

#define WLD_SDF_SIZE 64 // Standard 64x64 distance map per glyph

/**
 * @brief Represents a single persistent character.
 */
typedef struct {
    uint32_t codepoint;
    uint16_t width;
    uint16_t height;
    int16_t  bearingX;
    int16_t  bearingY;
    uint32_t advance;
    
    // The Distance Field: Each byte represents the distance 
    // to the nearest edge (128 = on the edge).
    uint8_t  distanceMap[WLD_SDF_SIZE * WLD_SDF_SIZE];
} WldSDFGlyph;

/**
 * @brief The Sovereign Font Object.
 * Stored in the persistent heap for instant UI re-hydration.
 */
typedef struct {
    struct Obj obj;
    ObjString* fontName;
    uint16_t   baseSize;
    
    // Persistent Map of Codepoint -> WldSDFGlyph
    // (Logic simplified for Phase 15.3 integration)
    WldSDFGlyph* glyphs;
    int          glyphCount;
} ObjFont;

// ==========================================
// FONT ENGINE API
// ==========================================

/**
 * @brief Renders a string of text onto a surface using SIMD unrolling.
 * 
 * @param surface The target framebuffer.
 * @param font    The persistent font object.
 * @param text    The UTF-8 string to render.
 * @param x, y    Screen coordinates.
 * @param scale   Size multiplier.
 * @param color   32-bit BGRA color.
 */
void wld_font_draw_text(ObjSurface* surface, ObjFont* font, const char* text, 
                        float x, float y, float scale, WldColor color);

#endif // WALIA_WEB_FONT_SDF_H