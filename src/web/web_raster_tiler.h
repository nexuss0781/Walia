#ifndef WALIA_WEB_RASTER_TILER_H
#define WALIA_WEB_RASTER_TILER_H

#include "web/wld_surface_common.h"
#include "web/web_surface.h"
#include "db/db_dispatch.h"

// ==========================================
// RENDER CONTEXT
// ==========================================

/**
 * @brief Thread-local context for a single tile render task.
 */
typedef struct {
    ObjSurface* surface;
    WldTile     tile;
    int         elementCount;
    ObjElement** elements; // Snapshot of visible elements
} WldRenderTask;

// ==========================================
// TILER API
// ==========================================

/**
 * @brief Initiates a project-wide parallel render pass.
 * 
 * Logic:
 * 1. Scans the surface for dirty memory cards.
 * 2. Identifies affected 16x16 tiles.
 * 3. Dispatches render tasks to the MPP Dispatcher (Phase 5).
 * 4. Merges the result into the physical framebuffer.
 */
void db_raster_process_surface(WldDispatcher* dispatcher, ObjSurface* surface, 
                               ObjElement** elements, int count);

/**
 * @brief The actual worker function that rasterizes a 16x16 block.
 * Executed by the worker threads in parallel.
 */
void db_raster_render_tile(WldRenderTask* task);

#endif // WALIA_WEB_RASTER_TILER_H
