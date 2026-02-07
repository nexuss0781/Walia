#ifndef WALIA_SYS_CACHE_ALIGN_H
#define WALIA_SYS_CACHE_ALIGN_H

#include "core/common.h"

// ==========================================
// SOVEREIGN ALIGNMENT MACROS
// ==========================================
/**
 * These macros ensure that critical objects do not span 
 * across two different cache lines, preventing 'Split-Load' penalties.
 */

#if defined(__GNUC__) || defined(__clang__)
    #define WLD_ALIGN_CACHE __attribute__((aligned(64)))
#elif defined(_MSC_VER)
    #define WLD_ALIGN_CACHE __declspec(align(64))
#else
    #define WLD_ALIGN_CACHE
#endif

/**
 * @brief UFO-GRADE PADDING
 * Ensures a structure occupies exactly N cache lines.
 */
#define WLD_CACHE_LINE_SIZE 64
#define WLD_PAD_TO_CACHE_LINE(size) \
    char _padding[WLD_CACHE_LINE_SIZE - (size % WLD_CACHE_LINE_SIZE)]

// ==========================================
// UPDATED CORE STRUCTURES (Phase 18)
// ==========================================

/**
 * @brief High-Performance Cache-Aligned Object Header.
 * 
 * By aligning the header to 64 bytes, we ensure that metadata 
 * and the start of the object's fields live in the same L1 cache fetch.
 */
typedef struct WLD_ALIGN_CACHE {
    uint8_t  type;
    uint8_t  generation; // Phase 4 GC support
    uint16_t flags;
    uint32_t metadata;
    struct Obj* next;
} ObjHeader;

#endif // WALIA_SYS_CACHE_ALIGN_H
