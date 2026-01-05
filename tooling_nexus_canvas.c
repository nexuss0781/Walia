#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tooling_nexus_canvas.h"

// ==========================================
// ANSI ESCAPE COMMANDS
// ==========================================

#define ANSI_HIDE_CURSOR "\033[?25l"
#define ANSI_SHOW_CURSOR "\033[?25h"
#define ANSI_HOME        "\033[H"
#define ANSI_CLEAR       "\033[2J"

static const char* colorCodes[] = {
    "\033[0m",    // DEFAULT
    "\033[37m",   // WHITE
    "\033[36m",   // CYAN
    "\033[33m",   // YELLOW
    "\033[31m",   // RED
    "\033[32m",   // GREEN
    "\033[35m",   // MAGENTA
    "\033[90m"    // GRAY
};

static const char* boldCodes[] = {
    "",           // Standard
    "\033[1m"     // BOLD
};

// ==========================================
// CORE RENDERING ENGINE
// ==========================================

void db_canvas_init() {
    // 1. Hide the cursor to prevent "cursor ghosting" during high-speed updates
    printf(ANSI_HIDE_CURSOR);
    // 2. Clear the terminal and move to home position (0,0)
    printf(ANSI_CLEAR ANSI_HOME);
    fflush(stdout);
}

void db_canvas_clear(WldCanvas* canvas) {
    // Update current terminal dimensions
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    canvas->width = (w.ws_col < CANVAS_MAX_WIDTH) ? w.ws_col : CANVAS_MAX_WIDTH;
    canvas->height = (w.ws_row < CANVAS_MAX_HEIGHT) ? w.ws_row : CANVAS_MAX_HEIGHT;

    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            canvas->buffer[y][x].character = ' ';
            canvas->buffer[y][x].color = COLOR_DEFAULT;
            canvas->buffer[y][x].isBold = false;
        }
    }
}

void db_canvas_put(WldCanvas* canvas, int x, int y, char c, WldColor color, bool bold) {
    if (x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
        canvas->buffer[y][x].character = c;
        canvas->buffer[y][x].color = color;
        canvas->buffer[y][x].isBold = bold;
    }
}

void db_canvas_draw_text(WldCanvas* canvas, int x, int y, const char* text, WldColor color) {
    int len = (int)strlen(text);
    for (int i = 0; i < len; i++) {
        db_canvas_put(canvas, x + i, y, text[i], color, false);
    }
}

/**
 * @brief High-Speed Atomic Render.
 * 
 * Logic:
 * We construct a single massive string in memory representing the frame 
 * and write it to stdout in ONE operation. This is significantly faster 
 * than multiple printf calls and eliminates flickering.
 */
void db_canvas_render(WldCanvas* canvas) {
    char output[CANVAS_MAX_WIDTH * CANVAS_MAX_HEIGHT * 20]; // Buffer for ANSI + chars
    int ptr = 0;

    // Move cursor to top-left without clearing
    ptr += sprintf(output + ptr, ANSI_HOME);

    WldColor lastColor = COLOR_DEFAULT;
    bool lastBold = false;

    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            WldCell* cell = &canvas->buffer[y][x];

            // Optimization: Only send ANSI code if the color/style changed
            if (cell->color != lastColor || cell->isBold != lastBold) {
                ptr += sprintf(output + ptr, "%s%s", colorCodes[cell->color], boldCodes[cell->isBold]);
                lastColor = cell->color;
                lastBold = cell->isBold;
            }

            output[ptr++] = cell->character;
        }
        output[ptr++] = '\n';
    }

    // Terminate and write the entire frame at once
    output[ptr] = '\0';
    fputs(output, stdout);
    fflush(stdout);
}

void db_canvas_shutdown() {
    // Reset colors and show the cursor before exiting
    printf("\033[0m" ANSI_SHOW_CURSOR);
    fflush(stdout);
}
