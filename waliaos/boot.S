# ==============================================================================
# WALIA SOVEREIGN BOOTSTRAP (x86_64)
# (c) 2025. Genesis Entry.
# ==============================================================================

# 0. MULTIBOOT1 HEADER (Legacy Compatibility)
.section .multiboot_legacy
.align 4
    .long 0x1BADB002               # Magic
    .long 0x00000003               # Flags (Align + MemInfo)
    .long -(0x1BADB002 + 0x00000003) # Checksum

# 1. MULTIBOOT2 HEADER (8-byte Aligned)
.section .multiboot
.align 8
header_start:
    .long 0xe85250d6                # Magic Number (Multiboot2)
    .long 0                         # Architecture (i386/32-bit protected mode start)
    .long header_end - header_start # Header length
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) # Checksum

    # Tag: Information Request
    .short 1
    .short 0
    .long 12
    .long 1                         # Console info
    .long 2                         # Boot command line
    
    # Tag: Entry Point (Explicit)
    .short 3
    .short 0
    .long 12
    .long _start

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
    
    # --- STEP 2: SEGMENT SOVEREIGNTY ---
    # Load our minimal GDT to ensure segment stability.
    lgdt gdt_descriptor
    
    # Perform a far jump to reload CS (Code Segment)
    ljmp $0x08, $.reload_cs

.reload_cs:
    # Set up data segments
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # --- STEP 3: STACK SOVEREIGNTY ---
    mov $stack_top, %esp

    # --- STEP 4: HANDOVER TO C-SUBSTRATE ---
    push %ebx           # Info structure pointer
    push %eax           # Magic number
    
    call walia_kernel_init

    # --- STEP 5: HARDWARE HALT ---
hang:
    hlt                 # Halt the CPU
    jmp hang            # Infinite safety loop

# --- ASSEMBLY HELPERS ---
.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax   # Get the pointer to the GDT, passed as a parameter.
    lgdt (%eax)         # Load the new GDT pointer

    mov $0x10, %ax      # 0x10 is the offset in the GDT to our data segment
    mov %ax, %ds        # Load all data segment selectors
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ljmp $0x08, $.flush # 0x08 is the offset to our code segment: Far jump!
.flush:
    ret

.global idt_flush
idt_flush:
    mov 4(%esp), %eax
    lidt (%eax)
    ret

# --- EXCEPTION HANDLING ---
.extern sys_exception_handler

# Macro for ISRs that don't push an error code
.macro ISR_NOERR index
.global isr\index
isr\index:
    push $0             # Dummy error code
    push $\index        # Interrupt number
    jmp isr_common_stub
.endm

# Macro for ISRs that DO push an error code
.macro ISR_ERR index
.global isr\index
isr\index:
    # Error code is already on the stack
    push $\index        # Interrupt number
    jmp isr_common_stub
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

.global static_isr_table
static_isr_table:
    .long isr0
    .long isr1
    .long isr2
    .long isr3
    .long isr4
    .long isr5
    .long isr6
    .long isr7
    .long isr8
    .long isr9
    .long isr10
    .long isr11
    .long isr12
    .long isr13
    .long isr14
    .long isr15
    .long isr16
    .long isr17
    .long isr18
    .long isr19
    .long isr20
    .long isr21
    .long isr22
    .long isr23
    .long isr24
    .long isr25
    .long isr26
    .long isr27
    .long isr28
    .long isr29
    .long isr30
    .long isr31

isr_common_stub:
    pusha               # Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    mov %ds, %ax        # Lower 16 bits of eax = ds
    push %eax           # Save the data segment descriptor
    
    mov $0x10, %ax      # Load kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    call sys_exception_handler
    
    pop %eax            # Reload the original data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    popa                # Pops edi, esi, ebp...
    add $8, %esp        # Cleans up the pushed error code and pushed ISR number
    iret                # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

# 3. MINIMAL SOVEREIGN GDT
.align 8
gdt_start:
    .quad 0x0000000000000000    # Null descriptor
    .quad 0x00cf9a000000ffff    # Code segment (0x08): base 0, limit 4GB, Type: Code/Execute/Read
    .quad 0x00cf92000000ffff    # Data segment (0x10): base 0, limit 4GB, Type: Data/Read/Write
gdt_end:

gdt_descriptor:
    .short gdt_end - gdt_start - 1
    .long gdt_start

# 4. RESERVED BOOT STACK
# We allocate 16KB of aligned space for the initial execution context.
.section .bss
.align 16
stack_bottom:
    .skip 16384         # 16 KB
stack_top:
