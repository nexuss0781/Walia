# Register Window Dynamics: The Sliding Stack

## 1. The Virtual Register File
Walia’s performance is rooted in its treatment of memory as a contiguous **Virtual Register File**. While physical CPUs have a fixed number of hardware registers (e.g., 16 or 32), the Walia Virtual Machine (WVM) provides each function invocation with its own private set of up to **256 virtual registers** (R0–R255).

These registers are not distinct memory allocations; they are "windows" or "views" into a single, massive, pre-allocated array known as the **VM Stack**.

## 2. Call Frame Architecture
Every time a function is called, the WVM creates a `CallFrame`. This structure acts as a metadata header for the function's execution environment.

```c
typedef struct {
    ObjClosure* closure; // The function code and captured environment
    Instruction* ip;     // The instruction pointer (local to this function)
    Value* slots;        // Pointer to the first register (R0) in the VM stack
    int returnToReg;     // The caller's register where the result will be placed
} CallFrame;
```

The `slots` pointer is the anchor of the register window. When an instruction references `R5`, the VM internally calculates the address as `frame->slots + 5`.

## 3. The Sliding Window (Zero-Copy Argument Passing)
Walia eliminates the "Push/Pop" overhead found in stack machines by using a **Sliding Window** mechanic for function calls.

### The Calling Sequence:
1.  **Preparation:** The caller places the arguments into its own registers (e.g., `R10`, `R11`, `R12`).
2.  **Activation:** The VM creates a new `CallFrame` for the callee.
3.  **The Slide:** The new frame's `slots` pointer is set to point directly at the caller's `R11`.
4.  **Result:** To the callee, the caller's `R11` becomes its own `R0`. 

**Industry Benefit:** Arguments are never copied, moved, or pushed. They stay in the exact same memory address while the "Register Window" simply slides over them. This results in **O(1) call overhead**, regardless of the number of arguments.

## 4. Stack Layout Visualization
The VM stack is a contiguous block of memory. As the call depth increases, the windows move deeper into the stack.

```text
[ VM Stack Memory ]
+---------------------------+
| Frame 0 (Script)          |  <- vm.stack (R0)
| [R0][R1][R2][R3][R4][R5]  |
+---------------------------+
| Frame 1 (Function Call)   |  <- Frame 0's R4 becomes Frame 1's R0
|             [R0][R1][R2]  |
+---------------------------+
| Frame 2 (Nested Call)     |  <- Frame 1's R2 becomes Frame 2's R0
|                   [R0]    |
+---------------------------+
```

## 5. Register Reuse and Scoping
Within a single function, Walia’s compiler performs **Register Pressure Analysis**.
*   **Temporary Values:** Registers used for intermediate calculations (e.g., the result of `1 + 2`) are immediately released and reused for the next operation.
*   **Block Scoping:** When a lexical block `{}` ends, the registers assigned to variables within that block are reclaimed by the compiler for subsequent variables in the same function.

This aggressive reuse ensures that most Walia functions operate within a very small memory footprint (typically under 16 registers), keeping the data entirely within the **CPU L1 Cache**.

## 6. Persistence and the Sliding Stack
Because Walia is an **Orthogonal Persistence** language, the VM stack is located within the memory-mapped `.state` heap.
*   **State Recovery:** In the event of a system-level checkpoint, the current stack positions and register values are preserved.
*   **Resumable Logic:** When a **Continuation** is captured (via an Algebraic Effect), the VM "slices" the relevant register windows and saves them to the heap. Because Walia uses relative offsets for its register windows, these slices can be "spliced" back onto the stack at any time, even after a full system reboot.

---
**Status:** Technical Specification 1.0  
**Mechanism:** Sliding Register Windows  
**Complexity:** O(1) Call Overhead  

