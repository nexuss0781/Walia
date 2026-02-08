#ifndef WALIA_TOOLING_NEXUS_WIDGETS_H
#define WALIA_TOOLING_NEXUS_WIDGETS_H

#include "../tooling/tooling_nexus_canvas.h"

// ==========================================
// WIDGET DATA STRUCTURES
// ==========================================

typedef struct {
    int x, y, w, h;
    const char* title;
} WldWidgetHeader;

// ==========================================
// SOVEREIGN WIDGET API
// ==========================================

/**
 * @brief Draws a Grid representation of the .wld file segments.
 * 
 * Colors:
 * - Cyan: SQL Table Pages
 * - Magenta: NoSQL Collection Pages
 * - Red: Dirty Shadow Pages (In-flight Transactions)
 * - Gray: Free/Unallocated Space
 */
void db_widget_draw_pagemap(WldCanvas* cv, int x, int y, int w, int h);

/**
 * @brief Draws a dual-meter showing the Nursery and Mature heap levels.
 */
void db_widget_draw_heaptank(WldCanvas* cv, int x, int y, size_t nurseryUsed, size_t matureUsed);

/**
 * @brief Draws a vertical gauge for Neural HNSW navigation.
 */
void db_widget_draw_neural_gauge(WldCanvas* cv, int x, int y, int h, int currentLevel, float similarity);

/**
 * @brief Draws the Parallel Execution Lanes (64-core monitoring).
 */
void db_widget_draw_lanes(WldCanvas* cv, int x, int y, int w, int coreCount, int* coreLoads);

#endif // WALIA_TOOLING_NEXUS_WIDGETS_H
