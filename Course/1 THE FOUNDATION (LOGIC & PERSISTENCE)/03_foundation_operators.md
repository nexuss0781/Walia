# Lesson 03: Operators and Expressions

## 1. The Engine of Logic
In Walia, an **Expression** is any piece of code that produces a value. Operators are the symbols we use to combine, compare, and transform those values. 

Because Walia is a persistent language, operators don't just calculate numbers—they can establish permanent relationships between different parts of your system's memory.

## 2. Standard Arithmetic
Walia supports standard mathematical operations on numbers.

| Operator | Action | Example |
| :--- | :--- | :--- |
| `+` | Addition | `10 + 5` |
| `-` | Subtraction | `20 - 7` |
| `*` | Multiplication | `4 * 3` |
| `/` | Division | `100 / 4` |

```Walia
var total = 100 + (50 * 2); // Result: 200
```

## 3. Comparison and Truth
Comparison operators check the relationship between two values and return a **Boolean** (`true` or `false`).

| Operator | Action | Example |
| :--- | :--- | :--- |
| `==` | Equality | `x == 10` |
| `!=` | Inequality | `x != 5` |
| `>` | Greater Than | `price > 100` |
| `<` | Less Than | `age < 18` |

### Logical Operators
Use these to combine multiple truths:
*   **`and`**: Returns true if both sides are true.
*   **`or`**: Returns true if at least one side is true.
*   **`!`**: Inverts a boolean (Not).

```Walia
var is_valid = (age > 18) and (has_id == true);
```

## 4. The Entanglement Operator (`~=`)
This is one of Walia's most advanced features. The **Entanglement Operator** creates a live, mathematical bond between two variables. 

In standard languages, if you say `a = b + c`, and `b` changes later, `a` stays the same. In Walia, if you use the entanglement operator, the values remain linked forever.

```Walia
var width = 10;
var height = 20;

// Entangle area with the product of width and height
var area ~= width * height;

print area; // Output: 200

width = 50; 
print area; // Output: 1000 (Updated automatically!)
```
**Sovereign Property:** Because this is a top-level relationship, this entanglement is **Persistent**. If you close your application and reopen it, changing `width` will still automatically update `area`.

## 5. The Pipe Operator (`|>`)
Walia introduces the **Pipe Operator** to manage the flow of data. It allows you to pass the result of one operation directly into the next, creating a clean "pipeline."

```Walia
// Without Pipes (Hard to read)
var result = process(filter(load_data()));

// With Pipes (Clean, logical flow)
var result = load_data() 
          |> filter() 
          |> process();
```
The result of the expression on the left is automatically passed as the first argument to the function on the right.

## 6. Bit-Level Manipulation (Systems Logic)
For situations where you need to talk directly to hardware or perform low-level encryption, Walia provides operators that manipulate the individual bits of a number.

*   `&` (And)
*   `|` (Or)
*   `^` (Xor)
*   `<<` (Left Shift)
*   `>>` (Right Shift)

```Walia
var flags: u8 = 0b00001010;
var mask:  u8 = 0b00001111;
var result = flags & mask; // Bitwise comparison
```

## 7. Practical: The Reactive System
Let's build a persistent, reactive logic chain.

1.  Open the Walia terminal.
2.  Define your source data:
    ```Walia
    var base_price = 500;
    var tax_rate = 0.15;
    ```
3.  Entangle the final price:
    ```Walia
    var final_price ~= base_price * (1 + tax_rate);
    ```
4.  Verify the result:
    ```Walia
    print final_price; // 575.0
    ```
5.  Change the tax rate and observe the "Ripple Effect":
    ```Walia
    tax_rate = 0.20;
    print final_price; // 600.0
    ```

Notice how the logic is "alive." You are no longer just writing instructions; you are defining the rules of a persistent universe.

---
**Next Step:** Proceed to **Lesson 04: Control Flow**, where we will learn how to guide the "River of Thought" through decisions and loops.