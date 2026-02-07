#include <stdio.h>
#include <string.h>

#include "tooling/tooling_nexus_widgets.h"

// ==========================================
// INTERNAL UI HELPERS
// ==========================================

static void draw_frame(WldCanvas* cv, int x, int y, int w, int h, const char* title) {
    // Draw Border
    for (int i = 0; i < w; i++) {
        db_canvas_put(cv, x + i, y, '-', COLOR_GRAY, false);
        db_canvas_put(cv, x + i, y + h - 1, '-', COLOR_GRAY, false);
    }
    for (int i = 0; i < h; i++) {
        db_canvas_put(cv, x, y + i, '|', COLOR_GRAY, false);
        db_canvas_put(cv, x + w - 1, y + i, '|', COLOR_GRAY, false);
    }
    // Draw Title
    db_canvas_draw_text(cv, x + 2, y, title, COLOR_WHITE);
}

// ==========================================
// PAGEMAP WIDGET (The Database Pulse)
// ==========================================

void db_widget_draw_pagemap(WldCanvas* cv, int x, int y, int w, int h) {
    draw_frame(cv, x, y, w, h, "SOVEREIGN PAGE MAP");

    int innerW = w - 2;
    int innerH = h - 2;
    
    // UFO Speed Visualization:
    // We represent the .wld file as a heat-map grid. 
    // In production, this data comes from the Pager's Card Table.
    for (int row = 0; row < innerH; row++) {
        for (int col = 0; col < innerW; col++) {
            // Simulated Page States
            WldColor color = COLOR_GRAY;
            char glyph = '.';
            
            int sample = (row * col) % 15;
            if (sample == 1) { color = COLOR_CYAN; glyph = 'S'; }    // SQL
            if (sample == 2) { color = COLOR_MAGENTA; glyph = 'N'; } // NoSQL
            if (sample == 3) { color = COLOR_RED; glyph = '#'; }     // Dirty/Shadow

            db_canvas_put(cv, x + 1 + col, y + 1 + row, glyph, color, false);
        }
    }
}

// ==========================================
// HEAPTANK WIDGET (The Sovereign Reaper)
// ==========================================

void db_widget_draw_heaptank(WldCanvas* cv, int x, int y, size_t nursery, size_t mature) {
    draw_frame(cv, x, y, 24, 8, "HEAP TANKS");

    // Tank 1: Nursery (Gen 0)
    db_canvas_draw_text(cv, x + 2, y + 2, "NURSERY:", COLOR_WHITE);
    int nWidth = (int)((float)nursery / 1024 * 10); // Scaled
    for (int i = 0; i < 10; i++) {
        char c = (i < nWidth) ? '=' : ' ';
        db_canvas_put(cv, x + 11 + i, y + 2, c, COLOR_GREEN, true);
    }

    // Tank 2: Mature (Gen 1)
    db_canvas_draw_text(cv, x + 2, y + 4, "MATURE: ", COLOR_WHITE);
    int mWidth = (int)((float)mature / 1024 * 10);
    for (int i = 0; i < 10; i++) {
        char c = (i < mWidth) ? '=' : ' ';
        db_canvas_put(cv, x + 11 + i, y + 4, c, COLOR_YELLOW, true);
    }
}

// ==========================================
// NEURAL GAUGE (HNSW Navigation)
// ==========================================

void db_widget_draw_neural_gauge(WldCanvas* cv, int x, int y, int h, int level, float sim) {
    draw_frame(cv, x, y, 12, h, "NEURAL");

    // Level Indicator (HNSW Layers 16 -> 0)
    for (int i = 0; i < h - 4; i++) {
        WldColor c = (i >= (h - 4 - level)) ? COLOR_CYAN : COLOR_GRAY;
        db_canvas_put(cv, x + 5, y + 2 + i, '>', c, true);
    }

    // Similarity Score
    char score[16];
    sprintf(score, "%.2f", sim);
    db_canvas_draw_text(cv, x + 2, y + h - 2, score, COLOR_YELLOW);
}

// ==========================================
// EXECUTION LANES (MPP Monitoring)
// ==========================================

void db_widget_draw_lanes(WldCanvas* cv, int x, int y, int w, int count, int* loads) {
    draw_frame(cv, x, y, w, count + 3, "EXECUTION LANES");

    for (int i = 0; i < count; i++) {
        char label[10];
        sprintf(label, "C%02d", i);
        db_canvas_draw_text(cv, x + 2, y + 2 + i, label, COLOR_WHITE);
        
        int loadWidth = (loads[i] * (w - 10)) / 100;
        for (int j = 0; j < (w - 10); j++) {
            char c = (j < loadWidth) ? '|' : '.';
            db_canvas_put(cv, x + 7 + j, y + 2 + i, c, COLOR_CYAN, false);
        }
    }
}
