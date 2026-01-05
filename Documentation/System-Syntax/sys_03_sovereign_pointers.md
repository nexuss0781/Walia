# Memory Sovereignty: The Pointer Guide

## 1. The Physical Reality
In Managed Walia, you work with "References"—opaque handles to objects managed by the GC. In Systems Walia, you work with **Pointers**. A pointer is the raw, unvarnished integer address of a byte in the system's virtual memory.

Mastering pointers is the key to breaking out of the sandbox. It allows you to write to video memory, parse binary files in-place, and communicate with the OS kernel.

## 2. The `ptr` Type
The `ptr` type is a 64-bit unsigned integer that the compiler treats semantically as a memory address.
*   **Storage:** 8 bytes.
*   **Format:** Standard Virtual Address (e.g., `0x7ffee000`).
*   **Null:** Unlike managed `nil`, a null pointer is explicitly `0`.

## 3. The Operators

### Address-Of (`&`)
Gets the physical location of a variable in the VM stack.
```javascript
var x: u32 = 42;
var p: ptr = &x; // p now holds the address of x
```
**Safety Note:** Stack variables are transient. Never return the address of a local variable from a function, as the stack frame will be destroyed (reused) upon return.

### Dereference (`*`)
Reads or writes to the address stored in the pointer.
```javascript
// Reading
var val = *p; 

// Writing
*p = 100; // x is now 100
```
**Type Awareness:** The compiler knows that `p` points to a `u32`. Therefore, `*p = 100` writes exactly 4 bytes.

## 4. Pointer Arithmetic (The Stride)
In Walia, adding to a pointer doesn't just increment the address by 1; it increments by the **Stride** of the underlying type.

```javascript
var list: ptr; // Points to an array of u32 (4 bytes each)
var second_item = list + 1; // Actually adds 4 bytes to the address
```

### Void Pointers
If you need raw byte-level arithmetic regardless of type, cast to `*u8` (byte pointer).
```javascript
var raw_ptr = list !as *u8;
var byte_offset = raw_ptr + 1; // Adds exactly 1 byte
```

## 5. Memory Safety Sentry (Debug Mode)
Pointers are powerful but dangerous. A "Wild Pointer" can corrupt the VM state or crash the process. To mitigate this, Walia implements a **Debug Sentry**.

*   **Bounds Tracking:** In Debug builds, a pointer variable carries hidden metadata about its valid range (Start/End).
*   **Violation:** If you try to dereference `ptr + 100` but the allocation was only 50 bytes, the Sentry halts execution with a `SEGFAULT_GUARD` error, preventing the physical crash.
*   **Zero-Cost Release:** In Release builds, this metadata is stripped. Pointer operations compile down to raw CPU `MOV` instructions for maximum speed.

## 6. Enterprise Use Case: Zero-Copy Buffer
Pointers allow you to parse data without copying it.

```javascript
// Function receiving a raw packet buffer
fun parse_packet(buffer: ptr) {
    // Cast raw memory to structured layout pointers
    var header = buffer !as *IPHeader;
    
    // Read directly from the buffer memory
    if (header.version == 4) {
        process_ipv4(header);
    }
}
```
In this example, no new objects are created. The `header` variable is just a "Lens" over the existing raw bytes of the `buffer`, achieving the **Zero-Copy** requirement for high-performance networking.

---
**Status:** Enterprise Specification 1.2  
**Operators:** `&` (Address), `*` (Dereference)  
**Safety:** Sentry-Guarded (Debug) / Raw Metal (Release)  
