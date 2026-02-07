# SQL Agency: Clustered B+ Tree Architecture

## 1. The "Table-is-Tree" Paradigm
In the WaliaDB SQL Agency, a table is not a flat file with a separate index. Instead, it utilizes a **Clustered B+ Tree** architecture. This means the primary key index and the actual row data are unified into a single, cohesive persistent structure. 

By storing data directly within the leaf nodes of the tree, WaliaDB eliminates the "Double I/O" penalty of traditional databases, ensuring that locating a key simultaneously locates the record.

## 2. Node Geometry (4KB Optimization)
To achieve UFO-grade throughput, every B+ Tree node is sized to exactly **4096 bytes**, aligning perfectly with the Sovereign Pager and the underlying hardware sectors.

### I. Internal Nodes (Navigation)
Internal nodes act as high-speed routers. They store `[Key][PageID]` pairs, where the PageID points to a child node in the next layer of the tree.
*   **Branching Factor:** ~330.
*   **Capacity:** A 3-level tree can index over **35 million pages**, providing logarithmic access to massive datasets with minimal depth.

### II. Leaf Nodes (Data Storage)
Leaf nodes are the terminal points of the tree. They store `[Key][Value]` pairs, where the "Value" is the actual packed binary row.
*   **Clustered Density:** ~250 records per page (depending on row width).
*   **Sovereign Storage:** Because Walia uses NaN-boxing, the "Value" in the leaf is often the raw bit-pattern of a Walia object, ready for immediate VM injection.

## 3. Persistent Node Structure
Every B+ Tree node begins with the standard 16-byte WaliaDB Page Header, followed by relational metadata:

| Field | Size | Description |
| :--- | :--- | :--- |
| `isLeaf` | 1 byte | Flag distinguishing navigation from data storage. |
| `entryCount` | 4 bytes | Number of active keys in the node. |
| `parent` | 4 bytes | Logical PageID of the parent (for upward traversal). |
| `next` | 4 bytes | Logical PageID of the right-hand neighbor (Leaf only). |
| `prev` | 4 bytes | Logical PageID of the left-hand neighbor (Leaf only). |

## 4. Sequential Scan Optimization (Horizontal Linking)
Unlike standard B-Trees, WaliaDB leaf nodes are **Doubly-Linked**.
*   **UFO-Speed Scans:** For queries like `SELECT *` or `WHERE id > 100`, the SQL Agency performs a single $O(\log n)$ search to find the starting leaf, then jumps horizontally using the `next` pointers.
*   **Read-Ahead Synergy:** This sequential access pattern allows the OS kernel to pre-fetch subsequent pages into the CPU cache, achieving the physical bandwidth limit of the storage hardware.

## 5. Operations Lifecycle

### I. Search ($O(\log n)$)
The engine performs a binary search within each internal node to identify the correct child pointer, descending until it reaches a leaf. Due to high fan-out, most searches involve only 3 to 4 page accesses.

### II. Atomic Insertion & Splitting
When a leaf node exceeds its 4KB capacity:
1.  **Shadowing:** The Shadow Pager (Phase 1.3) creates a new physical copy of the page.
2.  **Splitting:** The node is split into two, and the middle key is promoted to the parent.
3.  **Integrity:** Because this happens within a `WldTransaction`, the tree structure remains balanced and valid even if the process is interrupted.

## 6. Performance Characteristics

| Metric | Performance |
| :--- | :--- |
| **Point Lookup** | $O(\log_{330} n)$ — Typically 3-4 memory jumps. |
| **Range Scan** | $O(1)$ per page after initial seek. |
| **Storage Overhead** | < 5% (Internal nodes are highly compressed). |
| **Cache Locality** | High (Related keys are stored in the same 4KB page). |

By utilizing clustered B+ Trees, the SQL Agency provides the rigorous ordering and search efficiency required for enterprise relational models, serving as the high-integrity counterpart to the NoSQL Party's hash-based engine.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Algorithm:** Persistent Clustered B+ Tree  
**Fan-out:** 330 (Internal) / 250 (Leaf)
