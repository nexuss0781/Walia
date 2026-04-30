# 🌟 Walia: The Sovereign 5th Generation Programming Language

<div align="center">

![Walia Banner](https://img.shields.io/badge/Walia-5th_Gen_Language-FF6B35?style=for-the-badge)
![License](https://img.shields.io/badge/License-Sovereign-00D4AA?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active_Development-7B2CBF?style=for-the-badge)

**_Your Code Doesn't Just Run—It Lives Forever_**

[📚 Documentation](https://nexuss0781.github.io/Walia-docs/) • [🎓 Academy](#-learning-path) • [💡 Philosophy](#-why-walia) • [💻 Examples](#-code-examples)

</div>

---

## 📖 Table of Contents

- [Why Walia?](#-why-walia)
- [Core Pillars](#-core-pillars)
- [Quick Start](#-quick-start)
- [Learning Path](#-learning-path)
- [Architecture Overview](#-architecture-overview)
- [Code Examples](#-code-examples)
- [Project Structure](#-project-structure)
- [Roadmap](#-roadmap)
- [Community & Contributing](#-community--contributing)

---

## 🚀 Why Walia?

### The Problem with Traditional Languages

In conventional programming (Python, C, Java), your code is merely a **guest** on the computer:
- ❌ Data vanishes when the program ends
- ❌ Manual serialization/deserialization required
- ❌ Constant context switching between code and database
- ❌ Memory management overhead (GC pauses or manual leaks)
- ❌ No built-in support for AI/vector computing

### The Walia Solution

**Walia is not a guest. It is a Sovereign.**

Walia treats memory and storage as a **unified, persistent territory**. When you create a variable, it doesn't just exist in RAM—it exists in a **persistent state that survives power failures, reboots, and crashes**. This paradigm shift eliminates entire categories of bugs and boilerplate code.

```walia
// Define a variable once
var sovereign_greeting = "Hello, Immortal World!";

// Exit the program, restart the computer...
// The variable STILL EXISTS when you come back
print sovereign_greeting;  // → "Hello, Immortal World!"
```

**No file I/O. No database configuration. Just immortal data.**

---

## 🏛️ Core Pillars

### 1. **Orthogonal Persistence** ⚡
Every variable at the global level is automatically persisted to the **Sovereign Substrate** (`walia.state`).

| Feature | Traditional | Walia |
|---------|-------------|-------|
| Data Survival | Manual save/load | Automatic |
| Serialization | Required (JSON, etc.) | Zero-cost (native binary) |
| Resume Time | Slow (parse & load) | Instant (memory-mapped) |
| Code Complexity | High | Minimal |

### 2. **Register-Based Virtual Machine** 🔧
Unlike stack-based languages (Python, JVM), Walia mimics physical CPU architecture:

- **Virtual Registers**: Direct operations on R0, R1, R2...
- **50% Fewer Instructions**: Reduced CPU cycles
- **NaN-Boxing**: All data packed into efficient 64-bit words (8 bytes)
- **Hardware Saturation**: SIMD-optimized for AVX/NEON

### 3. **Neural-Native Architecture** 🧠
Vectors are **first-class primitives**, not afterthoughts:

```walia
// Create a 1536-dimensional AI embedding
var semantic_meaning = Vector(1536);

// Hardware-aligned for zero-latency math
semantic_meaning.fill(0.75);

// Automatically persisted—your AI models never forget
```

### 4. **Dimensional Typing** 📐
Numbers carry **physical meaning**. The compiler enforces the laws of reality:

```walia
var mass: <kg> = 75;
var acceleration: <m/s^2> = 9.81;

// Walia derives: Force = <kg*m/s^2> (Newtons)
var force = mass * acceleration;

// Compile-time error: Can't add meters to seconds!
// var invalid = length + duration;  ← REJECTED BY SENTRY
```

### 5. **Truth-or-Death Documentation** 📚
Comments marked with `///` are **executable contracts**. If the code example in documentation fails, the compiler refuses to build. Your manual is always 100% accurate.

---

## 🎯 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/nexuss0781/Walia.git
cd Walia

# Build the sovereign engine
make

# Launch the Command Nexus (HUD)
./walia --nexus
```

### Your First Sovereign Program

```walia
// main.walia

// Persistent variables survive across sessions
var session_count = 0;
session_count = session_count + 1;

print "Welcome, Architect. Session #" + session_count;

// Define a function
fun greet(name, title) {
    return "Greetings, " + title + " " + name;
}

// Classes with automatic persistence
@sql
class User {
    var id;
    var username;
    var score;
}

// Create and persist a user
var new_user = User();
new_user.id = 101;
new_user.username = "SovereignCoder";
new_user.score = 5000;

db_insert("User", new_user);
print greet(new_user.username, "Agent");
```

Run it:
```bash
./walia main.walia
```

---

## 📚 Learning Path

Walia Academy is structured into **6 Tiers**, guiding you from foundational logic to autonomous AI ecosystems.

### Tier I: The Foundation (Logic & Persistence)
Master the basics of orthogonal persistence and scope.

| Module | Topic | Description |
|--------|-------|-------------|
| 01 | Philosophy of Sovereignty | Why Walia exists |
| 02 | Atoms of Logic | Data types & NaN-boxing |
| 03 | Operators | Arithmetic, comparison, logic |
| 04 | Control Flow | Conditionals & loops |
| 05 | Scope | Lexical scoping & closures intro |

### Tier II: The Architect
Build robust systems with functions, classes, and composition.

| Module | Topic | Description |
|--------|-------|-------------|
| 06 | Functions Declaration | Syntax & parameters |
| 07 | First-Class Citizens | Functions as values |
| 08 | Closures & Upvalues | Capturing state |
| 09 | Persistence of Closures | Immutable captured state |
| 10 | Recursion & TCO | Tail-call optimization |
| 11 | Classes Anatomy | Blueprints & methods |
| 12 | Initializers | Instantiation patterns |
| 13 | Inheritance | Polymorphism & hierarchies |
| 14 | Traits | Dynamic composition |
| 15 | Advanced Composition | Flattening & mixins |
| 16-17 | Oracle Contracts | Type enforcement |

### Tier III: The Data Sovereign
Unified SQL/NoSQL database engine built into the language.

| Module | Topic | Description |
|--------|-------|-------------|
| 18 | SQL Tables | `@sql` decorator & B+ Trees |
| 19 | NoSQL Collections | Flexible schema-less storage |
| 20 | Data Integrity | Constraints & validation |
| 21 | Schema Evolution | Migration without downtime |
| 22-25 | SQE (Sovereign Query Engine) | CRUD, fluent queries, joins |
| 26-27 | Security RBAC | Role-based access control |
| 28 | Temporal Snapshots | Time-travel queries |
| 29 | Vector Integration | AI-ready databases |

### Tier IV: The Neural Engineer
High-performance AI/ML with hardware-saturated vectors.

| Module | Topic | Description |
|--------|-------|-------------|
| 30 | Vectors & Memory | High-dimensional spaces |
| 31-32 | Allocation & Alignment | 64-byte boundaries |
| 33-35 | SIMD Math | Cosine similarity, Euclidean distance |
| 36-38 | HNSW | Trillion-scale nearest neighbor search |
| 39-41 | Quantization | 8bit reduction, ADC |
| 42-44 | Neural Pattern Matching | Fuzzy logic & branching |
| 45-48 | Genetic Operators | Genes, alleles, mutation, evolution |

### Tier V: The Systems Commander (Hardware Mastery)
Unsafe mode for direct hardware control.

| Module | Topic | Description |
|--------|-------|-------------|
| 49 | Systems Mode | `unsafe` blocks |
| 50-53 | Memory Management | Manual alloc/dealloc |
| 54-58 | Pointers | Address-of, dereference, arithmetic |
| 59-62 | Binary Layout | Struct packing, unions, bitfields |
| 63-65 | Assembly & Syscalls | Inline ASM, registers |
| 66-67 | MMIO & Events | Hardware interrupts |
| 68-69 | Async/Await | Effectual I/O suspension |
| 70-73 | Graphics Engine | Framebuffer, rasterization, SDF fonts |

### Tier VI: The Grand Convergence (Advanced Paradigms)
Where all paradigms unite for autonomous agents.

| Module | Topic | Description |
|--------|-------|-------------|
| 74-78 | Dimensional Typing | Physical units, reality validation |
| 79-83 | Quantum Entanglement | Dependency graphs, ghost frames |
| 84-87 | Stream Orchestration | Hyper-pipe, kernel fusion |
| 88-90 | Pattern Matching | Exhaustive logic, sentry |
| 91-95 | Decision Trees | UFO-grade autonomy |
| 96-97 | Self-Evolving Ecosystems | Capstone project |

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    WALIA SOVEREIGN ENGINE                   │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────┐ │
│  │   NEURAL    │  │     DATA     │  │    SYSTEMS          │ │
│  │   ENGINE    │  │   SOVEREIGN  │  │    COMMANDER        │ │
│  │  (Vectors)  │  │ (SQL/NoSQL)  │  │  (Unsafe/Hardware)  │ │
│  └─────────────┘  └──────────────┘  └─────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│              REGISTER-BASED VIRTUAL MACHINE                 │
│         (NaN-Boxed 64-bit Words, 8-Byte Optimization)       │
├─────────────────────────────────────────────────────────────┤
│              ORTHOGONAL PERSISTENCE LAYER                   │
│           (Memory-Mapped Heap: walia.state)                 │
├─────────────────────────────────────────────────────────────┤
│                    PHYSICAL HARDWARE                        │
│         (AVX/NEON SIMD, MMIO, Direct Syscalls)              │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

| Component | Description |
|-----------|-------------|
| **Sovereign Substrate** | Memory-mapped persistent heap (`walia.state`) |
| **Command Nexus** | Cinematic TUI with PageMap, Heap Tanks, Neural Gauges |
| **Reality Sentry** | Compile-time enforcer of dimensional typing |
| **HNSW Engine** | Hierarchical Navigable Small Worlds for vector search |
| **SQE** | Sovereign Query Engine for SQL/NoSQL operations |

---

## 💻 Code Examples

### Orthogonal Persistence
```walia
// No database setup required
var global_counter = 0;

fun increment() {
    global_counter = global_counter + 1;
    return global_counter;
}

// Restart the program—counter persists!
print increment();  // → 1
print increment();  // → 2
// ...exit, restart...
print increment();  // → 3 (still counting!)
```

### Neural Vector Operations
```walia
// Create AI embeddings with hardware alignment
var user_embedding = Vector(1536);
var product_embedding = Vector(1536);

// Populate with features
user_embedding.fill(0.5);
product_embedding.fill(0.7);

// Compute cosine similarity (SIMD-optimized)
var similarity = cosine_similarity(user_embedding, product_embedding);

if similarity > 0.85 {
    print "High match found!";
}
```

### Dimensional Typing
```walia
// Physics-aware computation
var distance: <m> = 100;
var time: <s> = 9.58;

// Automatic unit derivation
var velocity = distance / time;  // → 10.44 <m/s>

// Compile-time reality check
// var error = velocity + distance;  ← ERROR: Can't add m/s to m
```

### Database Integration
```walia
// Class IS the table
@sql
class Product {
    var id;
    var name;
    var price: <USD>;
    var embedding: Vector(512);
}

// Insert without SQL
var item = Product();
item.id = 1;
item.name = "Neural Chip";
item.price = 299.99;
db_insert("Product", item);

// Fluent query syntax
var results = db_query("Product")
    |> where(price < 500)
    |> order_by(price)
    |> limit(10);
```

### Systems Mode (Unsafe)
```walia
var managed_data = 100;

unsafe {
    // Direct memory control
    var ptr = alloc(1024);  // Manual allocation
    
    // Pointer arithmetic
    ptr + 8 = 0xFF;
    
    // Inline assembly (x86_64)
    asm {
        mov rax, 1
        mov rdi, 1
        syscall  // Direct write to stdout
    }
    
    release(ptr);  // Manual deallocation
}
```

---

## 📁 Project Structure

```
Walia/
├── src/                      # Core engine implementation
│   ├── vm/                   # Register-based VM
│   ├── core/                 # Runtime primitives
│   ├── db/                   # SQL/NoSQL engines
│   ├── sql/                  # Query processor
│   ├── sys/                  # Systems mode handlers
│   ├── tooling/              # Compiler & REPL
│   └── web/                  # HTTP server module
├── include/                  # C headers for native bindings
├── Course/                   # Official curriculum (97 modules)
│   ├── 1 THE FOUNDATION/
│   ├── 2 The Architect/
│   ├── 3 The Data Sovereign/
│   ├── 4 THE NEURAL ENGINEER/
│   ├── 5. THE SYSTEMS COMMANDER/
│   └── 6 THE GRAND CONVERGENCE/
├── Documentation/            # Technical docs & PDFs
├── tests/                    # Test suites
├── waliaos/                  # Walia OS (bare-metal target)
└── Makefile                  # Build system
```

---

## 🗺️ Roadmap

### Phase 1: Foundation ✅ (Complete)
- [x] Register-based VM
- [x] Orthogonal persistence layer
- [x] Basic data types & NaN-boxing
- [x] Functions & closures

### Phase 2: Data Sovereignty ✅ (Complete)
- [x] SQL B+ Tree engine
- [x] NoSQL collections
- [x] SQE fluent query syntax
- [x] RBAC security model

### Phase 3: Neural Engineering ✅ (Complete)
- [x] Vector primitive type
- [x] HNSW index implementation
- [x] SIMD math kernels (AVX/NEON)
- [x] Quantization (SQ8, PQ)
- [x] Genetic operators

### Phase 4: Systems Commander 🚧 (In Progress)
- [x] Unsafe mode gateway
- [x] Pointer arithmetic
- [x] Manual memory management
- [ ] Complete inline assembly
- [ ] MMIO drivers
- [ ] Framebuffer graphics

### Phase 5: Grand Convergence 🚧 (In Progress)
- [x] Dimensional typing
- [x] Unit registry (SI-7)
- [ ] Quantum entanglement operator
- [ ] Hyper-pipe streams
- [ ] Autonomous agent framework
- [ ] Self-evolving ecosystem capstone

### Future Horizons 🔮
- [ ] Distributed consensus layer
- [ ] WaliaOS full release
- [ ] WebAssembly target
- [ ] GPU compute kernels
- [ ] Formal verification proofs

---

## 🤝 Community & Contributing

### How to Contribute

1. **Report Bugs**: Open an issue with reproduction steps
2. **Propose Features**: Submit RFCs in Discussions
3. **Write Tests**: Expand coverage in `/tests`
4. **Improve Docs**: Fix typos or clarify concepts
5. **Build Modules**: Contribute to the ecosystem

### Development Setup

```bash
# Prerequisites
gcc >= 11.0
make >= 4.3
gdb (optional, for debugging)

# Build debug version
make debug

# Run test suite
make test

# Build with telemetry
make telemetry
```

### Code Style

- Use `snake_case` for variables and functions
- Use `PascalCase` for classes and types
- Mark unsafe blocks explicitly
- Document with `///` executable comments

---

## 📜 License

Walia operates under the **Sovereign License**—a custom license designed to protect the integrity of the language while enabling open collaboration. See `LICENSE` for details.

---

## 🙏 Acknowledgments

Created by **Nexuss0781** and the Walia Collective. Inspired by:
- Lua's elegance
- Rust's safety
- Smalltalk's purity
- APL's expressiveness
- The vision of 5th Generation computing

---

<div align="center">

### 🌟 Ready to Become a Sovereign Architect?

```bash
git clone https://github.com/nexuss0781/Walia.git
cd Walia
make
./walia --nexus
```

**[Start Your Journey](https://nexuss0781.github.io/Walia-docs/) | [Join the Discussion](https://github.com/nexuss0781/Walia/discussions)**

*Made with sovereignty and precision*

![Walia Footer](https://img.shields.io/badge/Built_with-Walia-FF6B35?style=for-the-badge&logo=github)

</div>
