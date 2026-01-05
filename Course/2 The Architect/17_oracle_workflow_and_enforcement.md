
# Tier II: The Architect
## Module 17: Oracle Workflow and Enforcement

### 1. The Build Gate
When you run `walia build`, the compiler doesn't just look at code. It enters a special **Oracle Phase**.

1.  **Scanning:** It finds every `///` block.
2.  **Sandbox:** It spins up a "Ghost VM" (volatile memory).
3.  **Execution:** It runs every example in the sandbox.
4.  **Verdict:** Only if 100% of examples pass does it generate the binary.

### 2. The Lie Detector (What happens when you fail)
Let's look at the error message Walia generates when documentation lies.

**Source Code:**
```Walia
/// example: Math Test
///   assert(add(1, 1) == 5); // LIE!
fun add(a, b) { return a + b; }
```

**Compiler Output:**
```text
[CRITICAL] Oracle Truth-Violation in 'math.wal'
  >> Function: 'add'
  >> Line: 2
  >> Reason: Assertion failed. Expected true, got false.
  >> Status: Build Aborted. Sovereignty requires Truth.
```

### 3. The "Ghost" Sandbox
You might worry: "If my documentation deletes a file or writes to the DB, will it mess up my project?"
**No.**
The Oracle runs in a **Ghost Executioner**.
*   **Database Writes:** Are redirected to temporary memory.
*   **Networking:** Is mocked or blocked.
*   **Persistence:** Is disabled.

This ensures you can document dangerous functions (like `delete_user`) safely. The Oracle verifies the logic without executing the destructive side-effects.

### 4. Interactive Debugging
If a contract is failing and you don't know why, you can debug it live.
Command: `walia doc add --debug`
This opens the REPL pre-loaded with the failing example code so you can inspect variables and fix the logic.

### 5. Conclusion of Tier II
You have now mastered the **Structure** of Walia.
*   You can create reusable logic with **Functions** and **Closures**.
*   You can model the world with **Classes** and **Traits**.
*   You can enforce integrity with **The Oracle**.

You are ready for the next level. You are ready to become a **Data Sovereign**.

---
**Next Tier:** **Tier III: The Data Sovereign**. We will leave the world of pure logic and enter the world of **Massive Persistence**, **SQL**, and **Time Travel**.
