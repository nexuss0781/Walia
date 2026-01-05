# NoSQL Party: Extendible Hashing Logic

## 1. High-Velocity Discovery
The **NoSQL Party** core of WaliaDB is engineered for high-frequency point lookups and massive document ingestion. While the SQL Agency focuses on ordered relationships via B+ Trees, the NoSQL engine prioritizes constant-time access ($O(1)$) through **Extendible Hashing**.

This logic allows the database to grow dynamically without the catastrophic "Global Rehash" pause common in standard hash maps. By utilizing a persistent directory, WaliaDB can split individual data buckets incrementally, maintaining UFO-grade availability during multi-terabyte expansions.

## 2. Directory-Based Architecture
The NoSQL index is structured into two persistent layers mapped within the `.wld` heap:

### I. The Hash Directory (Page 4+)
A master table of PageIDs that acts as a jump-map. 
*   **Global Depth:** Determines the number of entries in the directory ($2^n$).
*   **Uniform Access:** Any key lookup begins by masking the key's hash with the current Global Depth to find the specific directory slot instantly.
*   **Capacity:** ~1024 bucket pointers per directory page.

### II. The Buckets (Data Pages)
Physical 4KB pages containing the actual `[KeyHash][DocumentPageID]` entries.
*   **Local Depth:** Tracks how many bits of the hash are shared by entries in this specific bucket.
*   **Sovereign Entries:** Each bucket can hold ~330 document references before requiring a split.

## 3. Incremental Resizing Mechanics
The defining enterprise feature of Extendible Hashing is its ability to scale with minimal disruption.

### I. Bucket Splitting (The Local Split)
When a bucket overflows but its **Local Depth** is less than the **Global Depth**:
1.  A new physical page is allocated.
2.  The Local Depth is incremented for both the old and new buckets.
3.  Entries are redistributed between the two based on the next significant bit of their hash.
4.  The Directory pointers are updated to point to the new bucket.
5.  **Performance Impact:** Isolated to a single bucket; no other part of the database is locked.

### II. Directory Doubling (The Global Split)
When a bucket overflows and its Local Depth equals the Global Depth:
1.  The Global Depth is incremented.
2.  The Directory size is doubled (logically).
3.  The first half of the directory is copied to the second half, doubling the pointers to existing buckets.
4.  The specific overflowing bucket is then split as per the standard local split logic.

## 4. O(1) Lookup Performance
WaliaDB achieves constant-time lookup by reducing the "Search" operation to two direct memory-offset calculations:

1.  **Step 1:** Hash the input string using the **Walia SHA-256 Engine** (from Phase 7).
2.  **Step 2:** `index = hash & ((1 << globalDepth) - 1)`.
3.  **Step 3:** Retrieve `bucketPageId` from `Directory[index]`.
4.  **Step 4:** Retrieve `documentPageId` from the Bucket.

Total memory accesses: **Exactly 2**. This remains true whether the database contains 1,000 documents or 1 billion documents.

## 5. Persistence and Atomic Safety
The Extendible Hashing engine is fully integrated with WaliaDB’s **Shadow Paging**.
*   **Dirty Tracking:** Bucket splits and directory updates trigger the `markCard()` write barrier.
*   **Consistency:** Because the Directory and Buckets are stored on separate pages, a transaction commit ensures that the directory only points to the new bucket once the physical data has been successfully flushed to the `.wld` file.

## 6. Heuristic: When NoSQL Over SQL
WaliaDB’s parallel architecture allows developers to choose the right tool for the task:
*   **SQL Agency:** Use for range queries (`WHERE age > 20`) or strict relational integrity.
*   **NoSQL Party:** Use for primary-key lookups, session management, or massive object stores where $O(1)$ speed is the primary requirement.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Mechanism:** Persistent Extendible Hashing  
**Performance:** O(1) Point Lookup
