#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"

// Page Directory alignment is critical (4KB)
// Using PSE (Page Size Extension) for 4MB pages - no page tables needed!
uint32_t page_directory[1024] __attribute__((aligned(4096)));

/**
 * @brief Initializes the Paging Unit using 4MB Pages (PSE).
 * 
 * Strategy: Identity map the first 1GB of physical RAM using 256 x 4MB pages.
 * This covers the kernel, VGA framebuffer, BSS, heap, and MMIO regions.
 * PSE avoids nested page table faults since we don't need separate page tables.
 */
void sys_paging_init() {
    // 1. Enable PSE (Page Size Extension) in CR4
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x00000010;  // Set PSE bit (bit 4)
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));

    // 2. Setup Page Directory with 4MB pages
    // Identity map first 1GB (256 entries x 4MB = 1024MB)
    for(unsigned int i = 0; i < 256; i++) {
        // Attributes: Present (1) + Read/Write (2) + Page Size 4MB (0x80) = 0x83
        page_directory[i] = (i * 0x400000) | 0x83;
    }
    
    // Clear remaining entries
    for(unsigned int i = 256; i < 1024; i++) {
        page_directory[i] = 0x00000002;  // Not present, R/W
    }

    // 3. Load CR3 with the Page Directory
    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory));

    // 4. Enable Paging (PG bit in CR0)
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    k_vga_print("[HARDEN] Paging Unit Active (PSE 4MB, 1GB Identity Mapped).\n");
}
