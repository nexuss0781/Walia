
# Bare Metal Bridge: Direct Syscalls

## 1. Bypassing the Standard Library
Every program typically talks to the Kernel via the C Standard Library (`libc`), which handles `open`, `write`, `fork`, etc. While convenient, `libc` adds bloat and abstraction. A **Sovereign System** talks directly to the kernel.

Walia provides the `syscall` primitive (Phase 11.4), which wraps the CPU's trap instruction (`syscall` on x64, `svc #0` on ARM) into a usable language construct.

## 2. The `syscall` Primitive
```javascript
var result = syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
```
*   **Arguments:** Supports up to 6 arguments (matching Linux convention).
*   **Return:** Returns the kernel status code (usually `0` for success, negative for error).
*   **Mapping:** The VM automatically maps `arg1` -> `RDI` (x64) or `X0` (ARM), etc.

## 3. Defining Syscall Numbers
You must define constants for the syscalls you need, as they vary by architecture.

```javascript
// x86_64 Linux Constants
var SYS_WRITE: u64 = 1;
var SYS_EXIT:  u64 = 60;
var STDOUT:    u64 = 1;

fun print_hello() {
    var msg: *u8 = "Hello Kernel\n";
    syscall(SYS_WRITE, STDOUT, msg, 13);
}

fun exit_prog() {
    syscall(SYS_EXIT, 0);
}
```

## 4. Zero-Copy I/O
Using `syscall` allows for true Zero-Copy I/O.
1.  **Allocate:** Use `alloc()` to get a persistent buffer.
2.  **Read:** Call `sys_read` directly into that buffer pointer.
3.  **Process:** Use the buffer in Walia.
4.  **Write:** Call `sys_write` from that buffer.

Data never moves from the buffer; it is populated by the kernel and read by the CPU in place.

## 5. Error Handling
The kernel returns errors as negative numbers (e.g., `-2` for ENOENT).
```javascript
var fd = syscall(SYS_OPEN, "missing.txt", 0);
if (fd < 0) {
    // Handle error (e.g., File Not Found)
    var errno = -fd;
}
```

## 6. Building a Micro-OS
With `syscall` and `asm`, you have everything needed to implement a standard library from scratch.
*   **Memory:** `mmap` / `brk`
*   **Files:** `open` / `close` / `read` / `write`
*   **Process:** `fork` / `execve` / `wait`
*   **Network:** `socket` / `bind` / `connect`

This capability is what allows Walia to become the **Sovereign Substrate** for a new Operating System.

---
**Status:** Enterprise Specification 1.2  
**Interface:** Linux Kernel ABI  
**Capability:** libc-Free Execution  
