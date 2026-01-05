# Versioning and Module Safety

## 1. The Sovereign Supply Chain
In traditional programming environments, "dependency hell" and "supply chain attacks" are common risks associated with external package managers (npm, pip). Walia eliminates these vulnerabilities by integrating module management and semantic versioning directly into the language substrate. This approach ensures that dependencies are as persistent and secure as the sovereign code itself.

## 2. Integrated Semantic Versioning
Walia utilizes a **"Lockfile-as-Code"** paradigm. Dependencies and version constraints are declared explicitly within the `.wal` source files, allowing the compiler to resolve and verify the environment without external manifest files.

### I. Import Syntax
```javascript
// Strict version pinning
import "json" at "2.1.0";

// Semantic range (Compatible with 1.x.x)
import "net_utils" ^1.0.0;
```

### II. Multi-Version Coexistence
Unlike traditional runtimes that allow only one version of a library to exist in memory, the Walia Virtual Machine can host **multiple versions of the same module simultaneously**. 
*   **Isolation:** Function calls are resolved to the specific version requested by the caller.
*   **Memory Efficiency:** Because Walia uses **Content-Addressable Logic**, shared functions between different versions are deduplicated in the persistent heap, saving RAM and storage.

## 3. Module Safety and Boundaries
Walia modules are isolated units of logic. By default, all variables and functions in a module are private unless explicitly exported.

*   **Sovereign Encapsulation:** A module cannot modify the global variables of another module unless they are shared via a persistent root.
*   **Namespace Integrity:** Modules prevent name collisions by creating distinct lexical environments, which are themselves first-class, persistent objects in the heap.

## 4. Hash-Based Verification
To prevent supply chain hijacking, Walia utilizes cryptographic hashes (SHA-256) to verify the integrity of imported code.

1.  **Normalization:** The compiler normalizes the AST of a module.
2.  **Hashing:** A mathematical fingerprint is generated.
3.  **Validation:** If the content of a library changes (even a single comment if configured for strictness), the hash changes.
4.  **Security:** The runtime refuses to execute an import if the hash does not match the local "Sovereign Lock."

## 5. Persistence Integrity (Binary Versioning)
Because the `walia.state` file is a direct image of the VM heap, Walia must ensure that the binary data on disk is compatible with the version of the `walia` runner being executed.

### The Superblock Version Check
During the **Warm Resume** process, the VM performs a two-tier safety check:
1.  **Language Version:** The `version` field in the Superblock must match the runner's version.
2.  **Architecture Check:** The VM verifies that the pointer-width and endianness of the `.state` file match the current hardware (e.g., preventing an x64 state file from loading on an ARM64 runner).

## 6. Upgrading Persistent Code
Walia supports **Hot Code Swapping**, allowing modules to be updated within a persistent environment without losing data.

*   **Logic Redirection:** The VM maintains a level of indirection in its method tables.
*   **Seamless Migration:** When a new version of a function is compiled, the VM updates the pointers in the persistent heap. Existing closures and continuations continue to use the logic they were created with, while new calls use the updated logic.

## 7. Enterprise Reliability
Integrated versioning ensures that a Walia system is **Self-Contained**. Once a script and its dependencies are committed to a `.state` image, that image is an immutable, executable truth that can be deployed across a cluster with the absolute certainty that it will behave identically everywhere.

---
**Status:** Enterprise Specification 1.0  
**Security Model:** Content-Addressable Integrity  
**Versioning:** SemVer 2.0 Native  
