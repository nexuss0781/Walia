# Walia Oracle: Living Documentation and Executable Truth

## 1. Philosophy: Documentation as a Sovereign Contract
In traditional software engineering, documentation is a secondary artifact that inevitably drifts from the implementation. Walia eliminates "Documentation Rot" by introducing the **Walia Oracle**. In this paradigm, documentation is not merely descriptive; it is a binding **Sovereign Contract**.

Through the use of triple-slash (`///`) comments, Walia treats code examples in the manual as **Mandatory Executable Logic**. If a documentation example fails to run or produces an incorrect result, the Walia compiler identifies it as a "Truth-Violation" and physically blocks the generation of the production binary.

## 2. Syntax: The Executive Triple-Slash
The Oracle Parser specifically scans for the `///` prefix. These lines are extracted during the pre-compilation phase and transformed into **Doc-Tests**.

### I. Structure of a Doc-Snippet
```javascript
/// example: High-speed list aggregation
///   var my_list = List();
///   list_add(my_list, 10);
///   list_add(my_list, 20);
///   assert(list_len(my_list) == 2);
fun process_collection(list) { 
    // implementation...
}
```

*   **Indicator:** Only lines beginning with `///` are processed by the Oracle.
*   **Example Tag:** The `example:` keyword identifies the start of an executable block.
*   **Body:** The indented code following the tag is compiled into a sandboxed Walia function.

## 3. Contextual Binding
The Walia Oracle is **Context-Aware**. It does not treat snippets as isolated text.

1.  **Look-Ahead Analysis:** When the Oracle Lexer finds a `///` block, it performs a look-ahead to the next logical node in the source file (the following `fun` or `class`).
2.  **Logical Linking:** The snippet is mathematically linked to the **SHA-256 Hash** of that logic node.
3.  **Verification:** If the function's logic changes (resulting in a new hash), the Oracle demands a re-verification of the associated documentation to ensure the "Truth" has been updated to match the "Reality."

## 4. The "Truth-or-Death" Build Gate
Walia enforces the accuracy of its manual through a hardware-level build gate. 

*   **The Executioner:** Before production bytecode is finalized, the **Oracle Runner** (Phase 10.4.2) executes every doc-test snippet in the project.
*   **The Sentry:** If any snippet triggers an assertion failure or a runtime error, a `FAULT_CRITICAL` is recorded in the **Diagnostic Ledger**.
*   **The Gate:** The compiler refuses to produce the `.state` image or the `walia` binary until the ledger is clear. 

**Enterprise Guarantee:** A compiled Walia binary is physical proof that the accompanying documentation is 100% accurate.

## 5. Sovereign Proof: The Verified Manual
The Oracle Generator (`tooling_oracle_gen.c`) produces the official **Enterprise Manual** in Markdown format. This manual is more than a guide; it is a **Certificate of Integrity**.

### I. The Proof Badge
Every entry in the generated manual includes a **Sovereign Proof Table**:

| Attribute | Value |
| :--- | :--- |
| **Logic Hash** | `a1b2c3d4e5f6...` |
| **Verification Timestamp** | `2025-05-20 14:30:05` |
| **Runtime Status** | `VERIFIED SOVEREIGN` ✅ |

## 6. Developer Experience (DX) Benefits
*   **Executable Specification:** Developers can write their requirements in the comments and watch the compiler enforce them.
*   **Zero-Drift Knowledge Base:** New team members can trust the manual implicitly, as it is physically impossible for a Walia manual to be out of date.
*   **Automatic Test Generation:** The Oracle acts as a "Free" unit testing layer, reducing the need for separate test files for basic API surface area.

## 7. Operational Safety
Doc-tests are executed using the **Ghost Executioner** (Phase 10.2).
*   **Sandbox Isolation:** Tests run on a volatile, Copy-on-Write (CoW) heap.
*   **Persistence Protection:** Even if a doc-test performs a `db_save()`, the physical `.wld` file remains untouched. The Oracle verifies the logic of the save operation without contaminating production data.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**System:** Walia Oracle (Truth Sentry)  
**Output:** `Documentation/sovereign_manual.md`

