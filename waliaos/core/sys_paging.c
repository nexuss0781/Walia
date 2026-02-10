#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"

// Page Directory and Page Table alignment is critical (4KB)
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

/**
 * @brief Initializes the Paging Unit.
 * Sets up identity mapping for the first 4MB of RAM (where the kernel lives).
 */
void sys_paging_init() {
    // 1. Initialize Page Directory
    // Each entry is 4 bytes. 1024 entries = 4KB.
    // Attributes: 0x02 = Read/Write, but NOT present.
    for(int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }

    // 2. Setup First Page Table (Identity Map 4MB)
    // We map the physical address identical to the virtual address.
    for(unsigned int i = 0; i < 1024; i++) {
        // Attributes: 0x03 = Present + Read/Write
        first_page_table[i] = (i * 4096) | 3;
    }

    // 3. Link Table to Directory
    // Index 0 manages 0x00000000 to 0x003FFFFF
    page_directory[0] = ((uint32_t)first_page_table) | 3;

    // 4. Handover to Hardware
    // Load CR3 with the address of the page directory
    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory));

    // Enable paging by setting the PG bit (bit 31) in CR0
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    k_vga_print("[HARDEN] Paging Unit Active (Identity Mapped 4MB).\n");
}
