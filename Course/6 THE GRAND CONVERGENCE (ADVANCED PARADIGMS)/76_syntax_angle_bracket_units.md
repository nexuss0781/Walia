
# Tier VI: The Grand Convergence
## Module 76: Syntax - Angle Bracket Units

### 1. The Language of Measurement
In a Sovereign environment, the syntax must be as intuitive as the physical laws it represents. Walia utilizes the **Angle Bracket (`< >`)** notation to bind physical meaning to data. This syntax is integrated into every layer of the language—from variable declarations to function signatures—ensuring that every part of your system is dimensionally aware.

### 2. Variable Annotations
The most common way to use units is during a variable declaration. By placing a unit tag after the colon, you establish a **Physical Constraint** for that variable.

```Walia
// Syntax: var name: <unit> = value;
var speed: <m/s> = 299792458; // Speed of light
var mass:  <kg>  = 5.972e24;  // Mass of Earth
```

**Sovereign Rule:** Once a variable is constrained to a unit, any future assignment must match that unit’s **SI-7 DNA**. You cannot assign a `<kg>` value to a `<m/s>` variable.

### 3. Composite Unit Syntax
Walia’s parser understands the mathematical relationships between units. You can build complex, derived units using three primary symbols inside the angle brackets:
*   **`*` (Multiplication):** Combines units.
*   **`/` (Division):** Creates rates.
*   **`^` (Exponent):** Defines areas, volumes, or time-squared.

```Walia
// Example: Force is Mass times Acceleration
var gravity: <m/s^2> = 9.81;
var weight:  <kg*m/s^2> = 735; 
```

### 4. Inline Unit Literals
You can attach a unit directly to a number literal. This creates a **Dimensioned Constant** that can be used in expressions without pre-declaring a variable.

```Walia
// The '10 <m>' is a dimensioned literal
var height = 10 <m>; 
var area = height * 5 <m>; // Result inferred as <m^2>
```

### 5. Function Signatures
For professional engineering, you should define the units of your inputs and outputs. This allows the **Walia Sentry** to verify that the logic is physically sound before the function is ever called.

```Walia
// A function that calculates Kinetic Energy: 0.5 * m * v^2
fun calculate_energy(mass: <kg>, velocity: <m/s>): <J> {
    return 0.5 * mass * (velocity * velocity);
}
```

**Intelligence Feature:** If you attempt to call `calculate_energy(10 <s>, 5 <m>)`, the compiler will block the execution. It knows that **Seconds** are not **Kilograms**, preventing a logical "Ghost Crash" in your AI models.

### 6. Unit-Aware Lists and Tensors
Because Walia handles **Massive Data**, you can apply units to entire collections. This ensures that every element in a multi-billion row table shares the same physical meaning.

```Walia
// A list where every item must be a length in meters
var path = List(<m>);
path.add(1.5 <m>);
// path.add(10 <kg>); // ERROR: Sovereign constraint violation
```

### 7. Visualizing the Syntax: The HUD
When you use angle bracket units, the **Command Nexus HUD** provides a "Physical Insight" view:
*   **Telemetry:** When you hover over a variable in the CLI, the HUD displays its SI-7 vector decomposition (e.g., `[L:1, M:0, T:-1...]`).
*   **PageMap:** Dimensioned variables are stored with a hidden metadata bit in the persistent heap, ensuring the "Meaning" survives a cold boot.

### 8. Practical: The Orbital Simulator
Let's build a small logic block that defines the parameters of a satellite orbit.

1.  **Define the Physical Constants:**
    ```Walia
    var orbit_radius: <km> = 42164;
    var period: <s> = 86164;
    ```
2.  **Calculate Velocity:**
    ```Walia
    // Inferred as <km/s>
    var orbital_speed = (2 * 3.14159 * orbit_radius) / period;
    
    print "Satellite Velocity: " + orbital_speed;
    ```
3.  **Apply a Constraint:**
    ```Walia
    var target_v: <km/s> = orbital_speed;
    print "Constraint Verified.";
    ```

### 9. Summary
1.  **Angle Brackets:** The primary syntax for defining units.
2.  **Operators:** Use `*`, `/`, and `^` to build composite units.
3.  **Inference:** Walia propagates units through mathematical chains automatically.
4.  **Verification:** Function parameters and return types can be physically constrained.

---
**Next Step:** Syntax allows us to express units. Now we must learn the rules of the engine. In the next module, we will explore the **Dimensional Algebra Laws**, learning exactly how the compiler multiplies and divides units to ensure your universe remains consistent.
