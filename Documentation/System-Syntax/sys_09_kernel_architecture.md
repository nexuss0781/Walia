# Capstone Design: Sovereign Kernel Architecture

## 1. The "Sovereign Zero" Vision
The final proof of Walia's capabilities is the construction of a bootable kernel. This is not a Linux application; it is a standalone binary that boots directly on bare metal (x86_64) via GRUB/Multiboot.

This document outlines the architecture of **Sovereign Zero**, a micro-kernel built entirely using Walia's Systems Mode features.

## 2. Boot Sequence
1.  **Multiboot Header (`boot.wal`):** A `layout` structure containing the magic numbers required by the bootloader.
2.  **Entry Point (`kernel.wal`):** An `asm` block that sets up the stack pointer (`esp`) and disables interrupts (`cli`).
3.  **Walia Runtime Init:** A minimal C-stub (linked statically) that initializes the Walia VM in "Kernel Mode" (No OS dependencies).
4.  **Main Loop:** The VM begins executing the `kernel_main` function in `kernel.wal`.

## 3. Memory Map
The kernel manages physical memory manually using the **Sovereign Allocator**.

| Address Range | Usage |
| :--- | :--- |
| `0x000000` - `0x001000` | Real Mode IVT (Reserved) |
| `0x001000` - `0x100000` | Kernel Code & Static Data |
| `0x100000` - `0x200000` | Sovereign Heap (The .state image) |
| `0xB8000`  - `0xB8FA0`  | VGA Text Buffer (MMIO) |

## 4. The VGA Driver (Hello World)
The first goal is to print text to the screen without an OS.

```javascript
layout VGACell {
    char:  u8;
    color: u8;
}

fun kprint(msg: *u8) {
    var vga: *VGACell = 0xB8000;
    var i: i32 = 0;
    while (*(msg + i) != 0) {
        var cell = vga + i;
        cell.char = *(msg + i);
        cell.color = 0x0F; // White on Black
        i = i + 1;
    }
}
```

## 5. Interrupt Handling (IDT)
The kernel must respond to keyboard presses and timer ticks.
1.  **Layout:** Define the IDT (Interrupt Descriptor Table) structure using `layout`.
2.  **ASM:** Write the `lidt` instruction in an `asm` block to load the table.
3.  **Handlers:** Register Walia functions as interrupt handlers (requires specific stack frame management via `asm` wrappers).

## 6. Persistence in the Kernel
Sovereign Zero utilizes Walia's persistence to maintain system state.
*   **The Registry:** The kernel maintains a persistent `Map` of running processes.
*   **Crash Recovery:** If the kernel panics, it reboots and reloads the `.state` file, restoring the process list instantly.

## 7. The path to AI-OS
Once the kernel can manage memory and interrupts, we can load the **Vector Wing**.
*   **Neural Scheduler:** An OS scheduler that uses HNSW proximity to group related processes on the same CPU core.
*   **Semantic Filesystem:** A filesystem where files are retrieved by *meaning* (Vector Search) rather than by path.

---
**Status:** Architectural Blueprint  
**Target:** x86_64 Bare Metal  
**Bootloader:** Multiboot 1 / 2 Compatible  
