
# Tier V: The Systems Commander
## Module 62: High-Precision Bitfields

### 1. Granular Control: Below the Byte
In standard data modeling, the smallest unit we address is the **Byte** (8 bits). However, the physical hardware of a 5th Generation computer often operates at a much more granular level. A hardware status register might use only 1 bit for "On/Off" or 4 bits for a "Mode" setting.

To capture this reality, Walia provides **High-Precision Bitfields**. This feature allows you to define variables that occupy a specific number of *bits* within a `layout`. By mastering bitfields, you can interact with hardware ports and compact network protocols with surgical precision, ensuring that not a single bit of your persistent storage is wasted.

### 2. Bitfield Syntax
Inside a `layout` or `union`, you define a bitfield by following the type with a second colon and the desired **Bit-Width**.

*   **Syntax:** `var name: type : width;`
*   **The Container:** The `type` (e.g., `u8`, `u16`) determines the "Bucket" the bits live in.
*   **The Width:** The `width` determines how many bits are assigned to that variable.

```Walia
// Example 1: A 1-byte control block with 4 distinct settings
layout HardwareControl {
    var enabled:    u8 : 1; // Uses only 1 bit
    var interrupt:  u8 : 1; // Uses only 1 bit
    var mode:       u8 : 2; // Uses 2 bits (4 possible states)
    var reserved:   u8 : 4; // Uses remaining 4 bits
} // Total size: Exactly 1 byte.
```

### 3. Automatic Bit-Packing
Walia’s compiler implements **Sovereign Packing Logic**. 
*   **The Mechanism:** When the compiler sees consecutive bitfields that use the same base type (like `u8`), it physically "packs" them into the same byte.
*   **Offset Logic:** In the example above, `mode` is physically located at the same memory address as `enabled`, but the engine knows it starts at Bit #2.

### 4. Reading and Writing Bit-Values
When you interact with a bitfield in your code, Walia handles the complexity of **Bit-Masking** automatically. You treat the variable like a standard number, and the engine ensures only the specific bits are modified.

```Walia
unsafe {
    var ctrl: HardwareControl;
    
    // Writing to a bitfield
    ctrl.enabled = true; // Sets Bit 0 to 1
    ctrl.mode = 3;       // Sets Bits 2 and 3 to '11'
    
    // The other bits (interrupt, reserved) remain untouched.
    print ctrl.mode; // Output: 3
}
```

### 5. Use Case: Compressed Network Protocols
Bitfields are the primary tool for implementing high-efficiency network stacks where headers must be as small as possible.

```Walia
layout SmallPacketHeader {
    var version:   u8 : 4;
    var priority:  u8 : 3;
    var is_last:   u8 : 1;
    var payload_id: u8;
} // Total size: 2 bytes
```

### 6. Alignment and Bit-Boundaries
If a bitfield definition exceeds the size of its base type, Walia follows the **Physical Boundary Rule**:
1.  **Wrap-Around:** If you try to put a 5-bit field into a `u8` that only has 3 bits left, Walia starts the field at the beginning of the **Next Byte**.
2.  **Explicit Alignment:** You can force a bitfield to start at a new byte by adding a standard (non-bitfield) variable between them.

### 7. Visualizing Bits: The Command Nexus
To see your bit-level operations in real-time, activate the HUD (`walia --nexus`):
*   **Execution Lanes:** You will see **SIMD Masking** pulses. This indicates the CPU is using hardware bit-masks to isolate your variables during a read or write.
*   **PageMap:** When you modify a 1-bit variable, the HUD tracks the "Micro-Write," showing the physical page update even for a sub-byte change.

### 8. Practical: The Status Register Simulation
Let's build a simulator for a physical hardware status register.

1.  **Define the Register Map:**
    ```Walia
    layout StatusRegister {
        var power_on:   u8 : 1;
        var error_code: u8 : 3;
        var temp_warn:  u8 : 1;
        var fan_speed:  u8 : 3;
    }
    ```
2.  **Manipulate the Hardware State:**
    ```Walia
    unsafe {
        var reg: StatusRegister;
        reg.power_on = true;
        reg.error_code = 0;   // No error
        reg.fan_speed = 7;    // Maximum speed (111 in binary)
        
        print "Status Byte physically initialized.";
    }
    ```

Notice that you are managing 4 different variables within a single 8-bit memory slot. This is the definition of **High-Precision Systems Sovereignty**.

### 9. Summary
1.  **Bitfields:** Variables with sub-byte precision.
2.  **Colon Syntax:** `var name: type : width;`.
3.  **Automatic Packing:** Multiple fields are unified into a single physical byte or word.
4.  **Hardware Precision:** Essential for drivers, registers, and ultra-compact data formats.

---
**Next Step:** We have mastered the organization of memory. Now we need to speak to the engine that drives it. In the next module, we enter the **Bare Metal Interface**, starting with **Inline Assembly Syntax**, where we learn to write raw machine code directly inside Walia logic.
  