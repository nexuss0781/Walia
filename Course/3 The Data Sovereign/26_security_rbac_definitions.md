
# Tier III: The Data Sovereign
## Module 26: Security and Role-Based Access Control (RBAC)

### 1. The Zero-Trust Substrate
In traditional applications, security is often a high-level "if" statement: `if (user.is_admin) { ... }`. This is fragile; if a developer forgets a check, the data is exposed. Walia implements **Sovereign Security**.

In our architecture, security is enforced by the **Sovereign Pager**. It is physically impossible to map a data page into memory unless your current execution context has the correct permission bits. We call this **Zero-Trust Memory Isolation**.

### 2. Defining Roles
Security begins with defining **Roles**. A Role is a persistent identity in the `walia.registry` that carries specific permissions.

```Walia
// Logic to define a new sovereign role
// This creates an entry on logical Page 3 of the .wld file
db_define_role("Admin", "SHA256_HASH_OF_KEY");
db_define_role("Guest", "PUBLIC_ACCESS");
```

### 3. Permission Bitmasks
Walia utilizes a high-speed bitmask system to define what a role can do. There are three primary permission levels:

| Level | Bit | Effect |
| :--- | :--- | :--- |
| **READ** | `0x01` | Allows the VM to read the data from disk. |
| **WRITE** | `0x02` | Allows the VM to update or insert records. |
| **ADMIN** | `0x04` | Allows the role to change permissions and schemas. |

### 4. Assigning Permissions to Tables
You assign permissions directly to your `@sql` or `@nosql` classes.

```Walia
// Restrict the 'Finance' table to the 'Admin' role
db_grant("Finance", "Admin", 0x01 | 0x02); // READ + WRITE

// Allow everyone to read the 'PublicCatalog'
db_grant("PublicCatalog", "Guest", 0x01); // READ ONLY
```

### 5. Hardware-Level Enforcement
When you write `var data = db_find_sql("Finance", 100)`, the following occurs:
1.  The VM checks the **Sovereign Context** for your `activeRole`.
2.  The Pager looks up the permission bits for that role on Page 3.
3.  If the bit is missing, the Pager returns a `null` pointer.
4.  **The Result:** The logic fails safely. Your code cannot even "see" the bits of the restricted data. This protects you against "Side-Channel" attacks and logic errors.

### 6. Managing the Sovereign Keys
Every role is protected by a **Sovereign Key** (a cryptographic hash).
*   **Persistent Integrity:** These keys are stored in the `.wld` file itself. 
*   **Self-Contained:** Your database doesn't need an external authentication server. The security is built into the physical storage format.

### 7. Practical: Creating a Multi-Tenant System
Let's define a system with restricted access.

1.  **Define the Tables:**
    ```Walia
    @sql class PublicInfo { var id; var msg; }
    @sql class SecretData { var id; var data; }
    ```
2.  **Initialize Security:**
    ```Walia
    db_define_role("Staff", "staff_key_123");
    
    // Staff can read and write SecretData
    db_grant("SecretData", "Staff", 0x03); 
    
    // Everyone can read PublicInfo
    db_grant("PublicInfo", "Guest", 0x01);
    ```

You have now established the "Rules of Engagement" for your system. Even if a malicious script attempts to access `SecretData`, the physical hardware (via the Pager) will refuse to yield the data unless the correct role is active.

---
**Next Step:** Defining roles is only half the battle. In the next module, we will learn how to use **Login and Context Switching** to assume these roles safely within your code.
