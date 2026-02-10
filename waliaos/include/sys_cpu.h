#ifndef WALIA_SYS_CPU_H
#define WALIA_SYS_CPU_H

#include "walia_kernel_base.h"

// --- PORT I/O ---
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

// --- GDT, IDT & PAGING ---
void sys_gdt_init();
void sys_idt_init();
void sys_paging_init();

#endif // WALIA_SYS_CPU_H
