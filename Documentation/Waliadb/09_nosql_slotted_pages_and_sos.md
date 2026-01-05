# NoSQL Party: Slotted Pages and SOS

## 1. Flexible Payload Management
The **NoSQL Party** core is designed to store arbitrary, variable-width Walia objects. Unlike the SQL Agency, which utilizes fixed-width rows for relational rigidity, the NoSQL engine utilizes **Slotted Page Architecture**. This allows WaliaDB to pack multiple documents of varying sizes into a single 4KB page while maintaining stable logical identifiers.

These pages serve as the physical host for **Sovereign Object Storage (SOS)**—the mechanism that serializes complex Walia Maps into persistent binary streams.

## 2. Slotted Page Architecture
Every 4KB page dedicated to NoSQL storage follows a specialized internal layout designed to eliminate fragmentation and maximize storage density.

### I. The Bidirectional Growth Model
To prevent memory overlap, the Slotted Page grows from both ends:
*   **Header & Data:** Grow forward from the top of the page (Offset 32+).
*   **Slot Array:** Grows backward from the end of the page (Offset 4096-).

### II. The Slot Indirection Table
Instead of pointing directly to a memory address, WaliaDB uses a **Slot Index**.
*   **Logical ID:** A script refers to a document as `(PageID, SlotIndex)`.
*   **The Slot Entry:** A 4-byte structure containing the `offset` and `size` of the record.
*   **Stability:** If a document grows or is moved within the page, the `offset` in the slot array is updated, but the `SlotIndex` remains constant. This ensures that external indices (like the Hash Directory) never break.

## 3. Self-Healing Compaction
High-velocity NoSQL workloads often result in "holes" when documents are deleted or updated. WaliaDB implements an automatic **Self-Healing Compaction** algorithm:

1.  **Trigger:** When a new `db_save()` call finds insufficient contiguous free space.
2.  **Repacking:** The engine performs a single-pass scan, shifting all active data segments to the top of the page.
3.  **Result:** Fragmented space is unified into a single block, and the `freeSpaceStart` pointer is reset. 
4.  **Zero-Latency:** Because this happens entirely within the 4KB memory-mapped segment, compaction is near-instantaneous ($O(n)$ where $n$ is the number of slots).

## 4. Sovereign Object Storage (SOS)
SOS is the serialization layer that translates high-level Walia objects into the binary substrate.

*   **NaN-Box Continuity:** SOS does not "convert" data to JSON or BSON. It writes the raw 8-byte NaN-boxed bit patterns of the object's properties directly to the slotted page.
*   **Binary Imaging:** When a document is retrieved, the VM "lifts" the bits directly into its registers. This is the definition of **Zero-Serialization**, resulting in UFO-grade retrieval speeds.

## 5. Automatic Overflow Chaining
While the standard page size is 4KB, WaliaDB can store objects of virtually unlimited size (e.g., multi-gigabyte logs or AI training datasets).

*   **Inline Threshold:** Documents smaller than ~3.8KB are stored entirely within a Slotted Page for maximum speed.
*   **Chaining:** If an object exceeds the threshold, the SOS engine stores a **Fragment Header** in the slot and moves the bulk data into a linked list of **Overflow Pages**.
*   **Transparency:** The Walia VM handles the reconstruction of these fragments automatically, presenting a seamless, contiguous object to the developer.

## 6. Storage Metrics

| Feature | Performance / Limit |
| :--- | :--- |
| **Max Inline Doc Size** | 3,800 Bytes |
| **Max Overflow Doc Size** | Limited by File System (~17 TB) |
| **Lookup Latency** | $O(1)$ (Pointer Offset) |
| **Compaction Cost** | $O(\text{Slots per Page})$ |

## 7. Consistency and Persistence
Slotted Page updates are 100% **ACID-Compliant**.
*   **Shadow Writes:** Compaction and insertions occur on shadow pages.
*   **Atomic Swap:** The new page layout only becomes "Sovereign" when the transaction commits, ensuring that a crash during compaction never results in a corrupted slot array.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Architecture:** Slotted Page / Bidirectional Growth  
**Subsystem:** Sovereign Object Storage (SOS)
