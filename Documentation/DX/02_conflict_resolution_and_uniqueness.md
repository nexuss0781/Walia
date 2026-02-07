# Conflict Resolution and Enforced Uniqueness

## 1. The Global Uniqueness Principle
To achieve a truly zero-config, "import-free" developer experience, Walia enforces a strict architectural rule: **Global Symbol Uniqueness**. In a Walia project, no two top-level symbols (functions or classes) may share the same name, regardless of which file they reside in. 

While this may seem restrictive to developers coming from legacy languages like C++ or Python, it is the fundamental mechanism that allows Walia to resolve logic at hardware speeds and ensures that the codebase remains readable and refactorable at any scale.

## 2. The "Struggle" (Interactive Resolution)
When the **Hash-Sentry** identifies a name collision (e.g., `fun init()` is defined in both `auth.wal` and `db.wal`), the engine halts execution immediately. It initiates a process known as the "Struggle"—an interactive session where the developer must reconcile the namespace conflict before the Sovereign Registry can be finalized.

### The Resolver Interface
The CLI transforms into a decision portal, presenting the conflicting logic nodes side-by-side with their respective file paths, line numbers, and SHA-256 hashes.

```text
[!] SYMBOL CONFLICT: 'process_data'
----------------------------------------------------------
1. Existing: src/logic/core.wal   (Line 45) [Hash: a1b2...]
2. Incoming: src/utils/helper.wal (Line 12) [Hash: c3d4...]
----------------------------------------------------------
Select an action:
  [1] Rename existing symbol
  [2] Rename incoming symbol
  [3] Auto-Resolve (Context-Aware)
  [4] Manual Review (Open in Editor)
```

## 3. Intelligent Auto-Resolution
The Walia Sentry provides a "UFO-grade" auto-resolution feature for high-velocity development. When selected, the engine applies **Context-Aware Renaming**.

*   **Heuristic:** The engine appends a sanitized version of the filename to the symbol.
*   **Example:** If `init()` in `network.wal` conflicts, the engine proposes `init_network()`.
*   **Refactoring:** Once a name is chosen, the **Healer Engine** (Phase 10.2) automatically walks the project and updates all callsites to match the new unique identity, ensuring the project remains executable without manual intervention.

## 4. The Editor Bridge
For complex conflicts where a simple rename is insufficient, Walia provides a direct bridge to the system's terminal editor (e.g., `vi`, `nano`). 

*   **Logic:** The CLI suspends the VM, opens the conflicting file at the exact line number of the fault, and waits for the developer to perform a manual refactor.
*   **Verification:** Upon saving and closing the editor, the Sentinel re-scans the file instantly to verify that the uniqueness constraint is now satisfied.

## 5. Explicit Overrides (The Local Loophole)
In rare cases where global uniqueness must be temporarily bypassed (e.g., during a large-scale migration), a developer may use an **Explicit Import**.

```javascript
import "legacy/auth" as local_auth;
```

By explicitly aliasing a file, the compiler ignores the Global Registry for those specific calls and prioritizes the local alias. This acts as a safety valve for enterprise legacy integration while keeping the primary project namespace clean.

## 6. Enterprise Benefits
*   **Navigational Clarity:** Developers never have to ask, "Which `User` class is this?" There is only one `User` class in the entire sovereign environment.
*   **Refactoring Safety:** Because every name is unique, "Search and Replace" becomes a 100% safe operation. There is no risk of accidentally renaming a function in a different module.
*   **Build Integrity:** The Registry acts as a permanent firewall against the "Dependency Hell" common in traditional software stacks.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**Enforcement:** Hard-Halt on Collision  
**Tooling:** Interactive TUI Resolver  


