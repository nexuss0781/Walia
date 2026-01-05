
# Tier III: The Data Sovereign
## Module 27: Security - Login and Context Switching

### 1. The Execution Context
In Lesson 26, we defined the "Laws of Access" by creating Roles and Permissions. However, code does not run in a vacuum. Every instruction executed by the Walia Virtual Machine happens within a **Sovereign Context**. 

Think of the context as an "Identity Card" that the CPU carries. When your code attempts to access a persistent table, the engine looks at this card to see if you have the right to proceed. This module teaches you how to manage that identity through **Context Switching**.

### 2. The `db.login()` Primitive
To assume a specific role, you use the `db.login` function. This is the official gateway to your data territories.

```Walia
// Syntax: db.login(role_name, secret_key)
var success = db.login("Admin", "sovereign_pass_99");

if (success) {
    print "System identity elevated to: Admin";
} else {
    print "Authentication failed. Access denied.";
}
```

**How it works:** 
1.  Walia hashes your provided key.
2.  It compares it against the cryptographic fingerprint stored on **Page 3** of the database.
3.  If they match, the VM's internal "Access Token" is updated instantly.

### 3. Immediate Context Switching
Walia is designed for high-concurrency environments. You can switch identities multiple times within a single script. This is essential for building servers or multi-user applications where the "Identity" of the logic changes based on who is making the request.

```Walia
// Operating as Guest
print db.PublicData.find(d => true); 

// Elevating to Staff
db.login("Staff", "staff_key");
db.InternalData.save("report_01", {"status": "ok"});

// Reverting to Guest (Logout)
db.logout();
```

### 4. Zero-Overhead Security Checks
Traditional security systems slow down as you add more rules. Walia uses **Hardware-Level Dispatch**.

Because permissions are stored as bitmasks (Module 26), the engine performs a "Bitwise AND" operation whenever you access data. This check happens in a **Single CPU Cycle**. Whether you have one security rule or one million, Walia validates your context at the absolute physical speed of the processor.

### 5. Persistent Identity (Warm Resumption)
Because Walia features **Orthogonal Persistence**, your login state is part of the system's "Soul."

*   **Scenario:** You log in as "Admin" and start a heavy data-processing task.
*   **Event:** The power is disconnected mid-process.
*   **Resumption:** When the system reboots, Walia restores the **Sovereign Context**. Your code resumes exactly where it was, and it is **still logged in**. You don't need to re-authenticate to finish a persistent task.

### 6. The "Sentry" Guard: Implicit Fail-Safe
If your context does not have permission for a table, Walia doesn't just return an empty list—it physically prevents the memory from being mapped.

```Walia
// Assume we are NOT logged in as Admin
var secret_info = db.RestrictedTable.find(x => true);

// RESULT: The Sentry intercepts the call. 
// 'secret_info' becomes 'nil' and a security pulse is emitted 
// to the Command Nexus HUD.
```

### 7. Practical: The Multi-User Logic Portal
Let's build a secure gateway that handles two different identities.

1.  **Define a Secure Function:**
    ```Walia
    fun access_vault(key) {
        if (db.login("Guardian", key)) {
            print "Vault Opened. Accessing gold reserves...";
            var gold = db_find_sql("Treasury", 1);
            print "Current Reserve: " + gold.amount;
            db.logout();
        } else {
            print "Intruder Alert! Physical Lockdown Initiated.";
        }
    }
    ```
2.  **Execute the Test:**
    ```Walia
    access_vault("wrong_key"); // Output: Intruder Alert!
    access_vault("correct_key_123"); // Output: Vault Opened.
    ```

Notice that the security is not a separate "plugin"—it is a native capability of the Walia language. By mastering **Context Switching**, you ensure that your persistent data remains under absolute control, even in a globally distributed environment.

---
**Next Step:** Security protects the present. But Walia can do more—it can see the past. In the next module, we will explore **Temporal Snapshots**, learning how to use `db.at()` to query your data as it existed hours, days, or years ago.
