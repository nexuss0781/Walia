/*
 * WALIA SOVEREIGN OS LINKER SCRIPT (v3.0.0)
 * Target: x86_64 Bare Metal
 */

ENTRY(_start)

SECTIONS
{
    /* 
     * The OS is loaded at 1 MiB (0x100000).
     * This preserves the lower memory for BIOS/VGA legacy structures.
     */
    . = 1M;

    /* 1. EXECUTION LOGIC (Code) */
    .text : ALIGN(4K)
    {
        _kernel_start = .;
        /* Multiboot2 Header MUST be at the very beginning of the binary */
        KEEP(*(.multiboot))
        *(.text)
    }

    /* 2. CONSTANT LOGIC (Read-Only Data) */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    /* 3. PERSISTENT STATE DATA (Initialized Data) */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* 4. VOLATILE WORKING MEMORY (Uninitialized Data & Stack) */
    .bss : ALIGN(4K)
    {
        *(.COMMON)
        *(.bss)
    }

    /* 
     * THE SOVEREIGN BOUNDARY 
     * This symbol marks the end of the physical OS image.
     * The Walia Pager uses this to begin the Slotted Heap (The Ocean).
     */
    . = ALIGN(4K);
    _kernel_end = .;
}
