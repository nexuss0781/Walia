# WALIA CORE ENGINE - DEEP DIVE ROADMAP
## Complete Analysis & Revolutionization Strategy for src/core/

---

## EXECUTIVE SUMMARY

The `src/core/` directory contains **13 implementation files** and **15 header files** totaling approximately **6,500+ lines of code**. This is the heart of the Walia language - a sophisticated bytecode VM with:

- **NaN-boxing** value representation
- **Register-based VM** (not stack-based)
- **Persistent heap** with memory-mapped storage
- **Mark-and-sweep GC** with write barriers
- **Closure-based** function model with upvalues
- **SHA-256 fingerprinting** for code identity
- **Computed goto dispatch** for performance
- **RLE-compressed** debug info

---

## PART 1: ARCHITECTURAL OVERVIEW

### The Execution Pipeline
```
Source Code → Scanner → Parser → AST → Compiler → Bytecode → VM → Execution
```

### Component Dependency Graph
```
common.h (foundation)
    ↓
value.h → memory.h → object.h → gc.h
    ↓           ↓         ↓
  chunk.h ← opcode.h   table.h
    ↓                     ↓
  vm.h ←──────────────────┘
    ↓
token.h → scanner.h → parser.h → ast.h → compiler.h
    ↓                                         ↓
  hash.h ←────────────────────────────────────┘
```

---

## PART 2: FILE-BY-FILE BREAKDOWN

### GROUP A: FOUNDATION LAYER (Study First)

#### 1. common.h (30 lines)
**Purpose**: Global configuration and constants

**Key Contents**:
- `NAN_BOXING` flag - enables 64-bit value optimization
- `hadError` global flag - compilation/runtime error tracking
- Exit codes: SUCCESS(0), COMPILE_ERROR(65), RUNTIME_ERROR(70), IO_ERROR(74)
- WLD_VERSION: 120
- WLD_PAGE_SIZE: 4096 bytes

**Critical Insights**:
- NaN-boxing is ALWAYS enabled (not optional)
- Single global error flag (not thread-safe)
- No kernel mode detection (WALIA_KERNEL undefined)

**Revolution Opportunities**:
- [ ] Make NaN-boxing runtime switchable for debugging
- [ ] Replace global error flag with thread-local storage
- [ ] Add compilation flags for different optimization levels
- [ ] Implement proper error context stack

---

#### 2. value.h + value.c (95 + 60 lines)
**Purpose**: NaN-boxed value representation

**Key Architecture**:
```c
typedef uint64_t Value;  // Everything is 64 bits

NaN-Boxing Layout:
- Numbers: IEEE 754 double (no tagging needed)
- nil:     0x7ffc000000000001
- false:   0x7ffc000000000002
- true:    0x7ffc000000000003
- Objects: 0x8000000000000000 | QNAN | pointer
```

**Type Checking Macros**:
- `IS_NUMBER(v)`: Check if NOT a quiet NaN
- `IS_NIL(v)`: Exact equality check
- `IS_BOOL(v)`: Mask and compare
- `IS_OBJ(v)`: Check sign bit + QNAN

**ValueArray**: Dynamic array of Values
- Growth strategy: capacity < 8 ? 8 : capacity * 2
- Used for constant pools in bytecode

**Critical Insights**:
- Uses `memcpy` for type punning (strict aliasing safe)
- No inline assembly for performance
- printValue() is incomplete (just prints "<obj>")

**Revolution Opportunities**:
- [ ] Add SIMD-optimized value array operations
- [ ] Implement tagged pointer compression for 32-bit systems
- [ ] Add value type introspection for debugging
- [ ] Optimize IS_* macros with compiler intrinsics
- [ ] Complete printValue() with full object printing

---

#### 3. memory.h + memory.c (60 + 95 lines)
**Purpose**: Persistent heap with memory-mapped storage

**Architecture**:
```
Persistent Heap (128 MB)
    ↓
mmap() to walia.state file
    ↓
Bump allocator (heapNext pointer)
    ↓
Card table for write barrier (512-byte cards)
```

**Key Functions**:
- `initPersistentHeap()`: Creates/maps 128MB file
- `reallocate()`: Bump pointer allocation
- `syncPersistentHeap()`: msync() to disk
- `markCard()`: Write barrier for GC
- `clearCards()`: Reset dirty flags

**Critical Insights**:
- Uses `MAP_SHARED` (changes visible to other processes)
- No compaction or defragmentation
- Bump allocator never reuses memory (until GC)
- Card size: 512 bytes (not cache-line aligned)
- No NUMA awareness
- Heap exhaustion = immediate exit(1)

**Revolution Opportunities**:
- [ ] Implement generational GC with nursery/tenured spaces
- [ ] Add compaction to reduce fragmentation
- [ ] Use `MAP_PRIVATE` with COW for snapshots
- [ ] Align cards to cache lines (64 bytes)
- [ ] Add memory pressure callbacks instead of exit()
- [ ] Implement arena allocators for different object sizes
- [ ] Add NUMA-aware allocation
- [ ] Support multiple heap files for scaling
- [ ] Add memory encryption for security
- [ ] Implement lazy heap initialization

---

#### 4. object.h + object.c (180 + 200 lines)
**Purpose**: Object type system and allocation

**Object Type Hierarchy**:
```
Obj (base)
├── ObjString (interned, FNV-1a hash)
├── ObjFunction (bytecode + metadata)
├── ObjNative (C function pointer)
├── ObjClosure (function + upvalues)
├── ObjUpvalue (captured variable)
├── ObjClass (name + method table)
├── ObjInstance (class + field table)
├── ObjBoundMethod (receiver + method)
├── ObjVector (SIMD-aligned float array)
├── ObjStream (iterator protocol)
├── ObjFuture (async result)
├── ObjGene (genetic programming)
├── ObjGenome (individual)
├── ObjCodeBlob (JIT-compiled native code)
├── ObjList (dynamic array)
├── ObjMap (hash table)
├── ObjContinuation (effect system)
└── ObjWaitGroup (parallel coordination)
```

