#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"
#include "../include/driver_serial.h"
#include "../include/sys_cpu_features.h"

// ==========================================
// SOVEREIGN DIAGNOSTIC SHELL
// A minimal interactive command-line for the
// hardened kernel environment.
// ==========================================

#define SHELL_BUF_SIZE 256
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64

// US QWERTY Scancode Map (Make Codes)
static const char scancode_lower[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
  '9', '0', '-', '=', '\b',
  '\t',
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
 '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',
  'm', ',', '.', '/',   0,
  '*',
    0,
  ' '
};

static const char scancode_upper[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',
  '(', ')', '_', '+', '\b',
  '\t',
  'Q', 'W', 'E', 'R',
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~',   0,
  '|', 'Z', 'X', 'C', 'V', 'B', 'N',
  'M', '<', '>', '?',   0,
  '*',
    0,
  ' '
};

static int shift_held = 0;

// ==========================================
// HELPER: String Compare (no libc)
// ==========================================
static int shell_strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

// ==========================================
// COMMAND HANDLERS
// ==========================================

static void cmd_help() {
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print("\n  Sovereign Diagnostic Shell Commands:\n");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    k_vga_print("  help     - Show this menu\n");
    k_vga_print("  status   - System status report\n");
    k_vga_print("  cpu      - CPU feature flags\n");
    k_vga_print("  mem      - Memory map summary\n");
    k_vga_print("  clear    - Clear screen\n");
    k_vga_print("  reboot   - Warm reboot\n\n");
}

static void cmd_status() {
    k_vga_set_color(VGA_GREEN, VGA_BLACK);
    k_vga_print("\n  [STATUS] Walia Sovereign OS v3.0.0\n");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    k_vga_print("  GDT:     ACTIVE (5 segments)\n");
    k_vga_print("  IDT:     ACTIVE (32 ISR gates)\n");
    k_vga_print("  Paging:  ACTIVE (PSE 4MB, 1GB mapped)\n");
    k_vga_print("  PIC:     Remapped (IRQ 32-47)\n");
    
    if (global_cpu_caps.is_safe_mode) {
        k_vga_set_color(VGA_YELLOW, VGA_BLACK);
        k_vga_print("  Mode:    SAFE MODE (Legacy Hardware)\n");
    } else {
        k_vga_set_color(VGA_GREEN, VGA_BLACK);
        k_vga_print("  Mode:    FULL SOVEREIGN\n");
    }
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    k_vga_print("\n");
}

static void cmd_cpu() {
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print("\n  [CPU] Iron Dome Feature Report:\n");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    
    k_vga_print("  SSE:       ");
    k_vga_print(global_cpu_caps.has_sse ? "YES\n" : "NO\n");
    
    k_vga_print("  SSE2:      ");
    k_vga_print(global_cpu_caps.has_sse2 ? "YES\n" : "NO\n");
    
    k_vga_print("  AVX:       ");
    k_vga_print(global_cpu_caps.has_avx ? "YES\n" : "NO\n");
    
    k_vga_print("  RDRAND:    ");
    k_vga_print(global_cpu_caps.has_rdrand ? "YES\n" : "NO\n");
    
    k_vga_print("  APIC:      ");
    k_vga_print(global_cpu_caps.has_apic ? "YES\n" : "NO\n");
    
    k_vga_print("  TSC:       ");
    k_vga_print(global_cpu_caps.has_tsc ? "YES\n" : "NO\n");
    
    k_vga_print("\n");
}

static void cmd_mem() {
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print("\n  [MEM] Memory Layout:\n");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    k_vga_print("  0x00000000 - 0x000FFFFF : BIOS / Real Mode (1MB)\n");
    k_vga_print("  0x00100000 - Kernel End  : Kernel Code + Data\n");
    k_vga_print("  Kernel End - +128MB      : Sovereign Heap\n");
    k_vga_print("  0x000B8000              : VGA Framebuffer\n");
    k_vga_print("  Paging: 256 x 4MB PSE pages (1GB identity)\n\n");
}

static void cmd_reboot() {
    k_vga_set_color(VGA_YELLOW, VGA_BLACK);
    k_vga_print("\n  [REBOOT] Performing warm reset...\n");
    // Triple fault to reboot: load an empty IDT and trigger an interrupt
    uint8_t empty_idt[6] = {0};
    __asm__ volatile ("lidt (%0)" : : "r"(empty_idt));
    __asm__ volatile ("int $3");
}

// ==========================================
// COMMAND DISPATCHER
// ==========================================
static void shell_execute(const char* cmd) {
    if (shell_strcmp(cmd, "help") == 0)        cmd_help();
    else if (shell_strcmp(cmd, "status") == 0) cmd_status();
    else if (shell_strcmp(cmd, "cpu") == 0)    cmd_cpu();
    else if (shell_strcmp(cmd, "mem") == 0)    cmd_mem();
    else if (shell_strcmp(cmd, "clear") == 0)  k_vga_init();
    else if (shell_strcmp(cmd, "reboot") == 0) cmd_reboot();
    else if (cmd[0] != '\0') {
        k_vga_set_color(VGA_RED, VGA_BLACK);
        k_vga_print("  Unknown command: ");
        k_vga_print(cmd);
        k_vga_print("\n  Type 'help' for available commands.\n");
        k_vga_set_color(VGA_WHITE, VGA_BLACK);
    }
}

// ==========================================
// KEYBOARD POLLING (Direct Hardware Access)
// ==========================================
static char shell_read_key() {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (!(status & 0x01)) return 0;  // No key available
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Key release (break code)
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        if (released == 42 || released == 54) shift_held = 0;
        return 0;
    }
    
    // Shift press
    if (scancode == 42 || scancode == 54) {
        shift_held = 1;
        return 0;
    }
    
    // Map scancode to ASCII
    if (scancode < sizeof(scancode_lower)) {
        return shift_held ? scancode_upper[scancode] : scancode_lower[scancode];
    }
    
    return 0;
}

// ==========================================
// THE SOVEREIGN SHELL LOOP
// ==========================================
void sys_shell_run() {
    char buf[SHELL_BUF_SIZE];
    int pos = 0;
    
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print("\n  ========================================\n");
    k_vga_print("   WALIA SOVEREIGN DIAGNOSTIC SHELL v1.0\n");
    k_vga_print("  ========================================\n");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    k_vga_print("  Type 'help' for commands.\n\n");
    
    // Print initial prompt
    k_vga_set_color(VGA_GREEN, VGA_BLACK);
    k_vga_print("wal> ");
    k_vga_set_color(VGA_WHITE, VGA_BLACK);
    
    while (1) {
        char c = shell_read_key();
        if (c == 0) {
            // No key - yield CPU to save power
            __asm__ volatile ("hlt");
            continue;
        }
        
        if (c == '\n') {
            // Execute command
            k_vga_print("\n");
            buf[pos] = '\0';
            
            shell_execute(buf);
            
            // Reset buffer and print new prompt
            pos = 0;
            k_vga_set_color(VGA_GREEN, VGA_BLACK);
            k_vga_print("wal> ");
            k_vga_set_color(VGA_WHITE, VGA_BLACK);
        } else if (c == '\b') {
            // Backspace
            if (pos > 0) {
                pos--;
                k_vga_backspace();
            }
        } else if (pos < SHELL_BUF_SIZE - 1) {
            // Normal character
            buf[pos++] = c;
            // Echo the character
            char str[2] = {c, '\0'};
            k_vga_print(str);
        }
    }
}
