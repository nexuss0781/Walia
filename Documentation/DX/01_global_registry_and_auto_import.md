# Project Intelligence: Global Registry and Auto-Discovery

## 1. Overview
Walia redefines project management by treating the entire codebase as a single, self-aware logical entity. The **Sovereign Intelligence Engine** eliminates the manual overhead of `import` statements and header inclusions. By shifting the responsibility of symbol discovery from the developer to the engine, Walia enables a "Zero-Friction" workflow where any function, class, or global variable is instantly available across the entire project.

## 2. The Hash-Sentry (Block-Level Fingerprinting)
The foundation of Walia’s project awareness is the **Hash-Sentry Scanner**. Unlike traditional compilers that view files as monolithic blobs of text, the Hash-Sentry identifies discrete **Logic Nodes** (`fun` or `class` blocks).

### I. The Fingerprinting Process
1.  **Extraction:** During a project scan, the Sentinel identifies block boundaries using a high-speed linear token pass.
2.  **Hashing:** It generates a **SHA-256 Content-Identity** for the code residing strictly inside the braces `{}` of each node.
3.  **Deduplication:** Identical logic blocks across different files are identified via their hash, ensuring that only unique logic consumes memory in the persistent heap.

### II. Differential Scanning
To maintain UFO-grade build speeds, the Hash-Sentry utilizes **Incremental Invalidation**.
*   **Clean Nodes:** If the current hash matches the entry in the `walia.registry`, the block is ignored.
*   **Dirty Nodes:** If the hash differs, the logic is marked for re-analysis, and the previous version is archived in the **Sovereign History**.

## 3. The Sovereign Registry
The **Sovereign Registry** is the persistent knowledge base of the project, stored in `walia.registry`. It acts as the "Universal Phone Book" for every symbol in the ecosystem.

### I. Registry Entry Schema
Every registered symbol contains the following industrial metadata:

| Field | Description |
| :--- | :--- |
| **Symbol Name** | The unique identifier (e.g., `ProcessTransaction`). |
| **File Path** | Absolute path to the source `.wal` file. |
| **Geography** | Exact line numbers and token spans (`startLine`, `endLine`). |
| **Logic Hash** | The current SHA-256 fingerprint of the body. |
| **Status** | The health of the node (`CLEAN`, `DIRTY`, `NEW`, `CONFLICT`). |

## 4. Zero-Config Auto-Discovery (Implicit Importing)
Walia eliminates "Import Hell" through **Implicit Linking**. When the compiler encounters an identifier that is not defined in the local scope, it does not throw an error. Instead, it queries the Sovereign Registry.

*   **Logic Resolution:** If the registry finds a unique match for the symbol, the compiler automatically injects a virtual import.
*   **Zero Latency:** Because the registry is a persistent binary hash map, symbol resolution occurs in $O(1)$ time, regardless of whether the project contains 100 or 60,000 files.

## 5. Explicit Prioritization
While Walia is designed for zero-configuration, it respects developer intent. If a developer provides an explicit `import` statement at the top of a file, the compiler prioritizes that specific source over the Global Registry. This allows for local overrides during development or testing.

## 6. Performance Characteristics
The Intelligence Engine is optimized for massive industrial codebases:
*   **Parallel Probing:** The scanner utilizes all CPU cores to hash files simultaneously.
*   **Barren Folder Skip:** If a directory contains no `.wal` files, it is marked as "Barren" in the registry and skipped in future scans unless a file-system change is detected.
*   **Memory Impact:** The registry uses NaN-boxed pointer mapping to minimize RAM usage, allowing for multi-million symbol indices to reside in standard L3 cache.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**System:** Sovereign Intelligence Engine  
**Index:** `walia.registry` (Binary Image)