**String Interning**:
- FNV-1a hash algorithm
- Immediate deduplication on creation
- Global string table in VM

**Function Metadata**:
```c
struct ObjFunction {
    int arity;              // Parameter count
    int upvalueCount;       // Closure variables
    Chunk chunk;            // Bytecode
    ObjString* name;        // Function name
    ObjCodeBlob* nativeThunk; // JIT code (if compiled)
    uint32_t callCount;     // Profiling counter
    bool isAsync;           // Async function flag
    uint8_t fingerprint[32]; // SHA-256 identity
};
```

**Critical Insights**:
- All objects 64-byte aligned (cache-line optimization)
- Objects linked in global list (vm.objects)
- No object pooling or recycling
- String interning happens immediately (not lazy)
- JIT threshold: 1024 calls

**Revolution Opportunities**:
- [ ] Implement object pooling for common types
- [ ] Add weak references
- [ ] Lazy string interning for short-lived strings
- [ ] Optimize small string storage (SSO)
- [ ] Add object metadata for profiling
- [ ] Implement copy-on-write for immutable objects
- [ ] Add object versioning for hot-reload
- [ ] Optimize upvalue representation
- [ ] Add object compression for serialization
- [ ] Implement object sharding for parallelism

---

#### 5. gc.h + gc.c (25 + 80 lines)
**Purpose**: Mark-and-sweep garbage collector

**Algorithm**:
```
1. Mark Phase:
   - Mark stack values
   - Mark call frames
   - Mark open upvalues
   - Mark global/string tables
   - Trace gray objects (BFS)

2. Sweep Phase:
   - Walk object list
   - Free unmarked objects
   - Reset mark bits
```

**Tri-Color Marking**:
- White: Unreachable (will be collected)
- Gray: Reachable but not traced (in grayStack)
- Black: Reachable and traced (isMarked = true)

**Critical Insights**:
- No generational collection
- No incremental collection (stop-the-world)
- Gray stack grows dynamically (realloc)
- Sweeps entire heap every time
- No write barrier integration (cards not used in GC)
- Calls `reallocate(obj, 0, 0)` to free

**Revolution Opportunities**:
- [ ] Implement generational GC (nursery + tenured)
- [ ] Add incremental marking (time-sliced)
- [ ] Use card table for remembered sets
- [ ] Implement concurrent marking
- [ ] Add GC pause time budgets
- [ ] Optimize gray stack (use object fields)
- [ ] Add weak reference support
- [ ] Implement object finalization
- [ ] Add GC telemetry and profiling
- [ ] Support multiple GC strategies (copying, compacting)
- [ ] Add escape analysis to avoid allocation
- [ ] Implement region-based collection

---

### GROUP B: BYTECODE LAYER

#### 6. opcode.h (70 lines)
**Purpose**: Instruction set architecture

**Instruction Format** (32-bit):
```
[OP:8][A:8][B:8][C:8]  or  [OP:8][A:8][BC:16]
```

**Instruction Categories**:

1. **Data & Flow** (17 ops):
   - LOAD_CONST, LOAD_BOOL, LOAD_NIL
   - MOVE, ADD, SUB, MUL, DIV
   - NOT, NEGATE, EQUAL, GREATER, LESS
   - JUMP, JUMP_IF_FALSE, LOOP, CALL

2. **Closures & Scope** (8 ops):
   - CLOSURE, GET_UPVALUE, SET_UPVALUE
   - RETURN, PRINT
   - DEFINE_GLOBAL, GET_GLOBAL, SET_GLOBAL

3. **Effects & Continuations** (3 ops):
   - HANDLE, PERFORM, RESUME

4. **Object Orientation** (6 ops):
   - CLASS, INHERIT, METHOD
   - GET_PROPERTY, SET_PROPERTY, INVOKE

5. **Advanced Paradigms** (5 ops):
   - ENTANGLE (reactive programming)
   - ITER_INIT, ITER_NEXT (iterators)
   - JUMP_TABLE, MATCH_SIMILAR (pattern matching)

6. **Bitwise & Low-Level** (8 ops):
   - BIT_AND, BIT_OR, BIT_XOR, BIT_SHL, BIT_SHR
   - PTR_READ, PTR_WRITE, ADDRESS_OF

7. **Genetic Programming** (2 ops):
   - GENE, EVOLVE

8. **System & Memory** (5 ops):
   - SYSCALL, ASM_EXEC
   - MALLOC, MFREE, SIZEOF

9. **Database & SIMD** (3 ops):
   - DB_SIMD_FILTER_GT, DB_LOAD_COL, DB_COMMIT

10. **System IO & Casting** (9 ops):
    - SYS_LOAD_U8/U16/U32/U64
    - SYS_STORE_U8/U16/U32/U64
    - SYS_CAST

**Total: 66 opcodes**

**Critical Insights**:
- Register-based (not stack-based)
- 256 registers max (8-bit addressing)
- No specialized string/array ops
- No SIMD vector ops (except DB filter)
- No atomic operations
- No exception handling ops

**Revolution Opportunities**:
- [ ] Add SIMD vector operations (VADD, VMUL, etc.)
- [ ] Add atomic operations (ATOMIC_ADD, CAS, etc.)
- [ ] Add specialized string ops (CONCAT, SLICE)
- [ ] Add array operations (ARRAY_GET, ARRAY_SET)
- [ ] Implement superinstructions (fused ops)
- [ ] Add prefetch hints
- [ ] Add branch prediction hints
- [ ] Implement variable-length encoding for constants
- [ ] Add debugging opcodes (BREAKPOINT, TRACE)
- [ ] Optimize hot paths with specialized ops

