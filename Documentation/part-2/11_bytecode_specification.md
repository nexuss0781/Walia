# Bytecode Specification: 32-Bit Instruction Word

## 1. Binary Layout
Every instruction in the Walia Virtual Machine is a fixed-width **32-bit (4-byte) word**. This uniformity ensures that the VM can fetch and decode instructions with a single memory access and zero length-detection overhead. 

Walia instructions are encoded in **Little-Endian** format (matching standard x64 and ARM64 hardware) and follow two primary layouts depending on the opcode's requirements.

### I. Standard 3-Operand Layout
Used for arithmetic, logic, and simple data movement.

| Bit Range | Size | Field | Description |
| :--- | :--- | :--- | :--- |
| 00 - 07 | 8 bits | **OpCode** | The specific operation to perform. |
| 08 - 15 | 8 bits | **A** | Destination Register index (R0-R255). |
| 16 - 23 | 8 bits | **B** | First Source Register index (R0-R255). |
| 24 - 31 | 8 bits | **C** | Second Source Register index (R0-R255). |

### II. Constant/Jump Layout (16-bit Operand)
Used for loading constants, jumping, or defining closures where a larger index or offset is required.

| Bit Range | Size | Field | Description |
| :--- | :--- | :--- | :--- |
| 00 - 07 | 8 bits | **OpCode** | The specific operation to perform. |
| 08 - 15 | 8 bits | **A** | Target Register or Condition Register. |
| 16 - 31 | 16 bits | **BC** | 16-bit Unsigned Operand (Index or Offset). |

## 2. Field Definitions

### OpCode (8 bits)
The first byte determines how the VM interprets the remaining 24 bits. With 8 bits, Walia supports up to **256 unique instructions**.

### Operand A (8 bits)
Typically represents the **Destination Register**. In a register-based VM, this is the index in the current call frame where the result of an operation will be stored.
*   **Example:** In `ADD R5, R1, R2`, Operand A is `5`.

### Operand B & C (8 bits each)
Typically represent **Source Registers**. 
*   **B:** First operand for binary operations or the source for a `MOVE`.
*   **C:** Second operand for binary operations or the argument count for a `CALL`.

### Operand BC (16 bits)
Formed by combining the bits of the B and C slots into a single unsigned 16-bit integer.
*   **Constant Access:** Allows the VM to address up to **65,536 constants** per chunk.
*   **Branching:** Allows jumps of up to **65,535 instructions** forward or backward in the bytecode.

## 3. Implementation Macros (C-Level)
The WVM utilizes high-speed bitwise macros to extract operands from the 32-bit instruction word during the dispatch loop.

```c
typedef uint32_t Instruction;

// Extraction
#define GET_OP(inst) ((inst) & 0xFF)
#define GET_A(inst)  (((inst) >> 8) & 0xFF)
#define GET_B(inst)  (((inst) >> 16) & 0xFF)
#define GET_C(inst)  (((inst) >> 24) & 0xFF)

// Combined 16-bit Extraction
#define GET_BC(inst) (((inst) >> 16) & 0xFFFF)
```

## 4. Packing and Compilation
During the compilation phase, the Walia compiler packs AST nodes into these 32-bit words. Because the layout is fixed, the compiler can write bytecode directly into the persistent `Chunk` without complex serialization logic.

### Instruction Word Example: `ADD R10, R1, R2`
If `OP_ADD` is hex `0x04`, the resulting 32-bit word in memory would be:
`0x02010A04`
*(C=02, B=01, A=0A, Op=04)*

## 5. Architectural Benefits
*   **Zero Decoding Logic:** There are no variable-length prefixes or escape codes. The VM knows the location of every operand before the opcode is even decoded.
*   **Parallelism Ready:** Fixed-width instructions make it significantly easier to implement speculative execution or multi-threaded instruction pre-fetching in future evolutions of the Walia runtime.

---
**Status:** Technical Specification 1.0  
**Format:** Fixed-Width 32-bit  
**Endianness:** Little-Endian  

