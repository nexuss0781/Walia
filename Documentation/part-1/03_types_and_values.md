# Types and Value Representation

## 1. The 8-Byte Rule
At the architectural level, Walia enforces a "Zero-Waste" memory policy. Every variable in the language, regardless of its type, occupies exactly 8 bytes (64 bits). This uniformity allows the Virtual Machine to utilize a flat Register Array, maximizing CPU cache efficiency and minimizing memory fragmentation.

## 2. Value Representation (NaN-Boxing)
Walia utilizes an industry-standard technique called **NaN-Boxing** to pack multiple data types into a single 64-bit word. By exploiting the IEEE 754 floating-point standard's "Not-a-Number" (NaN) space, Walia distinguishes between different types without requiring a separate "type tag" byte.

### I. Numbers (Unboxed)
Any 64-bit word that is not a NaN is treated as a standard double-precision floating-point number.
*   **Precision:** 53 bits of mantissa.
*   **Range:** Standard IEEE 754.
*   **Memory:** Stored "unboxed" (directly in the register).

### II. Immediates (Unboxed)
Special bit patterns within the NaN space represent "Immediate" values. These do not require heap allocation:
*   **Boolean:** Represenhgy{represented as `true` or `false`.
*   **Nil:** Represents the absence of a value.
*   **Memory:** 0 bytes of heap usage.

### III. Objects (Heap-Resident)
Pointers to complex structures (Strings, Functions, Closures) are stored within the remaining NaN bits.
*   **Pointer Width:** 48-bit addressing (Standard for modern x64/ARM64).
*   **Memory:** Allocated on the Persistent Heap.

## 3. Core Data Types

### String
Strings in Walia are immutable, heap-allocated, and **interned**.
*   **Interning:** If the string `"user_name"` appears 1,000 times in your script, Walia stores it exactly once in the global string table.
*   **Equality:** String comparisons are reduced to O(1) pointer equality checks, significantly speeding up variable lookups and logic.

### Function
Functions are first-class citizens. They hold the compiled bytecode and metadata required for execution.
*   **Arity:** Functions have a fixed number of parameters, checked at the call site.
*   **Persistence:** Functions are stored in the `.state` file, allowing code to be shared across distributed nodes or reloaded instantly.

### Closure
A closure is a function combined with its captured lexical environment.
*   **Upvalues:** Closures reference "Upvalues" (variables from outer scopes).
*   **Resilience:** In Walia, closures are persistent. If a closure captures a variable and the process is restarted, the closure retains the captured data.

## 4. Gradual Structural Typing
While Walia is dynamically typed for developer flexibility, it incorporates a **Structural Type System** checked during the semantic analysis pass.

*   **Duck Typing:** Objects are compatible based on their "shape" (the fields and methods they possess) rather than explicit inheritance.
*   **Validation:** The compiler detects "Member Not Found" and "Invalid Call" errors before the bytecode is executed, providing the safety of a static language with the speed of a dynamic script.

---
**Status:** Enterprise Specification 1.0  
**Encoding:** IEEE 754 / NaN-Boxed