---

#### 7. chunk.h + chunk.c (60 + 140 lines)
**Purpose**: Bytecode container with compressed debug info

**Chunk Structure**:
```c
struct Chunk {
    int count;              // Instruction count
    int capacity;           // Allocated capacity
    Instruction* code;      // 32-bit instructions
    ValueArray constants;   // Constant pool
    LineInfo lines;         // RLE-compressed line numbers
};
```

**Line Encoding (RLE)**:
```c
struct LineStart {
    int line;   // Source line number
    int count;  // How many instructions from this line
};
```

Example:
```
Instructions: [0, 1, 2, 3, 4, 5]
Lines:        [10, 10, 10, 11, 11, 12]
Encoded:      [{10, 3}, {11, 2}, {12, 1}]
```

**Growth Strategy**:
- capacity < 8 ? 8 : capacity * 2
- Applies to both code and constants

**Disassembler**:
- Prints bytecode in human-readable format
- Shows line numbers with "|" for repeated lines
- Decodes register operands (R0, R1, etc.)

**Critical Insights**:
- No constant deduplication (same value stored multiple times)
- No instruction compression
- RLE is simple but not optimal for random access
- Disassembler incomplete (only handles basic ops)

**Revolution Opportunities**:
- [ ] Implement constant pool deduplication
- [ ] Add instruction compression (variable-length encoding)
- [ ] Use delta encoding for line numbers
- [ ] Add source position mapping (column numbers)
- [ ] Implement bytecode verification
- [ ] Add bytecode optimization passes
- [ ] Support hot-swapping of chunks
- [ ] Add bytecode serialization format
- [ ] Implement bytecode signing for security
- [ ] Add instruction profiling metadata

---

#### 8. vm.h + vm.c + vm_interpret.c (120 + 450 + 60 lines)
**Purpose**: Virtual machine execution engine

**VM Architecture**:
```c
struct VM {
    // Execution State
    Value stack[STACK_MAX];      // 16,384 values (64 frames * 256)
    Value* stackTop;             // Stack pointer
    CallFrame frames[FRAMES_MAX]; // 64 call frames
    int frameCount;              // Active frames
    
    // Effect System
    EffectHandler* handlerStack; // Effect handlers
    int handlerCount;            // Handler count
    
    // Memory Management
    Obj* objects;                // Object linked list
    size_t bytesAllocated;       // Current allocation
    size_t nextGC;               // GC threshold
    Table strings;               // Interned strings
    Table globals;               // Global variables
    Table methods;               // Method cache
    
    // GC State
    int grayCount;               // Gray object count
    int grayCapacity;            // Gray stack capacity
    Obj** grayStack;             // Gray object stack
    ObjUpvalue* openUpvalues;    // Open upvalue list
    
    // Telemetry
    TraceEntry* traceBuffer;     // Time-travel trace (65536 entries)
    int traceIndex;              // Current trace position
    bool isTraceActive;          // Tracing enabled
    
    // Testing
    bool isTestMode;             // Test mode flag
};
```

**Call Frame**:
```c
struct CallFrame {
    ObjClosure* closure;    // Function being executed
    Instruction* ip;        // Instruction pointer
    Value* slots;           // Register window base
    int returnToReg;        // Where to store return value
};
```

**Dispatch Mechanism**:
- **GCC/Clang**: Computed goto (direct threading)
- **Other**: Switch statement

**Computed Goto Performance**:
```c
static void* dispatch_table[] = {
    &&OP_LOAD_CONST_lbl,
    &&OP_ADD_lbl,
    // ... 66 entries
};

#define DISPATCH() goto *dispatch_table[GET_OP(*frame->ip)]
```

**Critical VM Operations**:

1. **Arithmetic** (ADD, SUB, MUL, DIV):
   - Direct number operations
   - Falls back to operator overloading
   - Triggers ripple effects for reactive vars

2. **Function Calls**:
   - JIT threshold check (1024 calls)
   - Native thunk execution if available
   - Frame allocation and setup
   - Stack overflow detection

3. **Closures**:
   - Upvalue capture (open/closed)
   - Upvalue sharing detection
   - Closure creation with upvalue array

4. **Classes & Methods**:
   - Method table lookup
   - Bound method creation
   - Property get/set with traps
   - Inheritance via table copying

5. **Effects**:
   - Handler stack management
   - Continuation capture
   - Resume with value

**Watchdog Integration**:
- `PET_WATCHDOG()` macro in loops
- Prevents infinite loops in kernel mode
- Updates per-core heartbeat

**Critical Insights**:
- Register-based (not stack-based)
- No register allocation optimization
- No inline caching for property access
- Method dispatch is linear search
- No polymorphic inline caching
- Trace buffer is circular (overwrites old entries)
- No JIT compilation in core (calls external)

**Revolution Opportunities**:
- [ ] Implement register allocation optimization
- [ ] Add inline caching for property access
- [ ] Implement polymorphic inline caching (PIC)
- [ ] Add method dispatch optimization (vtables)
- [ ] Implement trace-based JIT compilation
- [ ] Add speculative optimization
- [ ] Implement deoptimization support
- [ ] Add parallel execution support
- [ ] Optimize closure representation
- [ ] Add stack overflow recovery
- [ ] Implement continuation-passing style
- [ ] Add debugging protocol (DAP)
- [ ] Optimize upvalue access
- [ ] Add instruction fusion
- [ ] Implement adaptive optimization

---

### GROUP C: FRONTEND LAYER

#### 9. token.h (70 lines)
**Purpose**: Token type definitions

