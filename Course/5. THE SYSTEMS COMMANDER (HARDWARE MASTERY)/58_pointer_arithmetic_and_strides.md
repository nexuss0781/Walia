
# Tier V: The Systems Commander
## Module 58: Pointer Arithmetic and Strides

### 1. Navigating the Physical Web
A pointer is a coordinate. To become a master of the machine, you must be able to navigate through blocks of memory mathematically. If you have a buffer containing 100 integers, you don't need 100 variables; you need one pointer that can "walk" from the first integer to the last.

This module introduces **Pointer Arithmetic**—the ability to add or subtract from an address to move it through the system's memory.

### 2. The Concept of "Stride"
In Walia, adding 1 to a pointer doesn't just increment the memory address by one byte. Instead, it moves the pointer by the **Size of the underlying type**. This is called the **Stride**.

*   If `p` points to a **`u8`** (1 byte), `p + 1` moves **1 byte**.
*   If `p` points to a **`u32`** (4 bytes), `p + 1` moves **4 bytes**.
*   If `p` points to a **`ptr`** (8 bytes), `p + 1` moves **8 bytes**.

**The Benefit:** You can iterate through lists of data without manually calculating the byte-offsets. The compiler handles the "Physical Math" for you.

```Walia
unsafe {
    // 1. Allocate space for 4 integers (16 bytes total)
    var buffer: ptr = alloc(16);
    
    // 2. Start at the first item
    var p = buffer;
    *p = 100;
    
    // 3. Move forward by 1 'Stride' (4 bytes)
    var next_p = p + 1;
    *next_p = 200;
    
    print *p;      // 100
    print *next_p; // 200
}
```

### 3. Basic Arithmetic Operations
| Action | Syntax | Result |
| :--- | :--- | :--- |
| **Move Forward** | `p + n` | Jumps `n` elements forward. |
| **Move Backward** | `p - n` | Jumps `n` elements backward. |
| **Distance** | `p1 - p2` | Returns the number of elements between two addresses. |

### 4. Pointer Subscripting (The `[]` Pattern)
Walia provides a concise way to perform pointer arithmetic and dereferencing in one step using the square bracket `[]` syntax, identical to arrays in other languages.

```Walia
unsafe {
    var buffer: ptr = alloc(100 * sizeof(u32));
    
    // These two lines are identical:
    *(buffer + 5) = 99; // Manual arithmetic
    buffer[5] = 99;     // Subscript syntax (Cleaner)
}
```

### 5. Type Punning: The Byte-Step
Sometimes, you need to ignore the "Stride" and move exactly one byte at a time (e.g., when parsing a raw binary stream). To do this, you cast your pointer to a **Byte Pointer (`*u8`)**.

```Walia
unsafe {
    var p: *u32 = 0x1000;
    
    // Moves 4 bytes (Stride of u32)
    var next_int = p + 1; 
    
    // Moves exactly 1 byte (Stride of u8)
    var next_byte = (p !as *u8) + 1;
}
```

### 6. The Danger: Out-of-Bounds Arithmetic
Pointer arithmetic is powerful but dangerous. 
*   **The Hazard:** If you add 1,000 to a pointer that only has a 10-byte buffer, the address will point to memory you do not own.
*   **The Sentry:** In **Debug Mode**, Walia’s **Sovereign Guard** tracks the "Allocation Bounds" of every pointer. If you try to dereference a pointer that has "Walked off the cliff," the engine will halt the program with a `MEMORY_BOUNDARY_VIOLATION`. 
*   **Release Performance:** In production, these checks are removed, ensuring your memory traversal runs at raw CPU speeds.

### 7. Practical Example: The High-Speed Sum
Let's use pointer arithmetic to sum a block of persistent memory.

```Walia
fun sum_buffer(buffer_ptr: ptr, count: i32) {
    var total = 0;
    var i = 0;
    
    unsafe {
        while (i < count) {
            // Read the current item and add to total
            total = total + buffer_ptr[i];
            i = i + 1;
        }
    }
    return total;
}
```

### 8. Summary
1.  **Pointer Arithmetic:** Moving an address mathematically through memory.
2.  **The Stride:** The distance moved is automatically determined by the data type size.
3.  **Subscripting:** `p[n]` is high-level sugar for `*(p + n)`.
4.  **Byte-Level Control:** Cast to `*u8` to perform raw byte-by-byte navigation.

---
**Next Step:** Navigating memory is the foundation. Now we need to **Organize** it. In the next module, we enter the world of **Physical Data Modeling**, learning to use the **`layout`** keyword to define complex structures that map perfectly to the computer's memory.
