# Sovereign Query Engine (SQE): JIT Query Compiler

## 1. Elimination of the Interpretation Tax
The **Sovereign Query Engine (SQE)** is the high-speed execution arm of WaliaDB. Traditional databases utilize a "Volcano Model" (iterator-based), where every row processed triggers multiple virtual function calls and conditional branches. WaliaDB eliminates this "Interpretation Tax" through **JIT (Just-In-Time) Query Compilation**.

Instead of interpreting a query string for every row, the SQE compiles the declarative intent of SQL and NoSQL filters directly into a tight, linear stream of **Walia Bytecode**, executing at the hardware's physical bandwidth limit.

## 2. The JIT Pipeline: Logic-to-Machine
When a query (e.g., `db_query`) is invoked, the SQE performs a three-stage transformation within the sovereign substrate:

### I. Schema-Aware Normalization
The engine retrieves the `WldTableDescriptor` from the Master Catalog. It identifies the physical byte offsets for every column involved in the query.
*   **Smart Feature:** Property names (e.g., `"age"`) are replaced with direct offsets (e.g., `+16 bytes`) during compilation, removing string-matching overhead from the execution loop.

### II. Bytecode Generation
The SQE emits specialized **Database Opcodes** that target the Walia Virtual Machine’s register file. 
*   **Result-Set Allocation:** The JIT emits an initial `OP_CALL` to the native `List()` constructor to prepare the result buffer.
*   **Relational Logic:** Joins are compiled into `OP_DB_RESOLVE_PTR` (Pointer-Based Joins) or B-Tree traversal opcodes.

### III. Loop Unrolling
To maximize CPU instruction throughput, the SQE unrolls database scans into batches of 8 rows. 
*   **Mechanism:** Instead of "Process 1 row, Jump back," the engine generates code that says "Process 8 rows, Check Mask, Jump back." This aligns perfectly with the CPU's fetch cycle and cache line size.

## 3. Bulk ISA Extensions (Sovereign Opcodes)
The Walia Virtual Machine is extended with "Bulk Opcodes" specifically designed for JIT query execution:

| Opcode | Description |
| :--- | :--- |
| `OP_DB_LOAD_COL` | Directly "rips" an 8-byte NaN-boxed value from a mapped memory address into a VM register. |
| `OP_DB_SIMD_FILTER_GT` | Invokes the AVX-512/NEON core to compare 8 rows against a constant in a single cycle, returning a match bitmask. |
| `OP_DB_PROJECT` | Bulk moves matched columns into the result list with zero intermediate allocations. |
| `OP_DB_NEXT_PAGE` | Performs the horizontal "leaf-jump" in a B+ Tree, maintaining O(1) sequential scan speed. |

## 4. Predicated Execution (Branchless Logic)
Modern CPUs suffer from "Pipeline Stalls" when encountering unpredictable branches (`if/else`). The SQE JIT utilizes **Predicated Execution**.

1.  **Filter:** The `OP_DB_SIMD_FILTER` produces a bitmask.
2.  **Collection:** The JIT generates a series of "Predicated Moves" that only collect data if the corresponding bit in the mask is `1`.
3.  **Efficiency:** This ensures that the CPU execution path is a straight line, regardless of whether 0% or 100% of the rows match the query.

## 5. Persistence: Logic Fingerprinting (Plan Caching)
Following Walia's **Content-Addressable Logic** philosophy, the SQE implements **Persistent Plan Caching**.

*   **Hashing:** Every unique SQL/NoSQL query generates a SHA-256 fingerprint based on its logic and target schema.
*   **Sovereign Reuse:** The compiled bytecode is stored in the `.wld` heap as a `WldQueryPlan`. 
*   **UFO Speed:** If a script runs the same query twice (even after a system reboot), the SQE skips compilation entirely and dispatches the existing bytecode in microseconds.

## 6. Performance Impact

| Engine Layer | Traditional Query Engine | WaliaDB SQE |
| :--- | :--- | :--- |
| **Model** | Interpreted Operator Tree | JIT-Compiled Bytecode Loop |
| **Call Overhead** | $O(\text{Operators} \times \text{Rows})$ | **Zero** (Single Loop) |
| **Memory Access** | Buffered / Copying | **Zero-Copy / Memory-Mapped** |
| **Branching** | High (Conditional per Row) | **Minimal** (SIMD Masked) |

By treating a database query as a first-class compiled function, the SQE provides the "UFO-grade" processing power required for multi-trillion neuron searches and enterprise-scale relational analytics.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Method:** Relational JIT Compilation  
**ISA:** Walia Bulk Extensions