**Token Categories**:

1. **Single-character** (18 tokens):
   - Delimiters: `( ) { } [ ]`
   - Operators: `, . - + ; / * : & @ ~ |`

2. **One or two character** (11 tokens):
   - Comparison: `! != = == > >= < <=`
   - Special: `~= |> => ..`

3. **Literals** (3 tokens):
   - IDENTIFIER, STRING, NUMBER

4. **Keywords** (28 tokens):
   - Control: if, else, for, while, in, match
   - Declaration: var, fun, class, gene
   - Values: true, false, nil
   - OOP: this, super, with
   - Effects: perform, handle, resume
   - Systems: layout, union, asm, syscall, alloc, release, sizeof
   - Async: async, await, parallel
   - Testing: test
   - Operators: and, or, print, return
   - Units: unit

5. **Special** (2 tokens):
   - ERROR, EOF

**Total: 132 token types**

**Critical Insights**:
- No token for comments (handled in scanner)
- No token for multiline strings
- No token for raw strings
- No token for template strings
- Unit syntax: `<m/s>` (angle brackets)

**Revolution Opportunities**:
- [ ] Add template string tokens
- [ ] Add raw string tokens
- [ ] Add multiline string tokens
- [ ] Add numeric literal suffixes (u32, f64)
- [ ] Add binary/octal/hex number tokens
- [ ] Add character literal token
- [ ] Add regex literal token
- [ ] Add documentation comment tokens
- [ ] Add attribute/annotation tokens

---

#### 10. scanner.h + scanner.c (30 + 250 lines)
**Purpose**: Lexical analysis (tokenization)

**Scanner State**:
```c
struct Scanner {
    const char* start;    // Token start
    const char* current;  // Current position
    int line;             // Current line number
};
```

**Scanning Strategy**:
- Single-pass, no backtracking
- Maximal munch (longest match)
- Keywords via trie-like checking

**Keyword Recognition**:
```c
// Optimized switch on first character
switch (scanner.start[0]) {
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    // ...
}
```

**String Scanning**:
- Supports newlines in strings
- No escape sequence processing (done later)
- Unterminated string = error token

**Number Scanning**:
- Supports decimals: `123.456`
- No scientific notation
- No hex/binary/octal
- No underscores for readability

**Whitespace Handling**:
- Skips spaces, tabs, `\r`, `\n`
- Handles `//` line comments
- No block comments

**Unit Syntax**:
```walia
var speed: <m/s> = 10.0;
```
- Scans `<...>` as TOKEN_UNIT
- Includes content between brackets

**Critical Insights**:
- No column number tracking
- No source file tracking (for includes)
- No Unicode support
- No escape sequence validation
- Keyword checking is O(1) but verbose

**Revolution Opportunities**:
- [ ] Add column number tracking
- [ ] Add source file/path tracking
- [ ] Implement Unicode support (UTF-8)
- [ ] Add escape sequence validation
- [ ] Support scientific notation (1e10)
- [ ] Add hex/binary/octal literals
- [ ] Support number separators (1_000_000)
- [ ] Add block comments /* */
- [ ] Add nested block comments
- [ ] Implement perfect hash for keywords
- [ ] Add scanner error recovery
- [ ] Support string interpolation scanning
- [ ] Add preprocessor directives

---

#### 11. parser.h + parser.c (50 + 550 lines)
**Purpose**: Syntax analysis (AST construction)

**Parser State**:
```c
struct Parser {
    Token current;      // Current token
    Token previous;     // Previous token
    bool hadError;      // Error flag
    bool panicMode;     // Panic mode for recovery
};
```

**Parsing Strategy**:
- Recursive descent
- Pratt parsing for expressions (precedence climbing)
- Single-pass (no AST optimization pass)

**Expression Precedence** (lowest to highest):
```
1. Assignment (=, ~=)
2. Logical OR (or)
3. Logical AND (and)
4. Equality (==, !=)
5. Comparison (<, >, <=, >=)
6. Term (+, -)
7. Factor (*, /)
8. Unary (!, -, &, *)
9. Call ((), [], .)
10. Primary (literals, identifiers)
```

**Statement Types**:
- Expression statement
- Print statement
- Variable declaration
- Block statement
- If statement
- While statement
- For statement (desugared to while)
- Function declaration
- Class declaration
- Gene declaration
- Layout declaration
- Assembly statement
- Return statement
- Release statement

**Advanced Features**:

1. **Pattern Matching**:
```walia
match value {
    ~pattern if guard => body,
    _ => default
}
```

2. **Slicing**:
```walia
array[start:end:step]
tensor[0:10, 5:15, ::2]
```

3. **Entanglement** (reactive):
```walia
var y ~= x * 2;  // y updates when x changes
```

4. **Pipe Operator**:
```walia
data |> filter |> map |> reduce
```

5. **Effects**:
```walia
handle "IO" {
    perform "read";
}
```

**Critical Insights**:
- No operator precedence table (hardcoded)
- No AST optimization
- Error recovery is basic (panic mode)
- No syntax sugar desugaring pass
- For loops desugared during parsing
- Match expressions create MatchCase arrays

**Revolution Opportunities**:
- [ ] Implement operator precedence table
- [ ] Add AST optimization pass
- [ ] Improve error recovery (synchronization points)
- [ ] Add better error messages with suggestions
- [ ] Implement syntax sugar desugaring pass
- [ ] Add macro system
- [ ] Support operator overloading in parser
- [ ] Add pattern matching exhaustiveness checking
- [ ] Implement incremental parsing
- [ ] Add syntax highlighting hints
- [ ] Support multiple error reporting
- [ ] Add AST validation pass
- [ ] Implement constant folding in parser
- [ ] Add dead code detection

