
# Tier V: The Systems Commander
## Module 64: Register Variable Mapping

### 1. The Fastest Pockets in the Machine
In previous modules, we learned that variables live in the system's memory. However, at the very heart of the computer sits the CPU, which contains a tiny set of high-speed storage slots called **Registers**. These are the "Fastest Pockets" in the machine—data stored in a register can be processed in a single nanosecond.

To achieve the ultimate level of systems performance, Walia allows you to bind a variable directly to a physical CPU register. This ensures that the data never touches the RAM, staying inside the silicon for immediate execution.

### 2. The `@reg` Attribute
Walia provides the `@reg` attribute to link a variable to a specific hardware register name. Once linked, any modification to that variable physically moves the bits into that specific part of the CPU.

*   **Syntax:** `@reg("register_name") var variable_name: type;`
*   **Context:** This is a hardware-direct operation and must be performed within an **`unsafe`** block.

```Walia
unsafe {
    // Example 1: Binding to the Primary Accumulator (x86_64)
    @reg("rax") 
    var cpu_accumulator: u64;
    
    cpu_accumulator = 1000; 
    // The value 1000 is now physically sitting in the 'rax' register.
}
```

### 3. Communicating with Assembly
The primary use of register mapping is to prepare data for an `asm` block. In standard languages, passing data to assembly is complex; in Walia, it is as simple as assigning a value to a variable.

```Walia
// Example 2: Passing data into a machine code operation
unsafe {
    @reg("rdi") var destination: ptr;
    @reg("rsi") var source: ptr;

    destination = 0xB8000; // VGA Buffer address
    source = my_data_ptr;

    asm {
        "movsq" // A single instruction to move 8 bytes between rsi and rdi
    }
}
```

### 4. Capturing Hardware Results
Register mapping works in both directions. You can use it to capture the result of a hardware instruction or a system call directly into your Walia logic.

```Walia
// Example 3: Capturing a CPU result
unsafe {
    @reg("rax") var result: i64;

    asm {
        "rdtsc" // Read Time-Stamp Counter (CPU Clock ticks)
    }

    print "The CPU has ticked " + result + " times since boot.";
}
```

### 5. Architectural Rule: The Volatile Register
Registers are shared by everything running on the CPU.
*   **The Law of Transience:** Unlike standard Walia variables, registers are **Not Persistent**. They are volatile and can be overwritten by the next instruction.
*   **The Sovereign Strategy:** Always map your variables to registers immediately before the `asm` block and read from them immediately after. Do not expect a register to hold its value across a long-running loop unless you are building a dedicated kernel driver.

### 6. Mapping across Architectures
Walia's intelligent compiler understands different CPU layouts. If you are on an ARM device (like a phone in Termux), you use the ARM register names.

| Architecture | Example Registers | Purpose |
| :--- | :--- | :--- |
| **x86_64** | `rax`, `rdi`, `rsp`, `rip` | Intel/AMD Standard |
| **ARM64** | `x0`, `x1`, `sp`, `pc` | Mobile/Cloud Standard |

### 7. Visualizing the CPU: The Command Nexus
To verify your register bindings, watch the **Command Nexus HUD**:
*   **Execution Lanes:** You will see a "Register Lock" indicator. This shows that the VM has reserved a physical CPU register for your specific variable.
*   **Telemetry:** The IPS (Instructions Per Second) counter will spike, as register-to-register math is the fastest possible operation a computer can perform.

### 8. Practical: The High-Speed Data Handshake
Let's build a logic structure that prepares two registers for a direct kernel interaction.

1.  **Define the Register Bindings:**
    ```Walia
    unsafe {
        @reg("rdi") var arg1: u64;
        @reg("rsi") var arg2: u64;
    }
    ```
2.  **Perform the Handshake:**
    ```Walia
    unsafe {
        arg1 = 0x01; // Logic A
        arg2 = 0xFF; // Logic B
        
        print "Registers RDI and RSI are physically primed.";
        
        asm {
            "nop" // Placeholder for your custom machine code
        }
    }
    ```

Notice how clean the interface is. You are writing standard Walia code, but you are controlling the physical state of the processor.

### 9. Summary
1.  **`@reg`**: The attribute used to bind variables to physical CPU slots.
2.  **Zero-Latency:** Register-mapped variables avoid the "Memory Wall" by staying inside the CPU.
3.  **Assembly Bridge:** The primary method for passing data into and out of `asm` blocks.
4.  **Hardware Awareness:** Requires knowledge of the target architecture (rax/x0).

---
**Next Step:** Registers are the path to the kernel. In the next module, we will explore **Direct System Calls (`syscall`)**, learning how to use these registers to request services directly from the Operating System kernel without any intermediate software.
