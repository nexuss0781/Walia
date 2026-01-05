
# Physical Data Modeling: Layouts

## 1. The Shape of Data
In high-level languages, a "Class" is a collection of behaviors and references. In Systems Programming, data has a physical shape—a specific sequence of bytes required to talk to hardware or network protocols. Walia introduces the `layout` keyword to define these rigid physical structures.

A `layout` is not an object. It is a **Template** that tells the compiler how to interpret a block of raw memory.

## 2. Defining a Layout
Layouts use the same syntax as classes but enforce strict typing on fields.

```javascript
layout EthernetHeader {
    dest_mac:   u8[6]; // Fixed-size array (future phase)
    src_mac:    u8[6];
    ethertype:  u16;
}
```

### The "Packed" Guarantee
Walia guarantees that the fields appear in memory in the exact order declared.
*   `dest_mac` is at offset 0.
*   `src_mac` is at offset 6.
*   `ethertype` is at offset 12.

## 3. Hardware Alignment (Padding)
CPUs crash or slow down if you try to read a `u32` from an odd address. Walia automatically handles **Alignment Padding**.

```javascript
layout AlignedExample {
    a: u8;  // Offset 0
    b: u32; // Offset 4 (3 bytes of padding added after 'a')
}
```
**Total Size:** 8 bytes.
**Constraint:** The compiler inserts hidden padding bytes to ensure `b` starts on a 4-byte boundary.

### `packed` Modifier
If you are parsing a network packet where every byte counts, you can disable padding.
```javascript
packed layout NetworkPacket {
    a: u8;
    b: u32; // Starts at Offset 1 (Unaligned)
}
```
*   **Warning:** Accessing unaligned fields may be slower on some CPUs (x86) or cause a crash on others (older ARM). Use with caution.

## 4. Using Layouts with Pointers
Layouts are most powerful when combined with pointers. You can "Cast" a raw buffer into a layout to read it structurally.

```javascript
var buffer: ptr = read_network_card();

// Reinterpret the raw bytes as an Ethernet Header
var header = buffer !as *EthernetHeader;

if (header.ethertype == 0x0800) {
    print "IPv4 Packet Detected";
}
```
This operation costs **Zero CPU Cycles**. No data is copied; the `header` variable is just a typed pointer to the existing buffer.

## 5. Nested Layouts
Layouts can contain other layouts, allowing for complex data modeling.

```javascript
layout IPPacket {
    eth: EthernetHeader; // Embedded struct
    version: u8;
}
```
This is essential for driver development, where hardware registers are often grouped into logical blocks.

## 6. The `offsetof` Operator
For advanced metaprogramming, you can query the physical offset of a field.

```javascript
var offset = offsetof(EthernetHeader, ethertype); // Returns 12
```
This allows you to write generic memory manipulation routines that depend on the physical structure of your data.

---
**Status:** Enterprise Specification 1.2  
**Feature:** C-Compatible Structs  
**Alignment:** Automatic (Default) / Packed (Optional)  