---

#### 12. ast.h + ast.c (280 + 350 lines)
**Purpose**: Abstract Syntax Tree representation

**Expression Types** (19 types):
```c
EXPR_BINARY      // a + b
EXPR_GROUPING    // (expr)
EXPR_LITERAL     // 42, "hello", true
EXPR_UNARY       // -x, !flag
EXPR_VARIABLE    // identifier
EXPR_ASSIGN      // x = value
EXPR_LOGICAL     // a and b, a or b
EXPR_CALL        // func(args)
EXPR_PERFORM     // perform "effect"
EXPR_ADDRESS     // &variable
EXPR_DEREF       // *pointer
EXPR_SYSCALL     // syscall(id, args)
EXPR_ALLOC       // alloc(size)
EXPR_SIZEOF      // sizeof(Type)
EXPR_SLICE       // array[1:10:2]
EXPR_AWAIT       // await future
EXPR_ENTANGLE    // x ~= source
EXPR_PIPE        // a |> b
EXPR_MATCH       // match x { ... }
EXPR_RANGE       // 1..10
```

**Statement Types** (16 types):
```c
STMT_EXPRESSION  // expr;
STMT_PRINT       // print expr;
STMT_VAR         // var x = init;
STMT_BLOCK       // { stmts }
STMT_IF          // if (cond) then else
STMT_WHILE       // while (cond) body
STMT_FUNCTION    // fun name(params) body
STMT_RETURN      // return value;
STMT_HANDLE      // handle "effect" body
STMT_RESUME      // resume cont value
STMT_TEST        // test "desc" body
STMT_LAYOUT      // layout Name { ... }
STMT_ASM         // asm { "code" }
STMT_RELEASE     // release ptr;
STMT_CLASS       // class Name { ... }
STMT_GENE        // gene Name { ... }
```

**AST Node Structure**:
```c
struct Expr {
    ExprType type;
    int line;
    WldDimensionVector dimensions;  // For unit checking
};

struct Stmt {
    StmtType type;
    int line;
};
```

**Memory Management**:
- Uses `reallocate()` for allocation
- Manual `freeExpr()` and `freeStmt()` recursion
- No reference counting
- No arena allocation

