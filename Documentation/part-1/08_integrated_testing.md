# Integrated Testing

## 1. Testing as a Language Primitive
In Walia, testing is not an external dependency or a third-party library; it is a first-class citizen of the language grammar. The `test` keyword allows developers to embed verification logic directly alongside their production code. This ensures that tests, documentation, and implementation remain a single, synchronized source of truth.

## 2. Syntax and Structure
A `test` block consists of a descriptive string and a body of Walia code.

```javascript
test "mathematical integrity of the add function" {
    var result = add(10, 20);
    assert(result == 30);
}
```

*   **Description:** The string serves as the identifier for the test runner's output.
*   **Isolation:** While `test` blocks live inside `.wal` files, they are ignored during standard execution (production mode) to ensure zero performance overhead.

## 3. The Execution Model
Walia provides two distinct modes of execution via the CLI:

### I. Production Mode (Standard)
```bash
./walia script.wal
```
In this mode, the compiler skips the bytecode generation for `test` blocks. They consume 0 bytes in the compiled chunk and 0 CPU cycles during runtime.

### II. Test Mode
```bash
./walia --test script.wal
```
In test mode, the VM activates the integrated test runner. It identifies all `test` blocks, executes them sequentially, and provides a sovereign report of passes and failures.

## 4. Assertion Logic
Walia provides a native `assert` primitive used exclusively within `test` blocks (or debug builds).

*   **Success:** If the expression evaluates to `true`, execution continues.
*   **Failure:** If the expression evaluates to `false`, the VM captures the current call stack, line number, and the failed expression, reporting it to the test runner before moving to the next test.

## 5. Testing and Persistence
Because Walia is an **Orthogonal Persistence** language, `test` blocks have a unique capability: they can verify the state of the persistent heap.

```javascript
var global_counter = 0;

test "global counter persistence" {
    global_counter = global_counter + 1;
    assert(global_counter > 0);
}
```

*   **State Validation:** Tests can be written to ensure that data structures in the `walia.state` file maintain their integrity across different versions of the code.
*   **Sandboxing:** When running in `--test` mode, the VM can be configured to use a temporary, non-persistent "Shadow Heap" to prevent tests from accidentally corrupting production data.

## 6. Mocking with Algebraic Effects
The combination of Integrated Testing and Algebraic Effects provides a powerful mocking mechanism. Instead of complex "Mocking Frameworks," you simply provide a different handler for an effect during a test.

```javascript
// Production code performs an effect
fun processOrder(id) {
    var price = perform "GetPrice";
    return price * 1.15;
}

// Test block provides a mock handler
test "order processing with mock price" {
    handle "GetPrice" {
        var result = processOrder(123);
        assert(result == 115); // Assumes mock returned 100
    }
}
```

## 7. DX Benefits
*   **Zero-Config:** No need to install `npm`, `pytest`, or `jest`. Just write `test` and run `--test`.
*   **Private Access:** `test` blocks have access to the private scope of the module they are in, allowing for deep unit testing of internal logic that isn't exported to the public API.
*   **Documentation:** Tests serve as executable examples of how a function or object is intended to be used.

---
**Status:** Enterprise Specification 1.0  
**Command:** `walia --test`  

