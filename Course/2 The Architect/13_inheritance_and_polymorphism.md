
# Tier II: The Architect
## Module 13: Inheritance and Polymorphism

### 1. Extending Behavior
Often, you will have multiple classes that share common logic. Instead of copying and pasting code, Walia allows you to create a **Subclass** that "Inherits" properties and methods from a **Superclass**.

Syntax: `class Subclass < Superclass`

### 2. The Relationship
*   **Superclass (Parent):** The generic blueprint (e.g., `Vehicle`).
*   **Subclass (Child):** The specific blueprint (e.g., `Car`).

The Child gets everything the Parent has, plus whatever it adds.

```Walia
class Vehicle {
    fun move() {
        print "Moving forward...";
    }
}

// Car inherits from Vehicle
class Car < Vehicle {
    fun honk() {
        print "Beep beep!";
    }
}

var c = Car();
c.move(); // Inherited! Output: Moving forward...
c.honk(); // Unique! Output: Beep beep!
```

### 3. Overriding (Polymorphism)
Sometimes a Child wants to do things differently than the Parent. You can define a method with the same name to **Override** it.

```Walia
class Plane < Vehicle {
    // Override 'move'
    fun move() {
        print "Flying high!";
    }
}

var p = Plane();
p.move(); // Output: Flying high!
```
This is **Polymorphism**: The same method name (`move`) does different things depending on the object type.

### 4. The `super` Keyword
If you override a method but still want to run the original logic, use `super`.

```Walia
class ElectricCar < Car {
    fun move() {
        super.move(); // Call the parent's move
        print "Silent running active.";
    }
}
```

### 5. Advanced: Deep Hierarchies
Walia supports deep inheritance chains (`SportsCar < Car < Vehicle`). However, excessive inheritance can become messy.
*   **Best Practice:** Use Inheritance for "Is-A" relationships (A Car IS A Vehicle).
*   **Alternative:** For "Has-A" or "Can-Do" relationships (A Car CAN Log Data), use **Traits** (next module).

### 6. Technical Sovereignty
Walia uses **Single Inheritance**. A class can only have one direct Superclass. This avoids the complexity of "Diamond Problems" found in C++. For multiple behaviors, we use the Trait system.

---
**Next Step:** Inheritance is powerful but rigid. In the next module, we will learn the modern way to share code: **Traits and Dynamic Composition**.
