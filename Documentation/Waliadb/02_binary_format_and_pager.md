# Binary Format and the Sovereign Pager

## 1. Physical Storage Specification
WaliaDB utilizes a strict, hardware-aligned binary format encapsulated in the `.wld` file extension. The architecture is designed to map the physical constraints of modern storage (SSD sectors) and memory (CPU pages) directly into the database substrate.

### I. The 4KB Page Boundary
The fundamental unit of storage in WaliaDB is the **4096-byte (4KB) Page**. 
*   **Hardware Alignment:** By aligning every data block to the 4KB OS memory page boundary, WaliaDB eliminates "Partial Page Writes" and ensures that every I/O operation is perfectly synchronized with the hardware's native page table.
*   **Logical Addressing:** Pages are identified by a 32-bit `PageID`, allowing a single `.wld` file to scale to **4.2 billion pages** (~17 Terabytes).

## 2. File Structure: The Sovereign Map

### Page 0: The Superblock
The first 4KB segment of any WaliaDB file is the **Superblock**. It serves as the anchor for the entire database.

| Field | Type | Description |
| :--- | :--- | :--- |
| `Magic` | uint32 | Validation constant (`0x574C4442`). |
| `Version` | uint32 | Database engine version identifier. |
| `PageCount` | uint32 | High-water mark of allocated pages. |
| `CatalogRoot`| PageID | Entry point to the Database Catalog (Page 1). |
| `ShadowRoot` | PageID | Pointer to the active Shadow Table (ACID Anchor). |
| `Timestamp` | uint64 | Last successful atomic commit time. |

### The Standard Page Header
Every page (from Page 1 to N) begins with a mandatory **16-byte metadata header**, ensuring that the data payload always starts on an 8-byte aligned boundary.

*   **Type (4 bytes):** Identifies the page role (Catalog, B-Tree, Hash, Overflow).
*   **LSN (4 bytes):** Log Sequence Number for recovery auditing.
*   **Checksum (4 bytes):** CRC for physical integrity validation.
*   **Padding (4 bytes):** Reserved for 64-bit alignment of the subsequent data.

## 3. The Pager Engine (`db_pager.c`)
The Pager is the low-level I/O manager. It abstracts the physical file system into a logical sequence of memory-addressable pages.

### I. Zero-Copy Memory Mapping (`mmap`)
WaliaDB does not use traditional `read()` or `write()` system calls for data retrieval. It utilizes `mmap` with the `MAP_SHARED` flag.
*   **Lazy Loading:** The Operating System kernel only loads pages from disk into physical RAM when the Walia VM actually dereferences the pointer.
*   **UFO Speed:** Because the file is part of the VM's address space, retrieving a "row" is just a memory-offset calculation. There is zero data movement between the OS buffer and the application.

### II. Geometric File Growth
To maintain high insertion speeds, the Pager avoids small, frequent file-system updates.
*   **Strategy:** When the database requires more space, the Pager doubles the file size (`ftruncate`) and re-maps the address space.
*   **Benefit:** This amortizes the cost of allocation, ensuring that WaliaDB can scale from megabytes to gigabytes without a performance cliff.

## 4. Logical-to-Physical Resolution
The Pager operates in tandem with the Shadow Paging system. 
1.  **Request:** An engine (SQL or NoSQL) asks for PageID 50.
2.  **Resolution:** The Pager calculates the offset: `mapping_base + (50 * 4096)`.
3.  **Access:** The CPU fetches the data directly from the persistent image.

## 5. Industrial Safety Features
*   **Magic Number Validation:** On boot, the Pager refuses to mount any file that does not lead with the `WLDB` signature, preventing accidental corruption from non-database files.
*   **Atomic Flush (`msync`):** The Pager provides the `db_pager_flush` primitive, which forces the hardware to commit a specific 4KB segment to the physical platter, a prerequisite for ACID compliance.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Alignment:** 4096-byte Hardware Paging  
**Mechanism:** `mmap` Persistent Imaging
