# Capstone Implementation: The VGA Driver

## 1. Memory-Mapped I/O (MMIO)
In x86 architecture, the video card's text buffer is mapped to physical address `0xB8000`. By writing directly to this address, we can display characters on the screen without any operating system drivers.

This demonstrates Walia's ability to handle **Absolute Pointers**.

## 2. Defining the Hardware
The VGA buffer consists of an array of 2-byte cells.

```javascript
// vga.wal
packed layout VGACell {
    char:  u8; // The ASCII character
    color: u8; // Foreground and Background color
}

// Standard VGA Dimensions
var VGA_WIDTH:  i32 = 80;
var VGA_HEIGHT: i32 = 25;
var VGA_MEMORY: ptr = 0xB8000;
```

## 3. The Driver Logic
We implement a simple driver that tracks the cursor position and handles scrolling.

```javascript
// State variables (Stored in Sovereign Heap)
var cursor_x: i32 = 0;
var cursor_y: i32 = 0;

fun vga_put_entry(c: u8, color: u8, x: i32, y: i32) {
    var index: i32 = y * VGA_WIDTH + x;
    var cell_ptr: *VGACell = VGA_MEMORY + index; // Pointer Arithmetic
    
    cell_ptr.char = c;
    cell_ptr.color = color;
}

fun vga_scroll() {
    // Memcpy lines up
    var dest: *u8 = VGA_MEMORY;
    var src:  *u8 = VGA_MEMORY + (VGA_WIDTH * 2);
    var size: i32 = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
    
    // Manual memory copy loop (or use future memcpy intrinsic)
    var i: i32 = 0;
    while (i < size) {
        *(dest + i) = *(src + i);
        i = i + 1;
    }
    
    // Clear last line
    // ...
}
```

## 4. The `kprint` Function
This is the kernel's `printf`. It iterates over a string and calls `vga_put_entry`.

```javascript
fun kprint(str: *u8) {
    var i: i32 = 0;
    while (*(str + i) != 0) {
        var c = *(str + i);
        
        if (c == '\n') {
            cursor_x = 0;
            cursor_y = cursor_y + 1;
        } else {
            vga_put_entry(c, 0x0F, cursor_x, cursor_y);
            cursor_x = cursor_x + 1;
            
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y = cursor_y + 1;
            }
        }
        
        if (cursor_y >= VGA_HEIGHT) {
            vga_scroll();
            cursor_y = VGA_HEIGHT - 1;
        }
        
        i = i + 1;
    }
}
```

## 5. Integrating Assembly
To move the hardware cursor (the blinking underscore), we must talk to the VGA ports `0x3D4` and `0x3D5` using `OUT` instructions. Walia handles this via `asm` blocks.

```javascript
fun vga_update_cursor(x: i32, y: i32) {
    var pos: u16 = (y * VGA_WIDTH) + x;
    
    asm {
        // Send High Byte
        "mov dx, 0x3D4"
        "mov al, 0x0E"
        "out dx, al"
        "mov dx, 0x3D5"
        // (Load high byte of pos into al logic needed here)
        "out dx, al"
        
        // Send Low Byte
        "mov dx, 0x3D4"
        "mov al, 0x0F"
        "out dx, al"
        "mov dx, 0x3D5"
        // (Load low byte of pos into al)
        "out dx, al"
    }
}
```

## 6. Verification
When `Sovereign Zero` boots, it calls:
```javascript
kprint("Walia Kernel v1.0\n");
kprint("Memory: Sovereign.\n");
```
If you see white text on a black screen, Walia has successfully conquered the bare metal.

---
**Status:** Capstone Implementation  
**Hardware:** VGA 80x25 Text Mode  
**Address:** 0xB8000  
