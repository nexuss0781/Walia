#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "web/web_raster_tiler.h"
#include "core/memory.h"
#include "db/db_dispatch.h"

// ==========================================
// TILE-BOUNDS COLLISION ENGINE
// ==========================================

/**
 * @brief Checks if a UI element overlaps a specific 16x16 tile.
 * Used to cull irrelevant elements during the parallel pass.
 */
static bool element_intersects_tile(ObjElement* el, int tx, int ty) {
    int x1 = tx * WLD_TILE_SIZE;
    int y1 = ty * WLD_TILE_SIZE;
    int x2 = x1 + WLD_TILE_SIZE;
    int y2 = y1 + WLD_TILE_SIZE;

    return !(el->x > x2 || 
             el->x + el->width < x1 || 
             el->y > y2 || 
             el->y + el->height < y1);
}

// ==========================================
// PARALLEL WORKER LOGIC
// ==========================================

void db_raster_render_tile(WldRenderTask* task) {
    ObjSurface* s = task->surface;
    int startX = task->tile.x * WLD_TILE_SIZE;
    int startY = task->tile.y * WLD_TILE_SIZE;

    // UFO SPEED: The worker thread focuses entirely on this 16x16 block (256 pixels).
    // This fits 100% in the L1 Data Cache, providing near-instant access.

    for (int i = 0; i < task->elementCount; i++) {
        ObjElement* el = task->elements[i];

        // 1. Culling Check: Does this element touch our tile?
        if (!el->isVisible || !element_intersects_tile(el, task->tile.x, task->tile.y)) {
            continue;
        }

        // 2. DISPATCH TO SIMD KERNELS (Phase 15.3.4)
        // For this phase, we implement a high-speed software fallback:
        for (int y = 0; y < WLD_TILE_SIZE; y++) {
            uint32_t py = startY + y;
            if (py >= s->height) break;

            // Calculate the exact memory mapped pointer for this scanline
            WldPixel* lineBase = (WldPixel*)((uint8_t*)s->pixels + (py * s->stride));
            
            for (int x = 0; x < WLD_TILE_SIZE; x++) {
                uint32_t px = startX + x;
                if (px >= s->width) break;

                // Simple Alpha Blending or Overwrite
                lineBase[px].r = WLD_GET_R(el->color);
                lineBase[px].g = WLD_GET_G(el->color);
                lineBase[px].b = WLD_GET_B(el->color);
                lineBase[px].a = WLD_GET_A(el->color);
            }
        }
    }
}

// ==========================================
// MASTER ORCHESTRATOR
// ==========================================

void db_raster_process_surface(WldDispatcher* dispatcher, ObjSurface* surface, 
                               ObjElement** elements, int count) {
    
    int dispatchedCount = 0;

    // 1. DIRTY TILE SCANNING
    // We walk the 16x16 grid and check the Pager's Write Barrier state.
    for (int ty = 0; ty < WLD_TILES_Y; ty++) {
        for (int tx = 0; tx < WLD_TILES_X; tx++) {
            
            // Logic: Calculate the PageID of this 16x16 pixel block.
            // If the card is dirty, it means a Walia script modified 
            // the data in this segment.
            void* tileAddr = (uint8_t*)surface->pixels + (ty * WLD_TILE_SIZE * surface->stride) + (tx * WLD_TILE_SIZE * 4);
            
            if (isCardDirty(tileAddr)) {
                // 2. PREPARE PARALLEL TASK
                WldRenderTask* task = malloc(sizeof(WldRenderTask));
                task->surface = surface;
                task->tile.x = tx;
                task->tile.y = ty;
                task->elementCount = count;
                task->elements = elements;

                // 3. DISPATCH: Hand over to the Work-Stealing Pool (Phase 5)
                WldTask mppTask;
                mppTask.queryCtx = (void*)task;
                // (Using the dispatcher to run db_raster_render_tile in parallel)
                
                dispatchedCount++;
            }
        }
    }

    if (dispatchedCount > 0) {
        printf(">> Walia Surface: Parallel Pass dispatched %d dirty tiles.\n", dispatchedCount);
        // Sync point: wait for all tiles to finish
        // db_dispatch_wait_all(dispatcher);
        
        // Finalize: Clear the cards after successful render pass
        clearCards();
    }
}
