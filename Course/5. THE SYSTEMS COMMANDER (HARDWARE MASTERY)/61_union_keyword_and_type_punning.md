
# Tier V: The Systems Commander
## Module 61: The `union` Keyword and Type Punning

### 1. Overlapping Realities
In standard data structures (like `class` or `layout`), every variable has its own unique space in memory. If you have two `u32` variables, you use 8 bytes. However, high-performance systems often require multiple variables to share the **exact same physical memory address**.

Walia provides the **`union`** keyword for this purpose. A union allows you to view the same 64-bit word through different "Logical Lenses." It is the primary tool for **Type Punning**—a technique used to inspect the raw bit-patterns of data without performing expensive conversions.

### 2. Defining a `union`
A union looks identical to a layout but behaves fundamentally differently. Instead of placing fields side-by-side, it "stacks" them at **Offset 0**.

```Walia
// Example 1: A basic memory union
union DataView {
    var as_float:  f64; // 8 bytes
    var as_bits:   u64; // 8 bytes (shares memory with as_float)
}
```

**The Law of the Largest:** The physical size of a `union` is equal to the size of its **largest** member. In the example above, `DataView` is exactly 8 bytes total.

### 3. Concept: Type Punning
Type punning is the act of writing data as one type and reading it as another. This is essential for low-level math optimizations (like the famous fast inverse square root) or for inspecting the internal structure of a floating-point number.

```Walia
unsafe {
    var view: DataView;
    
    // Write as a decimal number
    view.as_float = 3.14159;
    
    // Read the raw binary bit-pattern as an integer
    print view.as_bits; 
    // Output: 4614256656552045848 (The binary representation of Pi)
}
```

### 4. Use Case: Multi-Resolution Identifiers
Unions allow you to treat a large identifier as a single unit or as smaller sub-components without any data movement.

```Walia
union Identity {
    var full_id: u32; // 4 bytes
    
    layout Split {
        var group_code: u16; // 2 bytes
        var user_code:  u16; // 2 bytes
    } parts;
}

unsafe {
    var id: Identity;
    id.full_id = 0xABCD1234;
    
    // Access the sub-parts directly
    print id.parts.group_code; // 0xABCD
    print id.parts.user_code;  // 0x1234
}
```

### 5. Unions and Persistence
Because a union is a physical memory map, it is perfectly compatible with Walia’s **Orthogonal Persistence**. If you store a `union` in a persistent variable, the shared memory segment is saved to the `.state` image. 

*   **Integrity:** When the system reboots, the "multi-view" nature of the memory is preserved. You can write to one side of the union on Monday and read from the other side on Tuesday.

### 6. Safety and the Commander’s Responsibility
Unions are the most "Liquid" part of the Walia substrate. 
*   **The Risk:** The compiler cannot track which member of the union was written to last. If you write to a 4-byte `u32` and then try to read an 8-byte `f64` from the same union, the top 4 bytes will contain "Memory Trash" from whatever was there before.
*   **The Rule:** You must manually track which view of the union is currently "Active."

### 7. Practical: The Neural Bit-Scanner
Let's build a utility that allows us to inspect the individual bytes of a high-precision number.

1.  **Define the Union:**
    ```Walia
    union Inspector {
        var value: f64;
        var bytes: u8[8]; // Array of 8 bytes (Conceptual array syntax)
    }
    ```
2.  **Inspect the Memory:**
    ```Walia
    unsafe {
        var ins: Inspector;
        ins.value = 123.456;
        
        // Print the first physical byte of the float
        print ins.bytes[0];
    }
    ```

### 8. Summary
1.  **`union`**: Different variables sharing the same physical memory.
2.  **Size**: Determined by the largest member.
3.  **Type Punning**: Viewing bits as different types for optimization or inspection.
4.  **Zero Overhead**: Switching between views of a union costs zero CPU cycles.

---
**Next Step:** We have mastered shaping bytes and overlapping memory. Now we need to go even smaller. In the next module, we will explore **High-Precision Bitfields**, learning how to address and manipulate individual bits within a single byte for absolute hardware control.
