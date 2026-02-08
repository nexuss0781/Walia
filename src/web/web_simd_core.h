#ifndef WALIA_WEB_SIMD_CORE_H
#define WALIA_WEB_SIMD_CORE_H

#include "../web/wld_surface_common.h"

// ==========================================
// SIMD CORE API
// ==========================================

/**
 * @brief Performs a hardware-accelerated fill of a memory segment.
 * 
 * UFO Speed: Uses aligned 512-bit stores to saturate the memory controller.
 * 
 * @param dest Pointer to the start of the pixel data (Must be 64-byte aligned).
 * @param count Number of pixels to fill.
 * @param color The 32-bit BGRA color value.
 */
void wld_simd_fill(WldPixel* dest, uint32_t count, WldColor color);

/**
 * @brief High-speed Alpha Blending of two pixel streams.
 * 
 * Logic: Performs the calculation (Src * Alpha + Dst * (1-Alpha)) 
 * for 16 pixels simultaneously using branchless SIMD masks.
 */
void wld_simd_blend(WldPixel* dest, const WldPixel* src, uint32_t count);

/**
 * @brief Zero-Copy Image Blitting.
 * Moves rectangular blocks of persistent image data into the framebuffer.
 */
void wld_simd_blit(WldPixel* dest, uint32_t destStride, 
                   const WldPixel* src, uint32_t srcStride,
                   uint32_t width, uint32_t height);

#endif // WALIA_WEB_SIMD_CORE_H
