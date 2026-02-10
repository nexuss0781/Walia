## Walia Core Modernization – Task Plan

### 0. Overview

- **Goal**: Implement the `CORE_DEEP_DIVE_ROADMAP` for `src/core/` to reach a robust, "UFO-grade" VM and compiler (high performance, observability, and developer experience).
- **Scope**: All components described in `src/core/CORE_DEEP_DIVE_ROADMAP.md` (value system, memory/GC, objects, bytecode/VM, frontend pipeline, hashing/tables) plus cross-cutting performance and DX work.
- **Approach**: Phase-based delivery with small, reviewable PRs. Each task below should map to one or more GitHub issues and PRs.

---

### 1. Working Agreement (GitHub Best Practices)

- **Branching**
  - [ ] Use feature branches named `feature/core-<area>-<short-description>` (e.g. `feature/core-gc-generational`).
  - [ ] Keep branches focused on 1 logical unit of work (or tightly-related changes).

- **Issues & Tracking**
  - [ ] Create an issue for each major task or cluster of related subtasks.
  - [ ] Link PRs to issues using `Closes #<issue-number>` in the PR description.
  - [ ] Tag issues with labels such as `core`, `perf`, `gc`, `compiler`, `vm`, `dx`, `good-first-issue`.

- **Pull Requests**
  - [ ] One major concern per PR (e.g. "Implement generational GC nursery").
  - [ ] Include benchmarks and profiling numbers where relevant (before vs after).
  - [ ] Add unit/integration tests in the same PR that implements a feature.
  - [ ] Require at least one review and passing CI before merge.

- **Testing**
  - [ ] Maintain fast, deterministic tests suitable for CI.
  - [ ] Add regression tests for every bug fix.
  - [ ] Run full test + benchmark suites before large refactors or GC/VM changes.

- **Definition of Done (per task)**
  - [ ] Design documented in code comments and/or `docs/`.
  - [ ] Tests added/updated and passing.
  - [ ] Benchmarks (if performance-related) collected and recorded.
  - [ ] Public APIs and behavior changes documented.

---

### 2. Milestones / Phases (High-Level)

- **Phase 0 – Baseline & Safety Nets**
  - [x] Confirm `src/core/` matches the architecture described in `CORE_DEEP_DIVE_ROADMAP.md`.
  - [x] Establish or extend a core test suite (value, memory, objects, GC, bytecode, VM, scanner/parser/compiler).
  - [ ] Set up basic benchmarks for VM execution, GC, and compilation speed.
  - [ ] Add simple telemetry hooks for profiling (even if only compiled in debug builds).

- **Phase 1 – Foundation & Runtime (Weeks 1–10)**
  - [ ] Strengthen value system, memory, object model, GC, and tables.

- **Phase 2 – Bytecode & VM Performance (Weeks 11–16)**
  - [ ] Improve opcode set, chunk representation, and VM dispatch; add register allocation and inline caching.

- **Phase 3 – Frontend & Compilation Quality (Weeks 17–26)**
  - [ ] Improve scanner, parser, AST, and compiler (SSA, CFG, optimization passes, unit checking integration).

- **Phase 4 – Developer Experience & Tooling (Weeks 27–30)**
  - [ ] Error messages, debugging, profiling, LSP/IDE integration.

- **Phase 5 – Architecture, Concurrency, Security, Observability**
  - [ ] Modularize core subsystems, add multi-threading, sandboxing, and deep observability.

Each section below turns the roadmap’s “Revolution Opportunities” into concrete tasks grouped by subsystem.

---

### 3. Foundation Layer Tasks (`common`, `value`, `memory`, `object`, `gc`, `table`)

#### 3.1 `common.h`

- **Core tasks**
  - [ ] Introduce a **runtime-switchable NaN-boxing mode** (e.g. debug builds can toggle a slower, more explicit representation).
  - [ ] Replace the **global `hadError` flag** with VM-local or thread-local error state.
  - [ ] Add **compilation flags for optimization levels** (e.g. `WALIA_OPT_O0`, `WALIA_OPT_O2`, `WALIA_OPT_O3`).
  - [ ] Implement an **error context stack** (track file, line, phase, and message).

#### 3.2 `value.h` / `value.c`

- **Core tasks**
  - [x] Implement **full `printValue()`** that understands all object types (string, function, class, instance, etc.).
  - [x] Add **value type introspection helpers** (e.g. debug-only `walia_debug_describe_value(Value)`).
  - [ ] Benchmark and, if beneficial, **optimize `IS_*` macros** using compiler intrinsics or branch-hints.
- **Stretch tasks**
  - [ ] Prototype **SIMD-optimized operations** on `ValueArray` where safe.
  - [ ] Investigate **tagged pointer compression** for 32-bit builds and document trade-offs.