**Critical Insights**:
- AST nodes allocated individually (not in arena)
- No parent pointers (can't traverse up)
- No visitor pattern implementation
- Dimension vectors for physical units
- Match cases stored as dynamic arrays
- No AST serialization

**Revolution Opportunities**:
- [ ] Implement arena allocation for AST
- [ ] Add parent pointers for upward traversal
- [ ] Implement visitor pattern
- [ ] Add AST serialization/deserialization
- [ ] Implement AST cloning
- [ ] Add source location ranges (not just line)
- [ ] Implement AST diffing for hot-reload
- [ ] Add AST pretty-printing
- [ ] Implement AST validation
- [ ] Add AST transformation framework
- [ ] Support AST macros
- [ ] Add AST-level optimization
- [ ] Implement AST caching

---

#### 13. compiler.h + compiler.c (80 + 450 lines)
**Purpose**: AST to bytecode compilation

**Compiler State**:
```c
struct SovereignCompiler {
    SovereignCompiler* enclosing;  // Parent compiler (for nested functions)
    ObjFunction* function;         // Function being compiled
    FunctionType type;             // Script/Function/Test/Lambda
    
    Local locals[256];             // Local variables
    int localCount;                // Active locals
    int scopeDepth;                // Nesting level
    int regTop;                    // Next free register
    
    int packReg;                   // Current packing register
    int packOffset;                // Offset within packed register
    
    Upvalue upvalues[256];         // Captured variables
};
```

**Local Variable**:
```c
struct Local {
    Token name;                    // Variable name
    int depth;                     // Scope depth
    int reg;                       // Register number
    int offset;                    // Offset within register (for packing)
    WaliaSysType type;             // System type
    WldDimensionVector dimensions; // Physical units
    bool isCaptured;               // Captured by closure
};
```

**Register Allocation**:
- Bump allocator (regTop++)
- Small types packed into single register:
  - u8, u16, u32 can share a 64-bit register
  - Packing offset tracks position within register
- No register reuse optimization
- Max 256 registers (8-bit addressing)

**Compilation Phases**:

1. **Expression Compilation**:
   - Recursive descent through AST
   - Allocates target register
   - Emits bytecode instructions
   - Returns register containing result

2. **Statement Compilation**:
   - Handles control flow
   - Manages scope depth
   - Emits jumps and labels

3. **Function Compilation**:
   - Creates nested compiler
   - Compiles parameters as locals
   - Compiles body
   - Emits closure instruction with upvalues

4. **Class Compilation**:
   - Emits CLASS instruction
   - Compiles methods
   - Handles inheritance (table copying)
   - Handles traits (composition)

**Upvalue Resolution**:
```
1. Check local variables in current scope
2. If not found, recursively check enclosing scopes
3. Mark variable as captured
4. Add upvalue to current function
5. Emit GET_UPVALUE/SET_UPVALUE instructions
```

**Optimization Features**:
- Constant folding: None
- Dead code elimination: None
- Peephole optimization: None
- Register allocation: Bump allocator only

**Unit Checking**:
- Validates dimensional consistency
- Checks binary operations (m/s + m/s = m/s)
- Checks assignments (m/s = m/s)
- Integrated with compiler

**Function Fingerprinting**:
- SHA-256 hash of bytecode + constants
- Used for hot-reload and versioning
- Computed at end of compilation

**Critical Insights**:
- Single-pass compilation (no optimization passes)
- No SSA form
- No control flow graph
- No data flow analysis
- No type inference (explicit types only)
- Upvalue resolution is O(n) per lookup
- No tail call optimization

**Revolution Opportunities**:
- [ ] Implement multi-pass compilation
- [ ] Add SSA form transformation
- [ ] Build control flow graph
- [ ] Implement data flow analysis
- [ ] Add type inference
- [ ] Optimize upvalue resolution (hash table)
- [ ] Implement tail call optimization
- [ ] Add constant folding
- [ ] Implement dead code elimination
- [ ] Add peephole optimization
- [ ] Implement register allocation optimization
- [ ] Add loop optimization (hoisting, unrolling)
- [ ] Implement inlining
- [ ] Add escape analysis
- [ ] Implement partial evaluation
- [ ] Add profile-guided optimization

---

### GROUP D: UTILITY LAYER

#### 14. hash.h + hash.c (30 + 150 lines)
**Purpose**: SHA-256 cryptographic hashing

**Use Cases**:
- Function fingerprinting (identity)
- Hot-reload version checking
- Module versioning
- Code integrity verification

**API**:
```c
WaliaHashContext ctx;
walia_hash_init(&ctx);
walia_hash_update(&ctx, data, len);
walia_hash_final(&ctx, hash);  // 32-byte output
walia_hash_to_string(hash, str); // Hex string
```

**Implementation**:
- Standard SHA-256 algorithm
- 512-bit blocks
- 64 rounds per block
- No hardware acceleration (no AES-NI)

**Critical Insights**:
- Pure software implementation
- No SIMD optimization
- No incremental hashing support
- Used only for functions (not data)

**Revolution Opportunities**:
- [ ] Add hardware acceleration (AES-NI, SHA extensions)
- [ ] Implement SIMD optimization
- [ ] Add other hash algorithms (BLAKE3, xxHash)
- [ ] Support incremental hashing
- [ ] Add hash caching
- [ ] Implement content-addressable storage
- [ ] Add hash-based deduplication
- [ ] Support streaming hashing

---

#### 15. table.h + table.c (35 + 150 lines)
**Purpose**: Hash table implementation

**Table Structure**:
```c
struct Table {
    int count;       // Number of entries
    int capacity;    // Allocated capacity
    Entry* entries;  // Entry array
};

struct Entry {
    ObjString* key;  // Interned string key
    Value value;     // Any value
};
```

**Collision Resolution**:
- Open addressing with linear probing
- Tombstone entries (deleted but not removed)
- Load factor: 0.75 (resize when 75% full)

**Hashing**:
- Uses pre-computed hash from ObjString
- Modulo with capacity (power of 2)
- Linear probe on collision

**Operations**:
- `tableGet()`: O(1) average, O(n) worst
- `tableSet()`: O(1) average, O(n) worst
- `tableDelete()`: O(1) average, marks tombstone
- `tableFindString()`: String interning lookup

**Growth Strategy**:
- Doubles capacity when load factor exceeded
- Rehashes all entries
- Removes tombstones during resize

**GC Integration**:
- `tableMark()`: Marks all keys and values
- `tableRemoveWhite()`: Removes unmarked entries

**Critical Insights**:
- Only supports string keys (not general-purpose)
- No Robin Hood hashing
- No SIMD probing
- Tombstones never cleaned except during resize
- No concurrent access support

**Revolution Opportunities**:
- [ ] Support arbitrary key types
- [ ] Implement Robin Hood hashing
- [ ] Add SIMD probing for faster lookup
- [ ] Implement tombstone cleaning
- [ ] Add concurrent access support (lock-free)
- [ ] Optimize for small tables (inline storage)
- [ ] Add table iteration API
- [ ] Implement table serialization
- [ ] Add table statistics/profiling
- [ ] Support weak keys/values
- [ ] Implement persistent hash tables
- [ ] Add hash table compaction

---

## PART 3: COMPREHENSIVE STUDY ROADMAP

### PHASE 1: FOUNDATION UNDERSTANDING (Week 1-2)

#### Week 1: Value System & Memory
**Study Order**:
1. Read `common.h` - understand global configuration
2. Read `value.h` + `value.c` - master NaN-boxing
3. Read `memory.h` + `memory.c` - understand persistent heap

**Exercises**:
- [ ] Write a program to visualize NaN-boxed values in binary
- [ ] Implement a simple bump allocator
- [ ] Create a test that triggers heap exhaustion
- [ ] Measure memory-mapped file performance

**Key Questions**:
- How does NaN-boxing save memory?
- Why use memory-mapped files?
- What are the trade-offs of bump allocation?
- How does the write barrier work?

#### Week 2: Objects & GC
**Study Order**:
1. Read `object.h` + `object.c` - understand object model
2. Read `gc.h` + `gc.c` - understand garbage collection
3. Read `table.h` + `table.c` - understand hash tables

**Exercises**:
- [ ] Trace object allocation through the code
- [ ] Manually trigger GC and observe behavior
- [ ] Implement a simple mark-and-sweep GC
- [ ] Measure GC pause times

**Key Questions**:
- Why are objects 64-byte aligned?
- How does string interning work?
- What is tri-color marking?
- Why use open addressing?

---

### PHASE 2: BYTECODE ARCHITECTURE (Week 3-4)

#### Week 3: Instructions & Chunks
**Study Order**:
1. Read `opcode.h` - memorize instruction set
2. Read `chunk.h` + `chunk.c` - understand bytecode format
3. Read `hash.h` + `hash.c` - understand fingerprinting

**Exercises**:
- [ ] Write bytecode by hand for simple programs
- [ ] Implement a bytecode disassembler
- [ ] Create a bytecode verifier
- [ ] Measure RLE compression ratio

**Key Questions**:
- Why register-based instead of stack-based?
- How does RLE compression work?
- What are the benefits of 32-bit instructions?
- Why use SHA-256 for fingerprinting?

#### Week 4: Virtual Machine
**Study Order**:
1. Read `vm.h` + `vm.c` - understand VM architecture
2. Read `vm_interpret.c` - understand execution
3. Study computed goto dispatch

**Exercises**:
- [ ] Trace execution of a simple program
- [ ] Implement a simple register-based VM
- [ ] Measure dispatch overhead (goto vs switch)
- [ ] Profile hot instructions

**Key Questions**:
- How does computed goto improve performance?
- What is the call frame structure?
- How are closures implemented?
- How does the effect system work?

---

### PHASE 3: FRONTEND PIPELINE (Week 5-7)

#### Week 5: Lexical Analysis
**Study Order**:
1. Read `token.h` - understand token types
2. Read `scanner.h` + `scanner.c` - understand tokenization

**Exercises**:
- [ ] Implement a simple scanner
- [ ] Add support for new token types
- [ ] Measure scanning performance
- [ ] Test error handling

**Key Questions**:
- How does maximal munch work?
- Why use a trie for keywords?
- How are strings scanned?
- What is the unit syntax?

#### Week 6: Syntax Analysis
**Study Order**:
1. Read `parser.h` + `parser.c` - understand parsing
2. Read `ast.h` + `ast.c` - understand AST

**Exercises**:
- [ ] Implement a simple recursive descent parser
- [ ] Add support for new syntax
- [ ] Test error recovery
- [ ] Visualize AST structure

**Key Questions**:
- How does Pratt parsing work?
- What is panic mode recovery?
- How are expressions desugared?
- How is precedence handled?

#### Week 7: Code Generation
**Study Order**:
1. Read `compiler.h` + `compiler.c` - understand compilation

**Exercises**:
- [ ] Trace compilation of a function
- [ ] Implement register allocation
- [ ] Add a new opcode and compile it
- [ ] Measure compilation time

**Key Questions**:
- How does register allocation work?
- How are upvalues resolved?
- How is unit checking integrated?
- What optimizations are missing?

---

### PHASE 4: INTEGRATION & MASTERY (Week 8-10)

#### Week 8: End-to-End Tracing
**Exercises**:
- [ ] Trace a program from source to execution
- [ ] Identify bottlenecks in each phase
- [ ] Measure memory usage at each stage
- [ ] Profile the entire pipeline

#### Week 9: Advanced Features
**Study Topics**:
- Effect system implementation
- Reactive programming (entanglement)
- Pattern matching compilation
- Async/await implementation
- Genetic programming integration

**Exercises**:
- [ ] Implement a simple effect handler
- [ ] Create reactive variables
- [ ] Write pattern matching examples
- [ ] Test async functions

#### Week 10: Performance Analysis
**Exercises**:
- [ ] Benchmark against other VMs
- [ ] Identify optimization opportunities
- [ ] Measure cache performance
- [ ] Profile GC overhead

---

## PART 4: REVOLUTIONIZATION STRATEGY

### CRITICAL ISSUES TO ADDRESS

#### 1. Performance Bottlenecks
**Problems**:
- No register allocation optimization
- No inline caching
- Stop-the-world GC
- No JIT compilation in core
- Linear method dispatch

**Solutions**:
- Implement graph-coloring register allocation
- Add polymorphic inline caching
- Implement incremental/concurrent GC
- Add trace-based JIT
- Use vtables for method dispatch

#### 2. Memory Efficiency
**Problems**:
- No object pooling
- No small object optimization
- Bump allocator never reuses memory
- No compaction

**Solutions**:
- Implement object pools for common types
- Add small string optimization
- Implement generational GC with compaction
- Add memory pressure callbacks

#### 3. Compilation Quality
**Problems**:
- Single-pass compilation
- No optimization passes
- No constant folding
- No dead code elimination

**Solutions**:
- Implement multi-pass compilation
- Add SSA form
- Implement constant folding
- Add dead code elimination
- Implement inlining

#### 4. Developer Experience
**Problems**:
- Poor error messages
- No debugging support
- No profiling tools
- No IDE integration

**Solutions**:
- Add detailed error messages with suggestions
- Implement debugging protocol (DAP)
- Add built-in profiler
- Create language server protocol (LSP)

---

### MODERNIZATION PRIORITIES

#### Priority 1: Performance (Weeks 11-16)
**Goal**: 10x performance improvement

**Tasks**:
1. **Register Allocation** (Week 11):
   - Implement liveness analysis
   - Add graph-coloring allocation
   - Optimize register pressure

2. **Inline Caching** (Week 12):
   - Add monomorphic inline caches
   - Implement polymorphic inline caches
   - Add megamorphic fallback

3. **GC Optimization** (Week 13-14):
   - Implement generational GC
   - Add incremental marking
   - Implement concurrent sweeping
   - Add compaction

4. **JIT Compilation** (Week 15-16):
   - Implement trace recording
   - Add trace optimization
   - Implement native code generation
   - Add deoptimization support

#### Priority 2: Memory Efficiency (Weeks 17-20)
**Goal**: 50% memory reduction

**Tasks**:
1. **Object Optimization** (Week 17):
   - Implement object pooling
   - Add small string optimization
   - Optimize closure representation

2. **Heap Management** (Week 18):
   - Add arena allocators
   - Implement size classes
   - Add memory pressure handling

3. **Compression** (Week 19):
   - Implement pointer compression
   - Add bytecode compression
   - Optimize constant pools

4. **Profiling** (Week 20):
   - Add memory profiler
   - Implement heap snapshots
   - Add allocation tracking

#### Priority 3: Compilation Quality (Weeks 21-26)
**Goal**: Better code generation

**Tasks**:
1. **Multi-Pass Compilation** (Week 21-22):
   - Implement SSA form
   - Build control flow graph
   - Add data flow analysis

2. **Optimization Passes** (Week 23-24):
   - Implement constant folding
   - Add dead code elimination
   - Implement common subexpression elimination
   - Add loop optimization

3. **Advanced Features** (Week 25-26):
   - Implement inlining
   - Add escape analysis
   - Implement partial evaluation
   - Add profile-guided optimization

#### Priority 4: Developer Experience (Weeks 27-30)
**Goal**: World-class tooling

**Tasks**:
1. **Error Handling** (Week 27):
   - Improve error messages
   - Add error recovery
   - Implement suggestions
   - Add error context

2. **Debugging** (Week 28):
   - Implement DAP protocol
   - Add breakpoint support
   - Implement step debugging
   - Add variable inspection

3. **Profiling** (Week 29):
   - Add CPU profiler
   - Implement flame graphs
   - Add allocation profiler
   - Implement trace viewer

4. **IDE Integration** (Week 30):
   - Implement LSP server
   - Add syntax highlighting
   - Implement code completion
   - Add refactoring support

---

### ARCHITECTURAL IMPROVEMENTS

#### 1. Modularization
**Current**: Monolithic core
**Target**: Pluggable architecture

**Changes**:
- Separate VM from compiler
- Make GC pluggable
- Modularize object system
- Add extension API

#### 2. Concurrency
**Current**: Single-threaded
**Target**: Multi-threaded execution

**Changes**:
- Add thread-safe GC
- Implement work-stealing scheduler
- Add atomic operations
- Support parallel compilation

#### 3. Security
**Current**: No sandboxing
**Target**: Secure execution

**Changes**:
- Add capability-based security
- Implement memory protection
- Add code signing
- Support secure enclaves

#### 4. Observability
**Current**: Limited telemetry
**Target**: Full observability

**Changes**:
- Add structured logging
- Implement metrics collection
- Add distributed tracing
- Support profiling hooks

---

## PART 5: IMPLEMENTATION CHECKLIST

### Phase 1: Foundation (Weeks 1-10)
- [ ] Complete study of all core files
- [ ] Document all data structures
- [ ] Create test suite for core
- [ ] Benchmark baseline performance
- [ ] Identify all bottlenecks

### Phase 2: Quick Wins (Weeks 11-15)
- [ ] Implement register allocation
- [ ] Add inline caching
- [ ] Optimize hot paths
- [ ] Improve error messages
- [ ] Add basic profiling

### Phase 3: Major Refactoring (Weeks 16-25)
- [ ] Implement generational GC
- [ ] Add JIT compilation
- [ ] Implement SSA form
- [ ] Add optimization passes
- [ ] Refactor object system

### Phase 4: Advanced Features (Weeks 26-30)
- [ ] Implement debugging protocol
- [ ] Add LSP server
- [ ] Implement advanced optimizations
- [ ] Add concurrency support
- [ ] Implement security features

---

## PART 6: TESTING STRATEGY

### Unit Tests
**Coverage Target**: 90%

**Test Categories**:
- Value representation
- Memory allocation
- Object creation
- GC correctness
- Bytecode generation
- VM execution
- Scanner/parser
- Compiler

### Integration Tests
**Test Scenarios**:
- End-to-end compilation
- Complex programs
- Edge cases
- Error handling
- Performance regression

### Benchmarks
**Benchmark Suite**:
- Micro-benchmarks (individual ops)
- Macro-benchmarks (real programs)
- Memory benchmarks
- GC benchmarks
- Compilation benchmarks

---

## PART 7: DOCUMENTATION PLAN

### Code Documentation
- [ ] Add comprehensive comments
- [ ] Document all algorithms
- [ ] Explain design decisions
- [ ] Add usage examples

### Architecture Documentation
- [ ] Create architecture diagrams
- [ ] Document data flow
- [ ] Explain subsystem interactions
- [ ] Add performance characteristics

### Developer Guide
- [ ] Getting started guide
- [ ] Contribution guidelines
- [ ] Code style guide
- [ ] Testing guide

### User Documentation
- [ ] Language reference
- [ ] Standard library docs
- [ ] Tutorial series
- [ ] Best practices guide

---

## PART 8: SUCCESS METRICS

### Performance Metrics
- [ ] 10x faster execution
- [ ] 50% less memory usage
- [ ] 5x faster compilation
- [ ] 90% less GC pause time

### Quality Metrics
- [ ] 90% test coverage
- [ ] Zero critical bugs
- [ ] <1% crash rate
- [ ] 100% documentation coverage

### Developer Metrics
- [ ] <1 hour to get started
- [ ] <5 minutes to build
- [ ] <10 seconds for tests
- [ ] <1 day to understand core

---

## CONCLUSION

The Walia core is a sophisticated piece of engineering with:
- **6,500+ lines** of carefully crafted C code
- **NaN-boxing** for efficient value representation
- **Register-based VM** for performance
- **Persistent heap** for durability
- **Advanced features** (effects, reactive, genetic)

**Key Strengths**:
✓ Clean architecture
✓ Well-structured code
✓ Advanced features
✓ Good separation of concerns

**Key Weaknesses**:
✗ No optimization passes
✗ Stop-the-world GC
✗ Limited tooling
✗ Single-threaded only

**Revolution Potential**: 🚀🚀🚀🚀🚀 (5/5)

With focused effort over 30 weeks, you can transform this into a world-class language implementation that rivals Python, Ruby, and JavaScript in performance while maintaining its unique features.

**Next Steps**:
1. Start with Phase 1 (Foundation Understanding)
2. Build comprehensive test suite
3. Benchmark baseline performance
4. Begin implementing quick wins
5. Iterate and improve continuously

Good luck on your journey to revolutionize Walia! 🎯
