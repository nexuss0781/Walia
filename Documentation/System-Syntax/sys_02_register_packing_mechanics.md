# Register Packing Mechanics: Zero-Cost Abstraction1

## 1. The Virtual Stack Problem
In the standard Walia VM (Phases 1-10), every variable consumes a full 64-bit stack slot (`Value`). While efficient for dynamic typing, this is wasteful for systems programming. If you declare four `u8` variables (4 bytes total), standard Walia would consume 32 bytes (4 slots) of memory bandwidth.

**Phase 11.1** introduces **Register Packing**. The Sovereign Compiler is now intelligent enough to combine multiple "Metal" variables into a single virtual register, mirroring how a physical CPU manages sub-registers (e.g., `AL` inside `RAX`).

## 2. The Packing Algorithm
When the compiler encounters a sequence of Metal variable declarations, it attempts to fit them into the current register window slot.

### Example Scenario
```javascript
fun init_serial() {
    var port_hi: u8 = 0x03;
    var port_lo: u8 = 0xF8;
    var interrupt: u16 = 0x0004;
    var control: u32 = 0x00000001;
}
```

### Compiler Visualization
Instead of allocating Registers R1, R2, R3, and R4, the compiler packs all four variables into **Register R1**.

**Physical Layout of R1 (64-bit):**
```text
[ control (4 bytes) ] [ interrupt (2 bytes) ] [ lo (1 byte) ] [ hi (1 byte) ]
|-------------------| |---------------------| |-------------| |-------------|
Bits 32-63            Bits 16-31              Bits 8-15       Bits 0-7
```

## 3. Sub-Register Addressing
The Walia VM does not see `port_hi` as "Register R1". It sees it as "Register R1, Offset 0".
*   **Instruction:** `OP_SYS_STORE_U8 R1, 0, 0x03`
*   **Instruction:** `OP_SYS_STORE_U8 R1, 1, 0xF8`

This significantly reduces "Register Pressure." By packing variables, complex functions with dozens of local variables can fit entirely within the fast L1 cache window, avoiding the need to spill registers to slower RAM.

## 4. Packing Rules
To ensure hardware alignment and prevent performance penalties, the compiler adheres to strict packing rules:

1.  **Alignment Priority:** A `u32` will always be placed on a 4-byte boundary (Offset 0 or 4). It will never be placed at Offset 1, 2, or 3.
2.  **Sequential Fill:** The compiler fills the register from LSB (Least Significant Byte) to MSB (Most Significant Byte).
3.  **Overflow Behavior:** If a variable does not fit in the remaining space of the current register, the compiler "Closes" that register and allocates a fresh one (R+1).

## 5. Performance Impact
Register Packing provides two critical enterprise benefits:

### I. Cache Density
By compressing data representation by up to 8x (for `u8`s), Walia ensures that more "Hot Data" fits into the CPU's register file and L1 cache. This reduces the number of cache misses during high-frequency loops (like pixel rendering or packet filtering).

### II. Reduced Frame Size
Smaller stack frames mean that function calls are cheaper. The **Sliding Window** (Phase 13) has to slide a shorter distance, and less memory needs to be "Saved/Restored" during context switches or interrupts.

## 6. Verification
You can verify packing behavior using the `--verbose` flag:

```bash
./walia --verbose driver.wal
```

**Disassembly Output:**
```text
0001 OP_SYS_STORE_U8   R1, 0x00, 0x03  ; port_hi (Offset 0)
0002 OP_SYS_STORE_U8   R1, 0x01, 0xF8  ; port_lo (Offset 1)
0003 OP_SYS_STORE_U16  R1, 0x02, 0x0004; interrupt (Offset 2)
```

If you see multiple variables targeting the same Register Index (`R1`) but different offsets, the Sovereign Packer is active.

---
**Status:** Enterprise Specification 1.2  
**Algorithm:** Best-Fit Alignment Packing  
**Efficiency:** Up to 800% density increase for byte-streams.