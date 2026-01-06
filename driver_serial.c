#include "walia_kernel_base.h"
#include "driver_serial.h"

// ==========================================
// UART HARDWARE PORTS (COM1)
// ==========================================
#define COM1_BASE 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ==========================================
// SERIAL INITIALIZATION
// ==========================================

void k_serial_init() {
    // 1. Disable all interrupts
    outb(COM1_BASE + 1, 0x00);
    // 2. Enable DLAB (set baud rate divisor)
    outb(COM1_BASE + 3, 0x80);
    // 3. Set divisor to 3 (38400 baud)
    outb(COM1_BASE + 0, 0x03); // Low byte
    outb(COM1_BASE + 1, 0x00); // High byte
    // 4. 8 bits, no parity, one stop bit
    outb(COM1_BASE + 3, 0x03);
    // 5. Enable FIFO, clear with 14-byte threshold
    outb(COM1_BASE + 2, 0xC7);
    // 6. IRQs enabled, RTS/DSR set
    outb(COM1_BASE + 4, 0x0B);
}

// ==========================================
// ATOMIC TELEMETRY OUTPUT
// ==========================================

static int is_transmit_empty() {
    // Check Line Status Register (Bit 5)
    return inb(COM1_BASE + 5) & 0x20;
}

void k_serial_putc(char c) {
    // Wait-Free check: Poll hardware buffer
    while (is_transmit_empty() == 0) {
        __asm__ volatile ("pause");
    }
    outb(COM1_BASE, c);
}

void k_serial_print(const char* str) {
    while (*str) {
        // Handle CR/LF for serial terminal compatibility
        if (*str == '\n') k_serial_putc('\r');
        k_serial_putc(*str++);
    }
}
