# Shadow Paging and ACID Compliance

## 1. The Reliability Paradigm
In traditional enterprise databases, reliability is typically achieved through **Write-Ahead Logging (WAL)**, which requires every change to be written twice (once to a log and once to the data file). WaliaDB eliminates this "Double-Write Penalty" by utilizing **Shadow Paging**—a Copy-on-Write (CoW) mechanism that ensures the database is always in a consistent state on disk.

WaliaDB provides full **ACID (Atomicity, Consistency, Isolation, Durability)** guarantees through a single, atomic pointer swap in the Superblock.

## 2. Shadow Paging Mechanics (Copy-on-Write)
In WaliaDB, we never overwrite a committed data page. Instead, modifications occur in a "Shadow" segment of the persistent heap.

### I. Logical vs. Physical Mapping
The engine maintains a distinction between these two layers:
*   **Logical Page ID:** The persistent identifier used by the SQL and NoSQL engines (e.g., "Page 10 of the Users Table").
*   **Physical Page ID:** The actual 4096-byte offset within the `.wld` file where the data currently resides.

### II. The Shadow Table
The mapping between Logical and Physical IDs is stored in a **Shadow Table** (Page-aligned). This table is the "Source of Truth" for the current version of the database.

## 3. Transaction Lifecycle

### 1. Begin (`db_transaction_begin`)
When a transaction starts, it captures the current `shadowTableRoot` from the Superblock. This provides a point-in-time snapshot of the entire database.

### 2. Mutation (`db_get_page_write`)
When an engine needs to modify data:
1.  The Pager allocates a **new physical page** from the free list.
2.  The content of the original page is copied to this new "Shadow" page.
3.  The transaction's local Shadow Table is updated to map the Logical ID to the new Physical ID.
4.  The mutation happens entirely on the shadow page. The original committed data remains untouched and available to other readers.

### 3. Atomic Commit (`db_transaction_commit`)
To finalize the changes, WaliaDB performs a "Sovereign Handover":
1.  **Barrier:** All modified shadow pages are flushed to the physical disk via `msync`.
2.  **Pointer Swap:** The `shadowTableRoot` in the Superblock is updated to point to the new Shadow Table.
3.  **Final Flush:** The Superblock (Page 0) is flushed. Once the OS confirms this write, the transaction is officially Sovereign.

## 4. ACID Implementation Details

### I. Atomicity
Because the "Commit" is defined by a single 32-bit pointer change in the Superblock, a transaction is binary. Either the Superblock points to the new state, or it points to the old state. There is no such thing as a "half-committed" transaction in WaliaDB.

### II. Consistency
The **Relational Sentry** (Phase 2.4) validates all data against the schema before the Shadow Pager is ever engaged. This ensures that only valid data is promoted from "Shadow" to "Sovereign" status.

### III. Isolation (Snapshot Isolation)
WaliaDB provides high-concurrency isolation. Because writers work on shadow copies, readers (queries) can continue to read from the old physical pages without locks. This enables **Parallel Dual-Core** execution where the SQL Agency and NoSQL Party never block each other’s read operations.

### IV. Durability
Durability is guaranteed by the `db_pager_flush` mechanism. WaliaDB ensures that all data pages are physically resident on the disk platter before the Superblock is updated.

## 5. Crash Recovery
WaliaDB requires **Zero Recovery Time**. 
*   If the system crashes during execution, the Superblock on disk still points to the last successful `shadowTableRoot`. 
*   Upon reboot, the Pager simply mounts the file and resumes from that root. The "Shadow" pages from the interrupted transaction are simply ignored and reclaimed by the next Garbage Collection cycle.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Algorithm:** Shadow Paging (CoW)  
**Safety Level:** Atomic Commit / ACID Compliant
