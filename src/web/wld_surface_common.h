#ifndef WALIA_WLD_SURFACE_COMMON_H
#define WALIA_WLD_SURFACE_COMMON_H

#include "core/common.h"

// ==========================================
// VISUAL RESOLUTION STANDARDS
// ==========================================

#define WLD_RES_WIDTH        1920
#define WLD_RES_HEIGHT       1080
#define WLD_PIXEL_PITCH      4       // 4 bytes per pixel (32-bit)
#define WLD_SCANLINE_BYTES   (WLD_RES_WIDTH * WLD_PIXEL_PITCH)

// ==========================================
// HARDWARE ALIGNMENT (SIMD READY)
// ==========================================
// scanlines are padded to 64 bytes (512-bit) to ensure that 
// every AVX-512 'vmovaps' instruction executes without penalty.

#define WLD_SIMD_STRIDE      ((WLD_SCANLINE_BYTES + 63) & ~63)
#define WLD_SURFACE_SIZE     (WLD_SIMD_STRIDE * WLD_RES_HEIGHT)

// ==========================================
// TILED RASTERIZATION GEOMETRY
// ==========================================
// The screen is divided into 16x16 'Sovereign Tiles'. 
// Each tile is a work unit for the Parallel Dispatcher.

#define WLD_TILE_SIZE        16 
#define WLD_TILES_X          (WLD_RES_WIDTH / WLD_TILE_SIZE)
#define WLD_TILES_Y          (WLD_RES_HEIGHT / WLD_TILE_SIZE)
#define WLD_TOTAL_TILES      (WLD_TILES_X * WLD_TILES_Y)

// ==========================================
// PIXEL FORMAT (BGRA 8888)
// ==========================================
/**
 * @brief Represents a single 32-bit pixel.
 * Industry Standard: BGRA layout for direct /dev/fb0 mapping.
 */
typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} WldPixel;

typedef uint32_t WldColor;

// ==========================================
// TILE DESCRIPTOR
// ==========================================

typedef struct {
    uint16_t x;          // Horizontal tile index
    uint16_t y;          // Vertical tile index
    bool     isDirty;    // True if card-marking flagged this segment
    uint32_t lastCommit; // LSN of the last render pass
} WldTile;

// ==========================================
// COLOR MACROS (Zero-Cost Boxing)
// ==========================================

#define WLD_RGB(r, g, b)     ((uint32_t)((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define WLD_RGBA(r, g, b, a) ((uint32_t)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

#define WLD_GET_R(c) (((c) >> 16) & 0xFF)
#define WLD_GET_G(c) (((c) >> 8) & 0xFF)
#define WLD_GET_B(c) ((c) & 0xFF)
#define WLD_GET_A(c) (((c) >> 24) & 0xFF)

#endif // WALIA_WLD_SURFACE_COMMON_H
