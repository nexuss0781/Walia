
# Metal Types and Sovereign Safety

## 1. The Atomic Foundation
In standard "Managed Mode," Walia utilizes **NaN-Boxing**, where every value is a 64-bit floating-point number carrying a payload. While efficient for scripting, this abstraction prevents precise control over hardware registers and memory layouts required for Operating System development.

**Phase 11** introduces **Metal Types** (or Systems Types). These are fixed-width primitives that map 1:1 to CPU registers and memory addresses. When you declare a `u8`, Walia allocates exactly 8 bits of memory, not 64.

## 2. The Metal Registry
The Sovereign Compiler supports the following hardware-aligned types:

| Type | Bit Width | Description | Hardware Mapping |
| :--- | :--- | :--- | :--- |
| **`u8`** | 8 | Unsigned Byte | `AL` / `w0` (masked) |
| **`u16`** | 16 | Unsigned Short | `AX` / `w0` (masked) |
| **`u32`** | 32 | Unsigned Int | `EAX` / `w0` |
| **`u64`** | 64 | Unsigned Long | `RAX` / `x0` |
| **`i8` - `i64`** | 8-64 | Signed Integers | Two's Complement |
| **`f32`** | 32 | Single Float | IEEE 754 Single |
| **`f64`** | 64 | Double Float | IEEE 754 Double |
| **`ptr`** | 64 | Virtual Address | Raw Pointer |
| **`void`** | 0 | Empty Set | No Storage |

## 3. Declaration Syntax
To enter Systems Mode for a variable, use the **Colon Annotation syntax**.

```javascript
// Managed Variable (NaN-Boxed Value)
var counter = 0; 

// Metal Variable (Raw 8-bit Register)
var port_id: u8 = 80;

// Metal Variable (Raw 64-bit Address)
var video_buffer: ptr = 0xB8000;
```

**Compiler Behavior:**
1.  **Resolution:** The compiler resolves the type name `u8` against the `sys_types.h` registry.
2.  **Allocation:** Instead of reserving a full 8-byte stack slot, the Register Allocator (Phase 11.1) attempts to **Pack** this variable (see *Module 02*).
3.  **Emission:** The compiler emits `OP_SYS_STORE_U8` instead of the generic `OP_MOVE`.

## 4. The Safety/Speed Tradeoff
Systems programming often requires "unsafe" behavior (like integer overflow for hashing algorithms). Walia handles this via build configurations.

### I. Debug Mode (Sovereign Safety)
When compiled without optimization flags, Walia enforces **Strict Bounds Checking**.
```javascript
var x: u8 = 255;
x = x + 1; // RUNTIME ERROR: u8 Overflow
```

### II. Release Mode (UFO Speed)
When compiled for production, Walia removes the bounds checks and maps operations directly to CPU instructions.
```javascript
var x: u8 = 255;
x = x + 1; // Wraps to 0 (Standard Hardware Behavior)
```
This ensures that during development, logic errors are caught, but in production, the code runs at the raw speed of silicon.

## 5. Explicit Casting (`as` vs `!as`)
Moving between types requires explicit intent.

### Safe Cast (`as`)
Performs a checked conversion.
```javascript
var big: i32 = 500;
var small: u8 = big as u8; // Error: Value 500 does not fit in u8
```

### Unsafe Cast (`!as`)
Performs a raw bit-truncation or reinterpretation.
```javascript
var big: i32 = 500; // Binary: 00000001 11110100
var small: u8 = big !as u8; // Result: 244 (11110100) - Top bits truncated
```
The `!as` operator effectively tells the compiler: *"I know what I am doing; just move the bits."*

## 6. Enterprise Utility
Metal Types are the prerequisite for **Foreign Function Interface (FFI)** and **Driver Development**. By guaranteeing specific bit-widths, Walia can define structures that overlay perfectly onto network packets (TCP/IP headers) or hardware control blocks (VGA/GPIO), eliminating the need for serialization.

---
**Status:** Enterprise Specification 1.2  
**Feature:** Fixed-Width Primitives  
**Integrity:** Conditional Overflow Safety  
