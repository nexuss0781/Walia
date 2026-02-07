
# Sovereign Zero: The Source Code

This file contains the complete Walia source code for the **Sovereign Zero** Micro-Kernel. These files are intended to be compiled with the Walia Systems Compiler (`--target=bare-metal`) and linked into a bootable ISO.

---

## 1. `boot.wal` (Multiboot Header)
The entry point recognized by the GRUB bootloader. It defines the magic numbers that prove this binary is a kernel.

```javascript
// Multiboot 1 Header Constants
var MB_MAGIC: u32 = 0x1BADB002;
var MB_FLAGS: u32 = 0x00; // No special flags
var MB_CHECKSUM: u32 = -(MB_MAGIC + MB_FLAGS);

layout MultibootHeader {
    magic:    u32;
    flags:    u32;
    checksum: u32;
}

// The physical header at the start of the binary
var header: MultibootHeader;
header.magic = MB_MAGIC;
header.flags = MB_FLAGS;
header.checksum = MB_CHECKSUM;
```

---

## 2. `vga.wal` (Video Driver)
A minimal driver to print text to the screen.

```javascript
var VGA_MEM: ptr = 0xB8000;
var VGA_WIDTH: i32 = 80;
var VGA_HEIGHT: i32 = 25;

var term_row: i32 = 0;
var term_col: i32 = 0;
var term_color: u8 = 0x0F; // White on Black

fun term_init() {
    term_row = 0;
    term_col = 0;
    var i: i32 = 0;
    while (i < VGA_WIDTH * VGA_HEIGHT * 2) {
        *(VGA_MEM + i) = 0; // Clear screen
        i = i + 2;
    }
}

fun term_putc(c: u8) {
    if (c == '\n') {
        term_col = 0;
        term_row = term_row + 1;
        return;
    }
    
    var index: i32 = (term_row * VGA_WIDTH + term_col) * 2;
    *(VGA_MEM + index) = c;
    *(VGA_MEM + index + 1) = term_color;
    
    term_col = term_col + 1;
    if (term_col >= VGA_WIDTH) {
        term_col = 0;
        term_row = term_row + 1;
    }
}

fun kprint(str: *u8) {
    var i: i32 = 0;
    while (*(str + i) != 0) {
        term_putc(*(str + i));
        i = i + 1;
    }
}
```

---

## 3. `kernel.wal` (Main Entry)
The kernel logic. It initializes the hardware and prints a welcome message.

```javascript
import "vga.wal";

// Kernel Entry Point (Called by bootloader)
fun kernel_main() {
    term_init();
    
    kprint("Welcome to Sovereign Zero.\n");
    kprint("--------------------------\n");
    kprint("Architecture: x86_64\n");
    kprint("Language:     Walia Systems Mode\n");
    
    // Memory Probe
    var heap_start: ptr = 0x100000;
    *heap_start = 0xDEADBEEF;
    
    if (*heap_start == 0xDEADBEEF) {
        kprint("Memory Check: PASSED.\n");
    } else {
        kprint("Memory Check: FAILED.\n");
    }
    
    // Halt the CPU
    while (true) {
        asm { "hlt" }
    }
}
```

---

## 4. `linker.ld` (Linker Script)
Tells the linker how to arrange the binary sections (Code vs Data).

```ld
ENTRY(start)

SECTIONS {
    . = 1M; /* Load kernel at 1MB */

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
```

---

## Build Instructions
To build Sovereign Zero, you need the Walia Systems Compiler and a cross-compiler (gcc or ld).

```bash
# 1. Compile Walia sources to Object Files
walia -c boot.wal -o boot.o --target=freestanding
walia -c vga.wal -o vga.o --target=freestanding
walia -c kernel.wal -o kernel.o --target=freestanding

# 2. Link into a Kernel Binary
ld -T linker.ld -o sovereign.bin boot.o vga.o kernel.o

# 3. Test with QEMU
qemu-system-x86_64 -kernel sovereign.bin
```

---
**Status:** Capstone Complete  
**Outcome:** A bootable operating system written in Walia.