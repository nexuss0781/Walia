
# Tier V: The Systems Commander
## Module 60: Alignment and Padding Rules

### 1. The Picky CPU
In the previous module, we learned how to group data using the `layout` keyword. However, the CPU does not see memory as a simple continuous stream of bytes. To move data at "UFO-grade" speeds, the CPU prefers to access memory on specific boundaries. 

If a 4-byte integer (`u32`) starts at an odd address, the CPU may have to perform two separate memory fetches instead of one. In some high-performance processors (like ARM), trying to read an unaligned variable will cause the system to crash. Walia manages this physical complexity through **Alignment and Padding Rules**.

### 2. The Power-of-Two Rule
Every hardware type in Walia has a preferred **Alignment Boundary**, which is typically equal to its size:

| Type | Size | Required Alignment (Multiple of...) |
| :--- | :--- | :--- |
| `u8` | 1 byte | 1 (Anywhere) |
| `u16` | 2 bytes | 2 |
| `u32` | 4 bytes | 4 |
| `ptr` | 8 bytes | 8 |

**The Sovereign Rule:** By default, Walia automatically inserts "invisible" bytes—called **Padding**—to ensure every field in your `layout` starts on its required boundary.

### 3. Visualizing Padding
Consider this layout. At first glance, it looks like it should take 5 bytes.

```Walia
layout SensorData {
    var id:     u8;   // 1 byte
    var value:  u32;  // 4 bytes
}
```

**Physical Reality:**
1.  `id` starts at Offset 0.
2.  `value` requires 4-byte alignment. It cannot start at Offset 1.
3.  Walia inserts 3 bytes of **Padding** (empty space).
4.  `value` starts at Offset 4.

**Total Size:** 8 bytes.

### 4. The `packed` Modifier
There are times when you must ignore the CPU’s preference and follow a strict external protocol (e.g., a network packet that has no padding). For this, Walia provides the `packed` modifier.

```Walia
packed layout DensePacket {
    var id:     u8;   // Offset 0
    var value:  u32;  // Offset 1 (Exactly follows 'id')
}
```

**Total Size:** Exactly 5 bytes.
**Commander's Warning:** Use `packed` only when necessary for compatibility. It is physically slower for the CPU to process than the default aligned layout.

### 5. Tail Padding
To ensure that arrays of layouts are safe to use, Walia applies padding to the **End** of the structure. The total size of a `layout` is always a multiple of its largest member's alignment.

```Walia
layout Example {
    var a: u64; // Alignment: 8
    var b: u8;  // Alignment: 1
}
```
**Total Size:** 16 bytes. Walia adds 7 bytes of padding after `b` so that if you have a list of `Example` objects, every single one starts on an 8-byte boundary.

### 6. Optimizing for Density
A senior architect organizes fields from **largest to smallest** to minimize wasted space from padding.

**Inefficient (Many gaps):**
```Walia
layout Messy {
    var a: u8;
    // 7 bytes padding
    var b: u64;
    var c: u8;
    // 7 bytes padding
} // Total: 24 bytes
```

**Sovereign Efficient (Zero gaps):**
```Walia
layout Clean {
    var b: u64;
    var a: u8;
    var c: u8;
    // 6 bytes tail padding
} // Total: 16 bytes
```

### 7. Practical: Measuring the Gaps
Let's use our introspection tools to see the physical impact of alignment.

1.  **Define a mixed layout:**
    ```Walia
    layout AlignmentTest {
        var a: u8;
        var b: u32;
    }
    ```
2.  **Verify the size:**
    ```Walia
    unsafe {
        print sizeof(AlignmentTest); // Output: 8
    }
    ```
3.  **Define a packed version:**
    ```Walia
    packed layout PackedTest {
        var a: u8;
        var b: u32;
    }
    ```
4.  **Verify the size:**
    ```Walia
    unsafe {
        print sizeof(PackedTest); // Output: 5
    }
    ```

### 8. Summary
1.  **Alignment:** The physical address requirement for hardware types.
2.  **Padding:** Empty bytes added by the compiler to satisfy alignment.
3.  **Performance:** Aligned data is faster for the CPU to read and write.
4.  **`packed`**: Overrides alignment for binary protocol compatibility.
5.  **Ordering Matters:** Sorting fields by size reduces the persistent footprint of your data.

---
**Next Step:** Structure and alignment are the "Solid" parts of memory. Now we need the "Liquid." In the next module, we will explore the **Union Keyword and Type Punning**, learning how to make different variables share the exact same physical space to view the same bits through multiple lenses.
