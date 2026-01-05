# Lesson 02: Atoms of Logic (Data Types)

## 1. The 8-Byte Rule
Walia is engineered for extreme efficiency. To maintain a high-speed execution environment, the language follows a core architectural principle: **Every fundamental piece of data occupies exactly 8 bytes (64 bits).**

This is achieved through a structural design called **NaN-Boxing**. By using a single uniform container for all data types, Walia allows the computer to process large arrays of information without the constant need to check the size or shape of individual values. This predictability is what allows Walia to handle massive datasets with minimal energy consumption.

## 2. Fundamental Data Types
Walia provides a precise set of built-in types that serve as the building blocks for all complex logic.

### I. Numbers
All numbers in Walia are stored as 64-bit double-precision values. This covers integers, decimals, and scientific notation.

```Walia
var age = 25;              // Integer
var price = 99.99;         // Decimal
var mass = 5.97e24;        // Scientific notation
```

### II. Booleans
Booleans represent the two states of logic: truth and falsehood.

```Walia
var is_active = true;
var is_complete = false;
```

### III. Nil
The `nil` type represents the intentional absence of a value. Unlike other systems where "nothing" can cause a crash, Walia treats `nil` as a safe, recognized state.

```Walia
var result = nil; 
```

### IV. Strings
Strings are used to represent text. In Walia, text is **Immutable** (cannot be changed after creation) and **Interned**.

```Walia
var name = "Sovereign Engine";
```
**The Efficiency Secret:** If you use the exact same string in multiple places, Walia stores the actual text data only once in the persistent memory. This eliminates redundant data and ensures that your application uses the smallest possible footprint.

## 3. The Vector Type (Neural Foundations)
Because Walia is designed for the modern era of intelligence, it includes the `Vector` type as a core primitive.

```Walia
var brain_state = Vector(1536);
```
A Vector is a hardware-aligned sequence of numbers. While we will explore these in depth during the intelligence modules, remember that a Vector is the physical structure used to store "meaning" or "identity" within the language.

## 4. Declaring Variables
Walia uses the `var` keyword. You do not need to tell Walia what type of data you are storing; the engine identifies it automatically. This is called **Type Inference**.

### Persistent vs. Transient Variables
The location of your variable declaration determines its lifespan:

1.  **Sovereign Globals:** Any variable declared at the very top of your file (outside of any function) is **Persistent**. It is automatically saved to the computer's storage and will be there even after you turn the computer off and back on.
2.  **Local Variables:** Any variable declared inside a block of code (using `{ }`) is **Transient**. It exists only while that specific logic is running and is cleaned up immediately afterward to save RAM.

```Walia
var project_name = "Alpha"; // Global: This is persistent and immortal.

{
    var temp_calculation = 10 + 20; // Local: This vanishes after the '}'
}
```

## 5. Practical: The Persistence Proof
Let's witness how Walia handles your data types across time.

1.  Open your Walia terminal.
2.  Type the following to define different types:
    ```Walia
    var my_score = 100;
    var my_status = "In Progress";
    var my_flag = true;
    ```
3.  Exit the session using `.exit`.
4.  Restart the terminal and check the types:
    ```Walia
    print my_score;  // Returns 100
    print my_status; // Returns "In Progress"
    print my_flag;   // Returns true
    ```

Notice that the "shape" and "value" of your data remained perfectly intact without any manual saving.

---
**Next Step:** Proceed to **Lesson 03: Operators and Expressions**, where we will learn how to combine these atoms of logic to create powerful mathematical and logical relationships.