
# Tier VI: The Grand Convergence
## Module 75: Unit Registry and SI-7 Vectors

### 1. The DNA of Reality
In the previous module, we learned that Walia understands units like `<m>` or `<kg>`. But how does the engine store these concepts internally? To achieve "UFO-grade" performance, Walia does not store units as text. Instead, it uses a mathematical structure called the **SI-7 Dimensional Vector**.

This vector is the "DNA" of every value in your program. It represents the 7 fundamental base units of the physical universe as defined by the International System of Units (SI). Every complex unit, from Volts to Joules, is simply a unique combination of these seven base atoms.

### 2. The SI-7 Base Units
The Walia engine tracks seven independent dimensions. Every variable in a dimensional context carries a hidden 7-slot integer array:

| Index | Dimension | Symbol | Walia Unit |
| :--- | :--- | :--- | :--- |
| **0** | Length | L | `<m>` (Meter) |
| **1** | Mass | M | `<kg>` (Kilogram) |
| **2** | Time | T | `<s>` (Second) |
| **3** | Electric Current | I | `<A>` (Ampere) |
| **4** | Temperature | Θ | `<K>` (Kelvin) |
| **5** | Amount of Substance | N | `<mol>` (Mole) |
| **6** | Luminous Intensity | J | `<cd>` (Candela) |

### 3. Understanding the Dimensional Vector
When you define a unit, Walia represents it as a series of exponents.
*   **Distance (`<m>`):** `[1, 0, 0, 0, 0, 0, 0]`
*   **Time (`<s>`):** `[0, 0, 1, 0, 0, 0, 0]`
*   **Velocity (`<m/s>`):** `[1, 0, -1, 0, 0, 0, 0]` (Length divided by Time)
*   **Acceleration (`<m/s^2>`):** `[1, 0, -2, 0, 0, 0, 0]`

### 4. The Unit Registry (Aliases)
To make your code readable, the **Sovereign Unit Registry** provides aliases for complex, derived units. You can use the standard symbols, and the engine automatically resolves them to their SI-7 base vectors.

```Walia
// Example: The engine knows that a Newton (N) is kg * m / s^2
var force: <N> = 50; 

// Internally, 'force' is tagged with the vector: [1, 1, -2, 0, 0, 0, 0]
```

**Common Industry Aliases:**
*   `<N>` (Newton): Force
*   `<J>` (Joule): Energy
*   `<W>` (Watt): Power
*   `<Pa>` (Pascal): Pressure
*   `<V>` (Volt): Voltage

### 5. Persistent Constants
Because Walia is a persistent ecosystem, these unit definitions are part of the **Sovereign Substrate**. You can define your own custom units for specialized industrial fields, and they will be remembered forever in your project's identity.

```Walia
// Defining a custom unit for a specific project
// (Conceptual syntax for custom registration)
db_define_unit("CustomPower", "<kg*m^2/s^3>"); 
```

### 6. Why SI-7 Matters for AI
For the **Super Being** and **Simulation** phases, the SI-7 vector allows the AI to "Reason" about physics.
*   The AI can look at a new piece of data and identify it as "Energy" based on its vector fingerprint `[2, 1, -2, 0, 0, 0, 0]`.
*   It can automatically derive relationships. If it sees "Force" and "Distance," it knows it can calculate "Work."

### 7. Practical: Inspecting the Dimensional DNA
Let's look at how the engine treats different units as identical vectors.

1.  **Define two different names for the same physics:**
    ```Walia
    var work: <J> = 100;           // Energy (Joules)
    var torque: <N*m> = 100;       // Force * Distance
    ```
2.  **Verify the Vector Identity:**
    ```Walia
    // Because both resolve to [2, 1, -2, 0, 0, 0, 0], they are compatible!
    var total_energy = work + torque; 
    
    print total_energy; // Output: 200 <J>
    ```

Notice that even though the "Names" are different (`J` vs `N*m`), the **Sovereign Sentry** sees the identical SI-7 DNA and permits the operation.

### 8. Summary
1.  **SI-7 Vector:** The internal 7-dimensional representation of all physical units.
2.  **Base Units:** Meters, Kilograms, Seconds, Amperes, Kelvin, Moles, Candela.
3.  **Derived Logic:** Complex units are automatically broken down into their base exponents.
4.  **Unit Registry:** Maps industrial symbols (`N`, `J`, `V`) to their physical DNA.

---
**Next Step:** We have the DNA. Now we need the language to express it. In the next module, we will explore the **Syntax of Angle Bracket Units**, learning how to write complex physical expressions and how Walia handles units in assignments and function calls.
