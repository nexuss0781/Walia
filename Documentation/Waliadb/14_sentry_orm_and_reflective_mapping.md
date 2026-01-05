# Walia Sentry: Native ORM and Reflective Mapping

## 1. The Intelligence Bridge
**Walia Sentry** is the high-level intelligence layer that integrates the **Walia Language** and **WaliaDB** into a single, unified substrate. It removes the "Impedance Mismatch" found in traditional software stacks by eliminating the need for manual data mapping logic. In the Sentry paradigm, persistent storage is treated as a natural extension of the language's memory.

Through **Reflective Mapping**, the Sentry analyzes high-level Walia classes and automatically generates the optimized physical schemas (B-Trees or Hash Indices) required for UFO-grade execution.

## 2. Declarative Sovereignty: Decorators
Walia Sentry introduces the `@` decorator syntax to the language grammar, allowing developers to define the storage destiny of their data structures directly at the point of declaration.

### I. The `@sql` Decorator
Instructs the Walia Sentry to map the class to the **SQL Agency Party**. 
*   **Storage:** Persistent Clustered B+ Tree.
*   **Integrity:** Enables Primary Key enforcement and Pointer-Based Joins.

### II. The `@nosql` Decorator
Instructs the Sentry to map the class to the **NoSQL Party**.
*   **Storage:** Persistent Extendible Hash Directory.
*   **Flexibility:** Optimized for variable-width Slotted Pages and schema-less evolution.

### III. The `@temporal` Decorator
Enables the **Temporal Engine**, instructing the Pager to retain historical Shadow Root IDs for time-travel queries.

## 3. Reflective Mapping Engine
The Sentry Mapper (`db_sentry_mapper.c`) performs a compile-time analysis of the Walia Class AST (Abstract Syntax Tree) to construct a **Sovereign Mapping Contract**.

### I. Automatic Type Inference
The engine analyzes field initializers to determine the optimal physical column type:
*   **Number (literal) →** `WLD_TYPE_FLOAT` or `INT`.
*   **Boolean →** `WLD_TYPE_BOOL`.
*   **Vector() →** `WLD_TYPE_VECTOR` (Triggers Vector Wing indexing).
*   **Object Reference →** `WLD_TYPE_POINTER` (Triggers Pointer-Based Join optimization).

### II. Heuristic Primary Keys
To achieve zero-configuration persistence, the Sentry automatically designates the first field of a class (or a field explicitly named `id`) as the **Primary Key** for the B+ Tree.

## 4. Sovereign Contracts and Drift Protection
Every Walia class generates a **Content-Addressable Logic Hash** (SHA-256).

*   **Verifiable Integrity:** The hash is stored in the `.wld` Catalog.
*   **Drift Detection:** If a developer modifies a `@sql` class in the source code but attempts to run it against an old `.wld` file, the Sentry detects the hash mismatch.
*   **Safety:** This prevents "Silent Corruption," ensuring the VM never attempts to read a 16-byte row using an 8-byte logic contract.

## 5. Lambda Unrolling (UFO-Grade Filters)
The most advanced feature of the Walia Sentry is its ability to "unroll" native closures into database filters.

*   **Standard ORM:** Loops through objects, calling a closure for each one (Slow).
*   **Walia Sentry JIT:** Intercepts the closure (e.g., `u => u.rank > 10`) and compiles it directly into **SIMD-accelerated Bulk Opcodes** (`OP_DB_SIMD_FILTER_GT`).
*   **Result:** High-level Walia code executes at the hardware's physical bandwidth limit, processing millions of rows in milliseconds.

## 6. Sentry Lifecycle

| Phase | Action | Subsystem |
| :--- | :--- | :--- |
| **Parsing** | Identify `@` decorators and class fields. | `db_sentry_parser.c` |
| **Analysis** | Infer types and generate `WldColumn` offsets. | `db_sentry_mapper.c` |
| **Compilation** | Map field access to memory offsets; JIT unroll closures. | `db_sentry_jit.c` |
| **Deployment** | Atomically update Catalog and initialize B+ Trees/Hash Dirs. | `db_catalog.c` |

By embedding the ORM into the language substrate, Walia Sentry provides the ease of use found in dynamic scripting with the data integrity and raw performance of an industrial-grade C database engine.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Interface:** `@sql`, `@nosql`, `@temporal` Decorators  
**Engine:** Reflective Type Inference / Lambda Unrolling