#### 3.3 `memory.h` / `memory.c`

- **GC/heap-enabling tasks (foundation)**
  - [ ] Document the current **persistent heap layout** (file size, bump allocator, card table usage).
  - [ ] Add **memory pressure hooks** instead of hard `exit()` on heap exhaustion (allow VM to react or gracefully abort).
  - [ ] Align **card table entries** to cache line size where practical (e.g. 64 bytes or well-justified alternative).
  - [ ] Add **lazy heap initialization** so small tools/tests don’t always allocate full 128MB.

- **Later GC integration tasks (tie-in with `gc.c`)**
  - [ ] Wire the **card table** into GC as a remembered set for generational collection.
  - [ ] Introduce **arena or size-class allocators** for common object sizes.
  - [ ] Consider support for **multiple heap files** (e.g. sharding by region/module).
  - [ ] Optionally support **heap encryption** for sensitive deployments (documented, opt-in).

#### 3.4 `object.h` / `object.c`

- **Object lifecycle and layout**
  - [ ] Implement **object pooling** for high-churn types (e.g. `ObjString`, `ObjList`, `ObjUpvalue` if profiling justifies).
  - [ ] Add optional **small string optimization (SSO)** to avoid heap allocations for very short strings.
  - [ ] Introduce **weak reference support** where needed (API to create weak references and integrate with GC).
  - [ ] Add **profiling metadata** for objects (e.g. allocation site, size category) in debug builds.

- **Advanced features**
  - [ ] Investigate **copy-on-write** semantics for clearly immutable object types (document invariants).
  - [ ] Support **object versioning** hooks for hot reload (e.g. version field, compatibility checks).
  - [ ] Improve **upvalue representation** for fewer allocations and better cache locality.
  - [ ] Explore **object sharding strategies** for future parallelism (document designs even if not fully implemented yet).

#### 3.5 `gc.h` / `gc.c`

- **Step 1 – Tighten current GC**
  - [ ] Add detailed **GC telemetry** (bytes collected, pause times, cycles per second) toggled via config.
  - [ ] Ensure **gray stack management** is safe and efficient (no pathological realloc patterns).
  - [ ] Add **object finalization APIs** for rare cases that require cleanup (carefully documented).

- **Step 2 – Generational & incremental GC**
  - [ ] Design a **nursery vs tenured** heap layout and document promotion policy.
  - [ ] Implement **generational GC**, using the card table as a remembered set.
  - [ ] Add **incremental marking** (slice GC work across VM steps with a tunable budget).
  - [ ] Optionally prototype **concurrent marking** (document platform constraints).
  - [ ] Implement **compaction** for at least some generations to reduce fragmentation.

- **Step 3 – Advanced features**
  - [ ] Introduce **GC pause time budgets/config** (target pause lengths; log violations).
  - [ ] Add support for **weak references** and ensure tables/objects integrate correctly.
  - [ ] Experiment with **alternative GC strategies** (copying, regions) as separate modes or experimental builds.

#### 3.6 `table.h` / `table.c`

- **Core improvements**
  - [ ] Optionally support **non-string keys** (while preserving string interning where used).
  - [ ] Implement **Robin Hood hashing** or another modern scheme to reduce worst-case lookup times.
  - [ ] Introduce **tombstone cleaning** strategy independent of full resize (to prevent pathological clusters).
  - [ ] Provide a **table iteration API** that is safe against GC and resizing.

- **Performance and GC**
  - [ ] Add **SIMD-accelerated probing** where compiler and CPU support exists.
  - [ ] Support **weak keys/values** for caches and memoization tables.
  - [ ] Implement **persistent/immutable tables** where copy-on-write is beneficial.
  - [ ] Add optional **table statistics/profiling** (load factor histograms, probe counts).

---

### 4. Bytecode Layer Tasks (`opcode`, `chunk`, `hash`, `vm*`)

#### 4.1 `opcode.h`

- **Instruction set evolution**
  - [ ] Audit existing **66 opcodes** and classify into hot vs cold, general vs special-case.
  - [ ] Design and implement **SIMD vector opcodes** (e.g. `VADD`, `VMUL`) where they simplify common numerical workloads.
  - [ ] Add **atomic operation opcodes** (e.g. `ATOMIC_ADD`, `CAS`) for future concurrency work.
  - [ ] Introduce **specialized string/array opcodes** (e.g. `STR_CONCAT`, `STR_SLICE`, `ARRAY_GET`, `ARRAY_SET`) that map to optimized paths.
  - [ ] Design **superinstructions** / fused ops for the most common pairs/triples of instructions.

- **Debugging & observability**
  - [ ] Add **debugging opcodes** (`BREAKPOINT`, `TRACE`) integrated with the debugger/profiler.
  - [ ] Add optional **branch prediction hints/prefetch** instructions or metadata fields where supported.

