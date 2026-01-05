# Compilation Issues Report

## 1. Objective

The primary goal was to modernize the Walia codebase by:
1.  Upgrading the C standard from C99 to C11 in the `Makefile`.
2.  Introducing a new `mempool` feature for memory management.

## 2. Summary of Issues

Upon switching the compiler standard to C11, a cascade of compilation errors emerged that prevented the project from building. Despite numerous attempts to resolve these issues, the build consistently fails, primarily within `compiler.c`. The core problem appears to be a complex and fragile header dependency graph that is not compatible with the stricter type checking and declaration rules of C11.

## 3. Initial Error and Fix

The very first compilation attempt failed due to a simple syntax error in `token.h`:
*   **Error:** A stray comma (`,`) was present at the beginning of the file (`token.h:1:1`).
*   **Fix:** Removing the comma resolved this initial error.

## 4. Persistent Compilation Failures

After fixing the syntax error and updating the `Makefile` to use `-std=c11`, a series of more complex errors appeared. These errors consistently pointed to missing definitions and unknown types, all originating from `compiler.c`.

**Common Errors:**
*   `error: ‘OP_GET_UPVALUE’ undeclared` (and many other opcodes)
*   `error: ‘EXPR_PERFORM’ undeclared` (and many other AST node types)
*   `error: unknown type name ‘PerformExpr’`
*   `error: unknown type name ‘Obj’` (in `vm.h` and other files)

## 5. Unsuccessful Resolution Strategies

The following strategies were employed to fix the compilation errors, none of which were successful.

### Strategy 1: Systematic Header and Dependency Resolution

This multi-step approach aimed to fix the dependency graph logically.
1.  **Added `typedef`s:** `typedef struct Obj Obj;` and `typedef struct ObjString ObjString;` were added to `object.h` to make these valid type names.
2.  **Forward Declarations:** Added forward declarations for `struct Obj` and `struct ObjString` to `value.h` to break a suspected circular dependency between `object.h` and `value.h`.
3.  **Included Headers:** Added `#include "opcode.h"` and `#include "ast.h"` to the top of `compiler.c`.
4.  **Refactored `compiler.h` and `analysis.h`:** Reorganized these headers to correctly define the `Compiler` struct and function prototypes like `resolveLocal` to ensure they were visible where needed.

*   **Outcome:** This logical approach failed. The exact same compilation errors persisted, suggesting the include order was still incorrect or that a more fundamental structural problem existed.

### Strategy 2: Single "Unity Build" File

This approach attempted to bypass the header dependency issues entirely by forcing a specific compilation order.
1.  **Created `walia.c`:** A single C file was created that `#include`d every other `.c` file in the project.
2.  **Updated `Makefile`:** The `Makefile` was modified to compile only `walia.c`.

*   **Outcome:** This failed immediately with new errors, such as `fatal error: test_runner.h: No such file or directory`. The project's source files still rely on headers, and this approach breaks that reliance entirely. It also introduced new symbol redefinition errors.

### Strategy 3: Dependency Management with Conan

This was an attempt to see if a missing external dependency was the cause.
1.  **Created `conanfile.py`:** A configuration file for the Conan package manager was created, specifying a dependency on `openssl/3.2.1`.
2.  **Installed Dependencies:** Ran `conan install` to fetch and configure the library.

*   **Outcome:** This had no effect. The compilation errors are internal to the project's structure, not related to external libraries.

## 6. Conclusion

The Walia codebase has a tightly coupled and non-trivial header dependency structure that is not robust enough for a simple migration to the C11 standard. The stricter rules of C11 expose these underlying issues, leading to a cascade of compilation failures that are not easily resolved by simple fixes.

To successfully modernize this project, a more significant refactoring effort is required. This would involve a careful audit of the entire include graph and a restructuring of the header files to ensure that all types and functions are declared before they are used, without creating circular dependencies.