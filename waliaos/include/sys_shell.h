#ifndef WALIA_SYS_SHELL_H
#define WALIA_SYS_SHELL_H

#include "walia_kernel_base.h"

// ==========================================
// KEYBOARD HARDWARE PORTS
// ==========================================
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64

// ==========================================
// SOVEREIGN KEYBOARD SPECIFICATIONS
// ==========================================

typedef enum {
    KEY_NONE = 0,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_ENTER, KEY_BACKSPACE, KEY_TAB, KEY_ESC,
    KEY_SHIFT, KEY_CTRL, KEY_ALT,
    KEY_CHAR // Standard alphanumeric
} WldKeyType;

/**
 * @brief Represents a physical key interaction.
 */
typedef struct {
    WldKeyType type;
    char       ascii;    // Valid if type == KEY_CHAR
    bool       isDown;   // True = Pressed, False = Released
    uint8_t    scanCode; // Raw hardware ID
} WldKeyEvent;

// ==========================================
// SHELL SUBSTRATE API
// ==========================================

/**
 * @brief Initializes the PS/2 Keyboard controller.
 * Clears hardware buffers and enables IRQ1.
 */
void sys_keyboard_init();

/**
 * @brief Reaps the next available scancode from the controller.
 * 
 * UFO SPEED: Performs a non-blocking hardware check. 
 * If a key is found, it emits a PULSE_KEY_DOWN to the Fabric.
 */
void sys_keyboard_poll();

/**
 * @brief Translates raw scancodes to the Sovereign ASCII set.
 */
char sys_keyboard_to_ascii(uint8_t scancode, bool shifted);

#endif // WALIA_SYS_SHELL_H
