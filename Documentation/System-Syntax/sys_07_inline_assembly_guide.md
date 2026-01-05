
# Bare Metal Bridge: Inline Assembly

## 1. The Ultimate Escape Hatch
Walia is a high-level language, but Operating Systems sometimes need instructions that no language provides (like `CLI` to disable interrupts or `CPUID` to query hardware capabilities). The `asm` block allows you to inject raw machine code directly into the compiled stream.

Walia includes a multi-architecture **Sovereign Assembler** (Phase 11.4) that parses these blocks at compile-time and emits optimal binary blobs.

## 2. Syntax Structure
```javascript
asm {
    "mnemonic operand1, operand2"
    "mnemonic operand1"
}
```
*   **Safety:** `asm` blocks are inherently unsafe. They can crash the VM, corrupt memory, or halt the CPU. They must be wrapped in `unsafe { ... }` blocks (future feature) or carefully audited.
*   **Persistence:** The assembled code blob is stored in the `.wld` heap and marked executable.

## 3. Architecture Awareness
Code written in `asm` is specific to the CPU. Walia allows you to provide implementations for both x86_64 and ARM64 in the same file using Preprocessor Directives (future phase) or runtime checks.

### x86_64 Examples
```javascript
asm {
    "mov rax, 60"  // Syscall number for exit
    "mov rdi, 0"   // Status code 0
    "syscall"      // Invoke kernel
}
```

### ARM64 Examples
```javascript
asm {
    "mov x8, 93"   // Syscall number for exit
    "mov x0, 0"    // Status code 0
    "svc #0"       // Supervisor Call
}
```

## 4. The "Volatile" Keyword
By default, Walia's optimizer might move code around. If your assembly interacts with hardware state that changes outside the program flow (like an interrupt flag), you must mark the variable as `volatile`.

```javascript
// Reading a hardware clock register
var time: u64;
asm {
    "rdtsc"
    "shl rdx, 32"
    "or rax, rdx"
    "mov [time], rax" // Hypothetical binding syntax
}
```

## 5. Register Binding (Future Scope)
In future versions (Phase 13+), Walia will support binding Walia variables directly to CPU registers inside `asm` blocks.
```javascript
var count: u64 = 10;
asm(count => "rcx") {
    "loop_start:"
    "dec rcx"
    "jnz loop_start"
}
```
For Phase 11/12, we focus on pure instruction injection.

## 6. Industrial Use Cases
*   **Context Switching:** Saving/Restoring CPU registers during thread swaps.
*   **Hardware Initialization:** Setting up the GDT (Global Descriptor Table) or IDT (Interrupt Descriptor Table) during OS boot.
*   **Performance Loops:** Writing hand-tuned SIMD loops for cryptography or video decoding that beat the compiler's auto-vectorization.

---
**Status:** Enterprise Specification 1.2  
**Architecture:** x86_64 / ARM64 Native  
**Risk Level:** Critical (Unchecked Execution)  
