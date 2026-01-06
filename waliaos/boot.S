# ==============================================================================
# WALIA SOVEREIGN BOOTSTRAP (x86_64)
# (c) 2025. Genesis Entry.
# ==============================================================================

# 1. MULTIBOOT2 HEADER (8-byte Aligned)
.section .multiboot
.align 8
header_start:
    .long 0xe85250d6                # Magic Number (Multiboot2)
    .long 0                         # Architecture (i386/32-bit protected mode start)
    .long header_end - header_start # Header length
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) # Checksum

    # End Tag
    .short 0
    .short 0
    .long 8
header_end:

# 2. EXECUTION ORGAN ENTRY
.section .text
.global _start
.type _start, @function

_start:
    # --- STEP 1: PHYSICAL ISOLATION ---
    cli                 # Physically disable all interrupts (Seize control)
    
    # --- STEP 2: STACK SOVEREIGNTY ---
    # Setup the initial kernel stack. 
    # We point ESP to the top of our reserved memory block.
    mov $stack_top, %esp

    # --- STEP 3: HANDOVER TO C-SUBSTRATE ---
    # We pass the Multiboot magic (EAX) and Info Structure (EBX) 
    # to the kernel initializer.
    push %ebx           # Info structure pointer
    push %eax           # Magic number
    
    # This is the jump from Assembly into our C Scaffold
    call walia_kernel_init

    # --- STEP 4: HARDWARE HALT ---
    # If the kernel ever returns, we freeze the CPU to prevent junk execution.
hang:
    hlt                 # Halt the CPU
    jmp hang            # Infinite safety loop

# 3. RESERVED BOOT STACK
# We allocate 16KB of aligned space for the initial execution context.
.section .bss
.align 16
stack_bottom:
    .skip 16384         # 16 KB
stack_top:
