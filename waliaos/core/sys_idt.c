#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"

struct idt_entry_struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

extern void idt_flush(uint32_t);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void sys_pic_remap() {
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    // ICW2 (Remap IRQ 0-15 to 32-47)
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    // ICW3
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    // Mask all interrupts
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

extern uint32_t static_isr_table[];
extern void irq0();
extern void irq1();

void sys_idt_init() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // CPU Exception handlers (vectors 0-31)
    for(int i = 0; i < 32; i++) {
        idt_set_gate(i, static_isr_table[i], 0x08, 0x8E);
    }

    // Hardware IRQ handlers (vectors 32-33)
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // Keyboard

    sys_pic_remap();
    
    // Unmask IRQ0 (Timer) and IRQ1 (Keyboard) only
    // Mask = 0xFC = 11111100 (bits 0 and 1 cleared = unmasked)
    outb(0x21, 0xFC);
    // Keep slave PIC fully masked
    outb(0xA1, 0xFF);
    
    idt_flush((uint32_t)&idt_ptr);
    k_vga_print("[HARDEN] IDT Gates Locked & PIC Remapped.\n");
}