#### 4.2 `chunk.h` / `chunk.c`

- **Constant and instruction representation**
  - [ ] Implement **constant pool deduplication** to avoid storing identical constants multiple times.
  - [ ] Optionally add **instruction compression** (e.g. variable-length encoding for small immediates).
  - [ ] Improve **line number encoding** (delta or hybrid scheme) while preserving reasonable random access.
  - [ ] Extend debug info with **column numbers** and possibly file/module IDs.

- **Verification & optimization**
  - [ ] Implement a **bytecode verifier** (stack/register discipline, operand ranges, control flow sanity).
  - [ ] Add **simple bytecode-level optimization passes** (peephole, constant folding at chunk level).
  - [ ] Support **bytecode serialization/signing** for persistence and security.
  - [ ] Attach **profiling metadata** (per-instruction counters) when building in profiling mode.

#### 4.3 `hash.h` / `hash.c`

- **Performance & flexibility**
  - [ ] Add support for **hardware-accelerated SHA** where available (e.g. SHA extensions).
  - [ ] Add at least one **fast non-crypto hash** (e.g. BLAKE3 or xxHash) for non-security-sensitive use cases.
  - [ ] Support **streaming/incremental hashing** for large code/data segments.
  - [ ] Implement **hash caching** and/or content-addressable storage helper APIs.

---

### 5. VM Layer Tasks (`vm.h`, `vm.c`, `vm_interpret.c`)

- **Execution pipeline & dispatch**
  - [ ] Review and document the **computed goto dispatch** vs `switch` fallback trade-offs.
  - [ ] Add **instruction fusion** where the dispatch table can jump to combined handlers for common instruction pairs.
  - [ ] Make **watchdog integration** configurable and observable (log/publish when watchdog triggers).

- **Call frames & closures**
  - [ ] Optimize **closure representation** (fewer indirections/cache misses).
  - [ ] Improve **upvalue access paths** and sharing (profiling-driven).
  - [ ] Add **tail call optimization** support if compatible with language semantics.

- **Inline caching & method dispatch**
  - [ ] Implement **monomorphic inline caches** for property and method access.
  - [ ] Extend to **polymorphic inline caches (PICs)** and a **megamorphic fallback** path.
  - [ ] Introduce **vtables** or similar structures for fast method dispatch in hot paths.

- **JIT and adaptive optimization**
  - [ ] Implement **trace recording** (hot path identification via counters, e.g. call count, loop backedges).
  - [ ] Add a **trace-based JIT pipeline** (IR generation, simple optimizations, native code emission).
  - [ ] Implement **deoptimization** support (safe bailout from JIT to interpreter).
  - [ ] Add **adaptive optimization policies** based on profiling data.

- **Resilience & observability**
  - [ ] Add **stack overflow recovery mechanisms** (configurable policies: abort, grow, or recover).
  - [ ] Integrate **debugging protocol hooks (DAP)** at key VM events (breakpoints, stepping, variable inspection).
  - [ ] Extend **trace buffer** usage (time-travel debugging, exporting traces for tools).

---

### 6. Frontend Layer Tasks (`token`, `scanner`, `parser`, `ast`, `compiler`)

#### 6.1 `token.h`

- [ ] Add tokens for **template strings**, **raw strings**, and **multiline strings**.
- [ ] Add support for **numeric literal suffixes** (e.g. `u32`, `f64`) and **binary/octal/hex literals**.
- [ ] Introduce a **character literal** token type.
- [ ] Consider a **regex literal** token type if it fits language direction.
- [ ] Add tokens for **documentation comments** and **attributes/annotations**.

#### 6.2 `scanner.h` / `scanner.c`

- [ ] Track **column numbers** and **source file identifiers** for better diagnostics.
- [ ] Add **Unicode (UTF-8) support** and escape sequence validation.
- [ ] Support **scientific notation**, **hex/binary/octal literals**, and **numeric separators**.
- [ ] Implement **block comments** (`/* ... */`) with optional nesting.
- [ ] Replace verbose keyword checks with a **perfect hash or other compact keyword table**.
- [ ] Add **scanner error recovery** strategies and hooks for syntax highlighting / token streaming.

#### 6.3 `parser.h` / `parser.c`

- [ ] Replace hard-coded precedence with a **data-driven operator precedence table**.
- [ ] Improve **error recovery** (synchronization sets, more precise panic mode).
- [ ] Add **better error messages** with suggestions and source snippets.
- [ ] Implement a **syntax sugar desugaring pass** (e.g. for loops, pipes, entanglement) distinct from raw parsing.
- [ ] Add **pattern matching exhaustiveness checks**.
- [ ] Lay groundwork for **incremental parsing** in future IDE integration.

