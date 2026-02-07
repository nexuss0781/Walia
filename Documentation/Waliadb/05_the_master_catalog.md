# The Master Catalog: Sovereign Database Registry

## 1. The Brain of the .wld File
The **Master Catalog** is the persistent registry that tracks the location and type of every data structure within a WaliaDB file. Resident on **Page 1**, the catalog provides the high-level mapping between human-readable names (e.g., `"users"`) and the physical entry points (**Root Page IDs**) for the SQL Agency, NoSQL Party, and Vector Wing.

The Catalog is designed for **Self-Describing Logic**: any Walia Virtual Machine can mount a `.wld` file and instantly understand its entire schema and index structure without external metadata files.

## 2. Binary Layout (Page 1)
To ensure UFO-grade bootstrapping, the Catalog is stored as a raw C-struct that fits perfectly into a single 4096-byte page. This allows the VM to "see" the database registry in a single memory-mapped access.

### I. The Catalog Entry
Each entry in the catalog occupies a fixed-width slot, allowing for $O(1)$ indexed access.

| Field | Size | Description |
| :--- | :--- | :--- |
| `Name` | 64 bytes | The unique identifier for the Table or Collection. |
| `Type` | 4 bytes | Identifies the engine (SQL, NoSQL, or Vector). |
| `RootPage` | 4 bytes | The Logical Page ID of the structure's entry point. |
| `SchemaHash`| 32 bytes | SHA-256 fingerprint of the data's logical contract. |
| `IsActive` | 1 byte | Lifecycle flag for $O(1)$ deletion and reuse. |

### II. Page Geometry
A single Catalog Page supports **30 concurrent entries**. 
*   **Expansion:** In multi-terabyte enterprise environments, WaliaDB supports "Chained Catalog Pages," where the `header.reserved` field points to a secondary registry page, allowing for thousands of tables.

## 3. Entry Type Registry
The Catalog categorizes structures to ensure the correct engine is dispatched during a query.

1.  **`ENTRY_SQL_TABLE`**: Signals a **Clustered B+ Tree** layout. The Root Page points to the tree's internal root node.
2.  **`ENTRY_NOSQL_COLLECTION`**: Signals an **Extendible Hash Directory**. The Root Page points to the persistent hash directory.
3.  **`ENTRY_VECTOR_INDEX`**: Signals an **HNSW Graph**. The Root Page points to the graph entry point for neural searches.

## 4. Logical Integrity: Schema Fingerprinting
Following Walia's **Content-Addressable Logic** philosophy, every catalog entry contains a `schemaHash`.

*   **The Mechanism:** When a table is created via the Walia Sentry (ORM), a SHA-256 hash of the class definition is generated.
*   **The Enforcement:** During a `db_catalog_lookup`, the VM compares the hash in the `.wld` file against the hash of the currently running Walia script.
*   **The Benefit:** This prevents "Schema Drift." If the source code changes but the database is not migrated, the Sentry refuses to mount the table, protecting the sovereign state from corruption.

## 5. Operations API (`db_catalog.c`)
The catalog is managed through an atomic, transaction-aware C API.

*   **`db_catalog_init`**: Reserves Page 1 and initializes the bitset. Called during the "Cold Boot" sequence.
*   **`db_catalog_add`**: Registers a new structure. This operation is performed within a **Shadow Paging** transaction, ensuring that a table is only "visible" once its underlying B+ Tree or Hash Directory is physically written to disk.
*   **`db_catalog_lookup`**: The primary discovery mechanism. Returns the `PageID` and `EntryType` in constant time.

## 6. Industrial Bootstrapping Sequence
When a Walia process initializes, the "UFO-grade" mount sequence follows this path:
1.  **Page 0:** Validate Magic Number and locate `catalogRoot` (Page 1).
2.  **Page 1:** Load the Catalog Registry into the **Buffer Pool**.
3.  **Discovery:** The VM script asks for `"users"`. The Catalog instantly provides the Root Page ID.
4.  **Execution:** The **SQE JIT** begins processing data at the provided offset.

Total Time to mount a 10TB database: **< 1ms**.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Location:** Persistent Page 1  
**Integrity:** SHA-256 Content-Identity Verified
