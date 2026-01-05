/*
 * WALIA SOVEREIGN BOOTSTRAP (x86_64)
 * (c) 2025. Freestanding Kernel Entry.
 */

/* Multiboot2 Constants */
.set MAGIC,     0xE85250D6
.set ARCH,      0          /* x86 (32-bit protected mode start) */
.set LENGTH,    (multiboot_header_end - multiboot_header)
.set CHECKSUM,  -(MAGIC + ARCH + LENGTH)

.section .multiboot
.align 8
multiboot_header:
    /* Magic numbers */
    .long MAGIC
    .long ARCH
    .long LENGTH
    .long CHECKSUM

    /* End Tag (Type 0, Flags 0, Size 8) */
    .word 0
    .word 0
    .long 8
multiboot_header_end:

/*
 * Kernel Entry Point
 */
.section .text
.global _start
.type _start, @function
_start:
    /* 1. Disable Interrupts (Safety First) */
    cli

    /* 2. Setup the Stack Pointer */
    /* We use the reserved stack_space defined below */
    mov $stack_top, %esp

    /* 3. Handover to C Kernel */
    /* Note: In a real x64 boot, we'd setup Paging/Long Mode here. */
    /* For this Phase, we assume a 32-bit handoff which jumps to */
    /* 64-bit mode inside the kernel via assembly trampolines later. */
    call walia_kernel_main

    /* 4. Halt Loop (If kernel returns) */
    /* Should never happen, but safe to have. */
hang:
    hlt
    jmp hang

/*
 * Kernel Stack Space
 * Allocating 16KB for the initial kernel stack.
 */
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top: