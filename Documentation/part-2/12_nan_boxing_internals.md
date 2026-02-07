# NaN-Boxing: Value Representation Internals

## 1. The Uniform 64-Bit Value
In the Walia runtime, memory efficiency is achieved by ensuring that every variable, parameter, and register entry occupies exactly **64 bits (8 bytes)**. To represent multiple distinct types (Numbers, Booleans, Nil, and Pointers) within this fixed width without the overhead of an external "type tag" byte, Walia utilizes **NaN-Boxing**.

## 2. IEEE 754 Double-Precision Format
A standard 64-bit floating-point number (double) is structured as follows:
*   **Sign Bit:** 1 bit
*   **Exponent:** 11 bits
*   **Mantissa (Fraction):** 52 bits

According to the IEEE 754 standard, if all **11 exponent bits are set to 1**, the value is considered **NaN** (Not-a-Number). If the highest bit of the mantissa (the "quiet bit") is also set, it is a **Quiet NaN (QNAN)**. This leaves 51 bits of the mantissa entirely ignored by hardware floating-point units.

Walia "boxes" its non-numeric data into these 51 ignored bits.

## 3. The Walia Bitmask Scheme

### I. Floating-Point Numbers
If any of the 11 exponent bits are **0**, the value is a standard double. Walia performs no transformation on numbers; they are processed by the CPU's FPU at native speeds.

### II. The QNAN Prefix
For all non-numeric types, Walia applies a specific 64-bit prefix:
`0x7ffc000000000000` (The QNAN mask).

Any value where `(value & QNAN_MASK) == QNAN_MASK` is identified by the VM as a boxed type.

### III. Immediate Values (Immediates)
Immediates are small values stored directly in the 64-bit word. They use specific tags in the lowest bits of the NaN space.

| Type | Bit Pattern (Hex) | Description |
| :--- | :--- | :--- |
| **Nil** | `0x7ffc000000000001` | The Walia `nil` constant. |
| **False** | `0x7ffc000000000002` | The boolean `false`. |
| **True** | `0x7ffc000000000003` | The boolean `true`. |

### IV. Pointers (Heap Objects)
Walia exploits the fact that modern 64-bit processors only use the lower **48 bits** for memory addressing. By setting the **Sign Bit** (the 63rd bit) to 1 in addition to the QNAN prefix, Walia creates a unique space for pointers.

*   **Pointer Format:** `0xffff000000000000 | <48-bit address>`
*   **Type Determination:** If the QNAN bits are set AND the sign bit is set, the value is an `Obj*` pointer (String, Closure, Function).

## 4. Hardware and Performance Benefits
NaN-Boxing provides several industrial-grade advantages over traditional "Tagged Unions" or C-structs:

1.  **Memory Compression:** A traditional `Value` struct in C usually requires 16 bytes due to padding (8 for the union + 8 for the tag enum). NaN-Boxing reduces this by **50%**, allowing twice as much data to fit in the CPU cache.
2.  **Unboxed Arithmetic:** Mathematical operations do not require checking a tag or dereferencing a pointer. The VM simply treats the 64-bit word as a double.
3.  **Bitwise Dispatch:** Type checking is reduced to simple bitwise `AND` and `OR` operations, which are the fastest instructions a CPU can execute.

## 5. Pointer Tagging Safety
Walia's pointer representation ensures that a memory address can never be mistaken for a valid number. Because valid floating-point numbers never have the specific "Double NaN" bit pattern used by Walia pointers, the VM can safely distinguish between a literal `1.23` and a pointer to the string `"1.23"` in a single CPU cycle.

---
**Status:** Technical Specification 1.0  
**Logic:** IEEE 754 Payload Injection  
**Word Size:** 64-bit Optimized
