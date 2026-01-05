
# Unions and Bitfields: Sub-Byte Precision

## 1. Beyond the Byte
While `layout` defines structure at the byte level, systems programming often requires control at the **Bit Level**. Hardware registers pack multiple flags into a single byte, and network protocols use non-standard integer widths (like 4-bit headers). Walia provides `union` and Bitfields to master this domain.

## 2. The `union` Keyword
A `union` allows you to store different data types in the same memory location. The size of the union is the size of its largest member.

### Use Case: Type Punning
Interpreting the same bits as different types without conversion overhead.

```javascript
union FloatBits {
    f: f32;
    u: u32;
}

var x: FloatBits;
x.f = 3.14;

// Inspect the raw IEEE 754 bit pattern
print x.u; 
```
This is essential for fast inverse square root algorithms or low-level serialization.

### Use Case: Hardware Registers
Modeling a register that can be accessed as a full byte or individual flags.
```javascript
union ControlReg {
    full_byte: u8;
    flags:     FlagsLayout; // A layout defining bitfields
}
```

## 3. Bitfields (The Colon Syntax)
Inside a `layout`, you can specify the exact bit-width of a field using a second colon.

```javascript
layout FlagsLayout {
    enable:    u8 : 1; // 1 bit
    interrupt: u8 : 1; // 1 bit
    mode:      u8 : 2; // 2 bits
    padding:   u8 : 4; // 4 bits (Total = 8 bits / 1 byte)
}
```

### Compiler Logic: The Bit-Masking JIT
When you access `flags.mode`, the Walia compiler generates highly optimized machine code:
1.  **Load:** Reads the full byte containing the field.
2.  **Mask:** Applies `AND` mask (`0b00001100`) to isolate the bits.
3.  **Shift:** Right-shifts to align the value (`>> 2`).

This happens transparently. To the developer, `flags.mode` behaves like a normal integer variable (0-3).

## 4. Atomic Bit Manipulation
Walia ensures that writing to a bitfield does not corrupt neighboring bits. The compiler emits a **Read-Modify-Write** sequence:
1.  Read the byte.
2.  Clear the target bits (AND with inverse mask).
3.  Set the new bits (OR with shifted value).
4.  Write the byte back.

## 5. Enterprise Application: The TCP Header
Combining Layouts, Unions, and Bitfields allows for a complete, zero-copy definition of the TCP Header.

```javascript
layout TCPHeader {
    src_port: u16;
    dst_port: u16;
    seq_num:  u32;
    ack_num:  u32;
    
    // Mixed byte: Data Offset (4 bits) + Reserved (3 bits) + NS flag (1 bit)
    flags_mix: u8; 
    
    // ... rest of header
}
```

## 6. Constraints and Safety
*   **Performance:** Bitfield access is slower than full-byte access due to the shifting/masking overhead. Use only when hardware or protocol strictness requires it.
*   **Concurrency:** Writing to two different bitfields in the same byte simultaneously from different threads is **Not Thread Safe** (Hardware limitation). Walia Sentry will warn about "Bitfield Race Conditions" in parallel code.

---
**Status:** Enterprise Specification 1.2  
**Feature:** Type Punning / Bit-Level Addressing  
**Precision:** 1-bit Granularity  
