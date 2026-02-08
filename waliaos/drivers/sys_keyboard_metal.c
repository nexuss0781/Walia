#include "../include/sys_shell.h"
#include "src/tooling/tooling_nexus_bus.h"
#include "../include/driver_serial.h"

// INTERNAL STATE
static bool shift_active = false;
static bool caps_lock    = false;

// ==========================================
// HARDWARE ACCESS (The Metal)
// ==========================================

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ==========================================
// SOVEREIGN SCANCODE MAP (US QWERTY)
// ==========================================
static const char scancode_map[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' '	/* Space bar */
};

// ==========================================
// KEYBOARD ENGINE
// ==========================================

void sys_keyboard_init() {
    k_serial_print(">> Walia Input: Seizing PS/2 Keyboard Controller...\n");
    
    // Drain the keyboard buffer
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    k_serial_print(">> Walia Input: Sovereign Key-Map Loaded [US_QWERTY].\n");
}

void sys_keyboard_poll() {
    // 1. Check Status Register (Bit 0 = Output Buffer Full)
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    
    if (status & 0x01) {
        // 2. READ RAW BIT PATTERN
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        // 3. LOGIC STATE TRACKING
        // Scancodes above 0x80 represent a 'Key Release' (Break Code)
        if (scancode & 0x80) {
            uint8_t release_code = scancode & 0x7F;
            if (release_code == 42 || release_code == 54) shift_active = false;
        } else {
            // Key Pressed (Make Code)
            if (scancode == 42 || scancode == 54) {
                shift_active = true;
                return;
            }

            char c = scancode_map[scancode];
            if (c != 0) {
                // UFO SPEED: EMIT PULSE
                // We inject the keypress directly into the high-speed bus.
                // targetId stores the ASCII, payload stores the Shift flag.
                walia_pulse_emit(PULSE_SCAN_FILE, (uint32_t)c, (uint64_t)shift_active);
            }
        }
    }
}

char sys_keyboard_to_ascii(uint8_t scancode, bool shifted) {
    if (scancode >= sizeof(scancode_map)) return 0;
    char c = scancode_map[scancode];
    
    // Apply shift transformation
    if (shifted && c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}
