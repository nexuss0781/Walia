# Native Module Interface (FFI)

## 1. The Sovereign Bridge
While the Walia language provides a high-level, persistent, and effect-managed environment, it is designed to leverage the raw performance of C for system-level tasks. The **Foreign Function Interface (FFI)** is the official bridge that allows the Walia Virtual Machine to invoke native C logic with near-zero overhead.

Every "Battery" in the Walia Standard Library (Collections, I/O, Math) is implemented via this interface, ensuring that the language remains lean while its capabilities are virtually unlimited.

## 2. The Native Function Signature
All C functions intended for consumption by the Walia VM must adhere to a standardized function pointer signature defined in `object.h`.

```c
typedef Value (*NativeFn)(int argCount, Value* args);
```

*   **`argCount`:** An integer representing the number of arguments passed by the Walia script.
*   **`args`:** A pointer to the first register in the **Register Window** containing the arguments.
*   **Return Value:** Must return a valid `Value` (NaN-Boxed). If no return is intended, the function must return `NIL_VAL`.

## 3. Register Windowing in FFI
Walia’s FFI is uniquely efficient because it does not copy data between the script and C. When a native function is invoked via `OP_CALL`, the `args` pointer passed to C is a direct pointer into the **VM Stack**.

*   **Input:** `args[0]` corresponds to the first argument in the script call.
*   **Safety:** Native functions have read/write access to their register window. However, they must not access indices beyond `argCount - 1` to prevent stack corruption.

## 4. Registering Native Symbols
To expose a C function to the Walia runtime, it must be registered into the **Global Variable Table**. Walia provides the `defineNative` utility to handle this process safely.

```c
void defineNative(const char* name, NativeFn function) {
    // 1. Intern the name string
    ObjString* string = copyString(name, (int)strlen(name));
    push(OBJ_VAL(string)); // Rooting for GC safety

    // 2. Wrap the C pointer in a Walia Object
    ObjNative* native = newNative(function);
    push(OBJ_VAL(native)); // Rooting for GC safety

    // 3. Store in the Sovereign Global Table
    tableSet(&vm.globals, string, OBJ_VAL(native));

    pop(); pop(); // Unroot
}
```

## 5. Memory Management & GC Safety
Native functions operate in a "Managed Environment." Developers must adhere to two critical rules to ensure the integrity of the persistent heap:

### I. GC Rooting (The Push/Pop Rule)
If a native function allocates a new Walia object (e.g., creating a new `List` or `String`), and then performs another action that *might* trigger a Garbage Collection cycle, the newly allocated object must be **Rooted**.
*   **Action:** Call `push(OBJ_VAL(my_new_obj))` immediately after allocation.
*   **Result:** The GC treats the VM stack as a root and will not reclaim the object during the call.

### II. Write Barriers
If a native function modifies an existing heap object (e.g., updating a Map entry), it **must** invoke the write barrier:
```c
markCard(modified_object_ptr);
```
Failure to call `markCard` will result in the modification being lost during the next atomic checkpoint to the `.state` file.

## 6. Implementation Example: `math_sqr`
```c
static Value nativeSqr(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) {
        return NIL_VAL; // Production error handling
    }
    double n = AS_NUMBER(args[0]);
    return NUMBER_VAL(n * n);
}
```

## 7. Performance Characteristics
Calling a Native Function in Walia is computationally equivalent to a standard function call. There is no "Marshaling" or "Serialization" of data. The Walia `Value` is passed directly, and the C code interprets the bits natively, making Walia an ideal language for wrapping high-performance C++ or Rust libraries.

---
**Status:** Technical Specification 1.0  
**Interface:** Zero-Copy C-Bridge  
**Constraint:** Explicit GC Rooting & Write Barriers  

