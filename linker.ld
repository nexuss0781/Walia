/*
 * WALIA SOVEREIGN LINKER SCRIPT
 * Target: x86_64 Freestanding Kernel
 */

ENTRY(_start)

SECTIONS
{
    /* The kernel starts at 1MB in physical RAM */
    . = 1M;

    /* 1. Multiboot Header & Code */
    .text :
    {
        /* Ensure the Multiboot header comes first so GRUB finds it */
        *(.multiboot)
        *(.text)
    }

    /* 2. Read-Only Data */
    .rodata :
    {
        *(.rodata)
    }

    /* 3. Read-Write Data (Initialized) */
    .data :
    {
        *(.data)
    }

    /* 4. Uninitialized Data (Stack, Heaps) */
    .bss :
    {
        *(.COMMON)
        *(.bss)
    }

    /* 
     * Kernel End Marker
     * This variable is used by the Sovereign Allocator (Phase 11.5)
     * to know where "Free RAM" begins.
     */
    _kernel_end = .;
}