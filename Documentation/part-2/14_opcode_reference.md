# Opcode Reference: The Walia Instruction Set (ISA)

## 1. Instruction Set Architecture (ISA) Overview
The Walia Virtual Machine (WVM) utilizes a 3-address, fixed-width 32-bit instruction set. This reference categorizes every opcode by its functional group, detailing the operand requirements and the effect on the Virtual Register File.

**Legend:**
*   **A, B, C:** 8-bit Register Indices (R0-R255).
*   **BC:** 16-bit Unsigned Integer (Constant Index or Jump Offset).
*   **R(X):** The value stored in Register X of the current CallFrame.

---

## 2. Data Movement & Literals
Instructions used to populate registers with immediate data or move values between slots.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_LOAD_CONST` | 0x00 | A, BC | `R(A) = Constants[BC]`. Loads value from the constant pool. |
| `OP_LOAD_BOOL` | 0x01 | A, B, _ | `R(A) = (B == 1 ? true : false)`. Loads boolean immediate. |
| `OP_LOAD_NIL` | 0x02 | A, _, _ | `R(A) = nil`. Sets target register to the nil immediate. |
| `OP_MOVE` | 0x03 | A, B, _ | `R(A) = R(B)`. Copies value from source to destination. |

---

## 3. Arithmetic & Comparison
3-address operations performed natively on NaN-boxed doubles.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_ADD` | 0x04 | A, B, C | `R(A) = R(B) + R(C)`. Addition. |
| `OP_SUB` | 0x05 | A, B, C | `R(A) = R(B) - R(C)`. Subtraction. |
| `OP_MUL` | 0x06 | A, B, C | `R(A) = R(B) * R(C)`. Multiplication. |
| `OP_DIV` | 0x07 | A, B, C | `R(A) = R(B) / R(C)`. Division. |
| `OP_EQUAL` | 0x08 | A, B, C | `R(A) = (R(B) == R(C))`. Strict equality check. |
| `OP_GREATER` | 0x09 | A, B, C | `R(A) = (R(B) > R(C))`. Greater-than comparison. |
| `OP_LESS` | 0x0A | A, B, C | `R(A) = (R(B) < R(C))`. Less-than comparison. |
| `OP_NEGATE` | 0x0B | A, B, _ | `R(A) = -R(B)`. Arithmetic negation. |
| `OP_NOT` | 0x0C | A, B, _ | `R(A) = !R(B)`. Logical NOT (falsiness check). |

---

## 4. Control Flow & Call Infrastructure
Instructions that manipulate the Instruction Pointer (`ip`) and manage `CallFrame` transitions.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_JUMP` | 0x0D | _, BC | `ip += BC`. Unconditional forward branch. |
| `OP_JUMP_IF_FALSE`| 0x0E | A, BC | If `R(A)` is falsy, `ip += BC`. Conditional branch. |
| `OP_LOOP` | 0x0F | _, BC | `ip -= BC`. Unconditional backward branch. |
| `OP_CALL` | 0x10 | A, B, C | Call function at `R(B)` with `C` args. Store result in `R(A)`. |
| `OP_RETURN` | 0x11 | A, _, _ | Returns `R(A)` to caller and dismantles the current frame. |

---

## 5. Sovereign Persistence
Instructions that interact with the memory-mapped persistent heap and the Global Root.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_DEFINE_GLOBAL` | 0x12 | A, BC | Sets `Globals[Constants[BC]] = R(A)`. Initial definition. |
| `OP_GET_GLOBAL` | 0x13 | A, BC | `R(A) = Globals[Constants[BC]]`. Fetch from global root. |
| `OP_SET_GLOBAL` | 0x14 | A, BC | Updates `Globals[Constants[BC]] = R(A)`. Errors if undefined. |

---

## 6. Closures & Algebraic Effects
Advanced instructions for resumable continuations and lexical scoping.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_CLOSURE` | 0x15 | A, BC | Creates a closure from `Constants[BC]`. Followed by upvalue data. |
| `OP_GET_UPVALUE` | 0x16 | A, B, _ | `R(A) = Upvalues[B]`. Reads from captured variable. |
| `OP_SET_UPVALUE` | 0x17 | A, B, _ | `Upvalues[B] = R(A)`. Writes to captured variable. |
| `OP_HANDLE` | 0x18 | A, BC | Registers handler for `Constants[A]`. Jump to `ip + BC` if triggered. |
| `OP_PERFORM` | 0x19 | A, BC | Performs effect `Constants[BC]`. Captured slice returns to `R(A)`. |
| `OP_RESUME` | 0x1A | A, B, _ | Resumes continuation `R(A)` with value `R(B)`. |

---

## 7. Operational & Debug
Standard system instructions.

| Opcode | Hex | Layout | Description |
| :--- | :--- | :--- | :--- |
| `OP_PRINT` | 0x1B | A, _, _ | Invokes the native print logic on `R(A)`. |

---

## 8. Technical Notes on Instruction Lifecycle
1.  **Fetch:** The VM reads 4 bytes from `vm.ip`.
2.  **Decode:** Operands are extracted via bit-shifts (e.g., `(inst >> 8) & 0xFF`).
3.  **Dispatch:** The VM uses **Computed Gotos** to jump directly to the C label associated with the Opcode Hex ID.
4.  **Persistence:** If an opcode modifies a heap-resident object (like `OP_SET_UPVALUE` or `OP_SET_GLOBAL`), the **Write Barrier** (`markCard`) is triggered automatically within the C-handler for that opcode.

---
**Status:** Technical Specification 1.0  
**Instruction Set:** 32-bit Fixed Register ISA  
**Implementation:** C-Label Dispatch
