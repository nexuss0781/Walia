#ifndef WALIA_TOOLING_NEXUS_CANVAS_H
#define WALIA_TOOLING_NEXUS_CANVAS_H

#include "core/common.h"

// ==========================================
// CANVAS SPECIFICATIONS
// ==========================================

#define CANVAS_MAX_WIDTH  160
#define CANVAS_MAX_HEIGHT 60

typedef enum {
    COLOR_DEFAULT,
    COLOR_WHITE,
    COLOR_CYAN,
    COLOR_YELLOW,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_MAGENTA,
    COLOR_GRAY
} WldColor;

/**
 * @brief Represents a single character cell on the HUD.
 */
typedef struct {
    char     character;
    WldColor color;
    bool     isBold;
} WldCell;

/**
 * @brief The Virtual Screen Buffer.
 * Prevents flickering by providing double-buffering logic.
 */
typedef struct {
    WldCell buffer[CANVAS_MAX_HEIGHT][CANVAS_MAX_WIDTH];
    int width;
    int height;
} WldCanvas;

// ==========================================
// CANVAS GRAPHICS API
// ==========================================

/**
 * @brief Initializes the terminal for TUI mode.
 * Hides the cursor and clears the screen.
 */
void db_canvas_init();

/**
 * @brief Clears the virtual buffer before a new frame is drawn.
 */
void db_canvas_clear(WldCanvas* canvas);

/**
 * @brief Draws a single character at a specific coordinate.
 */
void db_canvas_put(WldCanvas* canvas, int x, int y, char c, WldColor color, bool bold);

/**
 * @brief Draws a formatted string horizontally.
 */
void db_canvas_draw_text(WldCanvas* canvas, int x, int y, const char* text, WldColor color);

/**
 * @brief Flushes the virtual buffer to the physical terminal stdout.
 */
void db_canvas_render(WldCanvas* canvas);

/**
 * @brief Restores the terminal to standard mode.
 */
void db_canvas_shutdown();

#endif // WALIA_TOOLING_NEXUS_CANVAS_H