#### 6.4 `ast.h` / `ast.c`

- [ ] Introduce **arena allocation** for AST nodes to improve allocation locality and teardown speed.
- [ ] Add **parent pointers** or another way to traverse upwards in the tree.
- [ ] Implement a **visitor pattern** and a library of visitors (pretty-printer, analyzer, transformer).
- [ ] Add **source location ranges** (start/end positions) in addition to line numbers.
- [ ] Implement **AST serialization/deserialization** for tools and hot reload.
- [ ] Provide **AST diffing** utilities for incremental compilation and IDEs.

#### 6.5 `compiler.h` / `compiler.c`

- **SSA and optimization infrastructure**
  - [ ] Implement **multi-pass compilation**: front-end IR, SSA transformation, optimization passes, final codegen.
  - [ ] Build a **control flow graph (CFG)** and **data flow analysis** framework.
  - [ ] Add **SSA form** and a way to lower back into the register-based bytecode.

- **Optimization passes**
  - [ ] Implement **constant folding** and **strength reduction**.
  - [ ] Add **dead code elimination** and **common subexpression elimination**.
  - [ ] Implement **loop optimizations** (hoisting invariants, optional unrolling).
  - [ ] Add **inlining** based on profiling or heuristics.
  - [ ] Introduce **escape analysis** to avoid heap allocation where possible.
  - [ ] Prototype **partial evaluation** and **profile-guided optimization** hooks.

- **Register allocation and upvalues**
  - [ ] Implement **liveness analysis** for temporaries and locals.
  - [ ] Replace bump-only register allocation with **graph-coloring** or another global allocator.
  - [ ] Optimize **upvalue resolution** (e.g. hashed or indexed lookups instead of O(n) scans).

- **Unit checking & dimensions**
  - [ ] Document the **unit/dimension system** more thoroughly and add tests.
  - [ ] Ensure **unit checking** integrates cleanly with new optimization passes.

---

### 7. Developer Experience & Tooling

- **Error handling**
  - [ ] Implement a **centralized error reporting API** that consumes file/line/column, phase, and context stack.
  - [ ] Add **rich error messages** with hints and references to documentation.

- **Debugging**
  - [ ] Implement a **Debug Adapter Protocol (DAP)** server that hooks into breakpoints, stepping, and variable inspection.
  - [ ] Add **conditional breakpoints** and **watch expressions**.

- **Profiling**
  - [ ] Implement a **CPU profiler** (sampling or instrumentation-based).
  - [ ] Add **allocation and GC profilers**.
  - [ ] Support exporting **flame graphs** and **trace viewer** data.

- **IDE / LSP**
  - [ ] Implement a **Language Server Protocol (LSP)** server for Walia (diagnostics, completion, goto definition).
  - [ ] Integrate **syntax highlighting**, **hover docs**, and **refactoring support**.

---

### 8. Architecture, Concurrency, Security, Observability

- **Modularization**
  - [ ] Separate **VM**, **compiler**, and **GC** into more clearly defined modules with stable internal APIs.
  - [ ] Define an **extension/plugin API** for embedding and extending Walia.

- **Concurrency**
  - [ ] Design and implement a **work-stealing scheduler** and per-VM or per-fiber execution model.
  - [ ] Add **thread-safe GC** strategies appropriate for the chosen concurrency model.
  - [ ] Introduce **atomic operations** and concurrency-safe data structures where needed.

- **Security**
  - [ ] Add a **capability-based security model** for syscalls and dangerous operations.
  - [ ] Implement **code signing** and integrity checks for loaded bytecode/modules.
  - [ ] Explore integration with **secure enclaves** where relevant.

- **Observability**
  - [ ] Add **structured logging** with configurable sinks (stdout, file, external systems).
  - [ ] Integrate **metrics collection** (counters, histograms, timers) with tags.
  - [ ] Provide **distributed tracing hooks** and an API for external tracers.

---

### 9. Success Metrics (from CORE roadmap)

- **Performance**
  - [ ] 10x faster execution vs current baseline on agreed benchmark suite.
  - [ ] 50% reduction in memory usage for representative workloads.
  - [ ] 5x faster compilation for typical projects.
  - [ ] 90% reduction in worst-case GC pause times.

- **Quality**
  - [ ] ≥ 90% test coverage of `src/core/`.
  - [ ] Zero known critical bugs in core.
  - [ ] < 1% crash rate in real-world telemetry.

- **Developer Experience**
  - [ ] < 1 hour to get a new contributor productive.
  - [ ] < 5 minutes for a clean build on reference hardware.
  - [ ] < 10 seconds to run the core test suite.
  - [ ] < 1 day for an engineer to understand core architecture using docs + code.

