# Walia Master Implementation TODO

> **Purpose:** This is the single execution index for implementing Walia and WaliOS. Engineers should watch this file, work from the phases in order, and open the linked roadmap documents for the detailed design, file inventories, implementation notes, and subsystem-specific opportunities.

**Status:** Proposed implementation plan  
**Owner:** Walia engineering team  
**Repository:** `nexuss0781/Walia`  
**Canonical branch at planning time:** `main`  
**Last reviewed:** 2026-08-12

---

## 1. How to Use This Document

The roadmap files in this repository contain the detailed architectural intent. This document does not replace them; it turns them into an executable order of work. Each phase below identifies the authoritative roadmap, the files to inspect, the implementation clusters to complete, and the exit gate that must be satisfied before the next phase begins.

Use the following status markers consistently. A task is not complete merely because code exists: it is complete only when the implementation, regression tests, documentation, and—where relevant—benchmark evidence are present.

| Marker | Meaning |
|---|---|
| `[ ]` | Not started. |
| `[~]` | In progress on the current branch or work item. |
| `[x]` | Complete and verified against the phase exit gate. |
| `[!]` | Blocked, unsafe, or awaiting a design decision. Add a short explanation beside it. |
| `[?]` | Requires clarification or a decision before implementation. |

Every implementation change should update this file in the same pull request as the code. Keep work in small, reviewable slices, preserve the phase order unless a dependency is explicitly documented, and record the test or benchmark command used to verify the completed item.

> **Working rule:** Read the phase’s linked roadmap section first, inspect the current implementation second, implement one coherent slice third, and only then mark the corresponding item `[x]`.

---

## 2. Current Baseline and Planning Decisions

The repository already contains a working top-level C build, a `src/` language/runtime tree, a `waliaos/` kernel tree, integrated tests under `tests/`, and extensive subsystem roadmaps. The root Makefile currently exposes `make`, `make test`, `make debug`, `make clean`, and installation targets; the exact commands are defined in [`Makefile`](Makefile). The existing core task plan records the architecture and core test suite as established, while baseline benchmarks and profiling telemetry remain pending.[^1]

The top-level roadmap proposes a dependency-first sequence from the language core through VM, database, systems, standard library, SQL, web, tooling, root integration, WaliOS, drivers, libraries, and final synthesis.[^2] The root integration analysis supplies the cross-subsystem initialization order and memory/error boundaries that this master plan uses as its principal dependency rule.[^3]

One source file, [`src/ROOT_INTEGRATION_ROADMAP.md`](src/ROOT_INTEGRATION_ROADMAP.md), is currently empty. Until it is populated, use [`src/ROOT_INTEGRATION_DEEP_ANALYSIS.md`](src/ROOT_INTEGRATION_DEEP_ANALYSIS.md) as the authoritative root-integration design reference and use this document as the execution checklist. Do not infer completion from the empty placeholder.

The older [`CORE_TODO_LIST.md`](CORE_TODO_LIST.md) remains useful as a detailed inventory of core optimization opportunities, but it is not the progress authority. Progress is tracked here, with the linked subsystem roadmaps used for implementation detail.

### 2.1 Baseline tasks before feature work

- [ ] Run a clean host build with `make clean && make`; record compiler version, architecture, warnings, and any failure.
- [ ] Run `make test`; record the current passing/failing test count and preserve the first failure as a regression case.
- [ ] Run `make debug`; confirm that debug tracing does not change functional results.
- [ ] Verify that `src/core/`, `src/vm/`, `src/db/`, `src/sys/`, `src/lib/`, `src/sql/`, `src/web/`, `src/tooling/`, and the WaliOS subtrees compile or are intentionally excluded with a documented reason.
- [ ] Reconcile the file inventories in R5–R16 with the current tree. Track missing files under the phase where their interface is designed.
- [ ] Establish a reproducible benchmark command for interpreter execution, compilation latency, allocation/GC behavior, and the integrated test suite.
- [ ] Add a minimal debug-only telemetry path for phase duration, allocation counts, GC cycles, and test results.
- [ ] Add a root-level progress log or release note entry for every completed phase gate.
- [ ] Decide whether the project will keep the current AVX-512-by-default build flags or introduce capability detection and portable fallbacks before performance work begins.

**Baseline exit gate:** A clean build, a recorded test baseline, a repeatable benchmark harness, and a documented list of current build/test failures exist before any optimization or large subsystem expansion is accepted.

---

## 3. Roadmap Source Register

The following register is the map from this single TODO to the detailed source material. Open the linked roadmap before implementing the corresponding phase.

| ID | Authoritative file | Scope used by this TODO |
|---|---|---|
| R0 | [`REVOLUTIONIZE_ROADMAP.md`](REVOLUTIONIZE_ROADMAP.md) | Repository-wide dependency order, 70-week study/implementation sequence, and final synthesis. |
| R1 | [`CORE_TODO_LIST.md`](CORE_TODO_LIST.md) | Legacy expanded core opportunity inventory; use as a supplementary checklist only. |
| R2 | [`src/tooling/TASK.md`](src/tooling/TASK.md) | Existing core modernization task contract, review rules, definition of done, and core phase framing. |
| R3 | [`src/ROOT_INTEGRATION_DEEP_ANALYSIS.md`](src/ROOT_INTEGRATION_DEEP_ANALYSIS.md) | Root application/runtime integration, initialization order, error propagation, persistence, and testing patterns. |
| R4 | [`src/ROOT_INTEGRATION_ROADMAP.md`](src/ROOT_INTEGRATION_ROADMAP.md) | Reserved root-integration roadmap placeholder; currently empty, so R3 and this document govern the work. |
| R5 | [`src/core/CORE_DEEP_DIVE_ROADMAP.md`](src/core/CORE_DEEP_DIVE_ROADMAP.md) | Core value, memory, object, GC, bytecode, VM, frontend, compiler, hash, table, tests, and optimization strategy. |
| R6 | [`src/vm/VM_REVOLUTION_ROADMAP.md`](src/vm/VM_REVOLUTION_ROADMAP.md) | Dispatch, shapes, caches, async, reactive/ripple systems, genetic programming, system operations, database fusion, JIT, profiling, and debugging. |
| R7 | [`src/db/DB_REVOLUTION_ROADMAP.md`](src/db/DB_REVOLUTION_ROADMAP.md) | Storage, transactions, indexes, relational/document models, vectors, tensors, query engines, security, temporal data, and parallel execution. |
| R8 | [`src/sys/SYS_REVOLUTION_ROADMAP.md`](src/sys/SYS_REVOLUTION_ROADMAP.md) | Allocators, cache/hardware probing, FFI/JIT, assembly, syscalls, event reactor, system types, and dimensional typing. |
| R9 | [`src/lib/LIB_REVOLUTION_ROADMAP.md`](src/lib/LIB_REVOLUTION_ROADMAP.md) | Collections, iterators, streams, async, timers, networking, database APIs, tensor/vector/evolution APIs, UI/reactive APIs, and transcoding. |
| R10 | [`src/sql/SQL_REVOLUTION_ROADMAP.md`](src/sql/SQL_REVOLUTION_ROADMAP.md) | SQL parser/compiler completion, optimization, execution features, and database/VM integration. |
| R11 | [`src/web/WEB_REVOLUTION_ROADMAP.md`](src/web/WEB_REVOLUTION_ROADMAP.md) | Surfaces, SIMD graphics, rasterization, fonts, reactivity, events, transcoding, GPU acceleration, and UI tooling. |
| R12 | [`src/tooling/TOOLING_REVOLUTION_ROADMAP.md`](src/tooling/TOOLING_REVOLUTION_ROADMAP.md) | Telemetry/Nexus, Oracle documentation, hash/registry/history, ledger/healer, simulation, MPPA analysis, reporting, and UI. |
| R13 | [`waliaos/WALIAOS_BUILD_REVOLUTION_ROADMAP.md`](waliaos/WALIAOS_BUILD_REVOLUTION_ROADMAP.md) | Boot assembly, linker, Makefile, 32/64-bit migration, security hardening, and build variants. |
| R14 | [`waliaos/core/WALIAOS_CORE_REVOLUTION_ROADMAP.md`](waliaos/core/WALIAOS_CORE_REVOLUTION_ROADMAP.md) | Kernel initialization, hardware substrate, interrupts, memory, scheduling, storage, topology, watchdog, shell, and security. |
| R15 | [`waliaos/drivers/WALIAOS_DRIVERS_REVOLUTION_ROADMAP.md`](waliaos/drivers/WALIAOS_DRIVERS_REVOLUTION_ROADMAP.md) | Timers, serial/VGA, AHCI/NVMe, keyboard, PCI, MSR, and thermal drivers. |
| R16 | [`waliaos/lib/WALIAOS_LIB_REVOLUTION_ROADMAP.md`](waliaos/lib/WALIAOS_LIB_REVOLUTION_ROADMAP.md) | IPC rings, compression, deduplication, fabric interconnect, metal utilities, PMMO storage, security gate, and shell core. |
| R17 | [`Documentation/EAII.md`](Documentation/EAII.md) and [`Documentation/`](Documentation/) | Language concepts, system syntax, database architecture, VM behavior, user-facing contracts, and study/reference material. |

The subsystem roadmaps contain their own file inventories. When a roadmap names a file that is not yet present, create it only after the phase design and public interface have been reviewed; do not create empty placeholders merely to satisfy an inventory count.

---

## 4. Dependency Order and Global Delivery Gates

The order below is intentional. The runtime needs stable values, allocation, object ownership, and error state before advanced GC or VM optimization. The database needs stable values, persistence boundaries, paging, and concurrency primitives. The standard library and SQL layers should consume stable runtime/database APIs rather than defining incompatible duplicates. WaliOS and hardware work should follow validated host-side behavior, because bare-metal debugging is more expensive and less observable.

| Gate | Must be true before advancing | Primary references |
|---|---|---|
| G0: Safety net | Build, test, benchmark, and telemetry baselines are reproducible. | R2, R3, Makefile |
| G1: Core contract | Values, memory, objects, GC, tables, bytecode, and frontend have documented invariants and regression tests. | R5, R1 |
| G2: Runtime contract | Interpreter behavior is stable and observable; optimized paths have differential tests against the baseline interpreter. | R5, R6 |
| G3: Data contract | Paging, row/document storage, indexes, transactions, and recovery semantics are tested before higher query features. | R7, R10 |
| G4: Systems contract | Allocators, FFI, syscalls, event loop, and type/dimension boundaries are capability-checked and failure-safe. | R8 |
| G5: Product API contract | Standard library, SQL, web, and tooling APIs depend on stable lower layers and have end-to-end tests. | R9, R10, R11, R12 |
| G6: Integration contract | Root initialization order, error propagation, persistence, native registration, and shutdown are deterministic. | R3, R4 |
| G7: Bare-metal contract | Boot, linker, kernel, drivers, storage, security, and recovery work in a documented emulator/hardware matrix. | R13–R16 |
| G8: Release contract | Documentation, examples, benchmarks, compatibility claims, security review, and release artifacts agree with the code. | R0, R17 |

### 4.1 Definition of done for every implementation item

- [ ] The linked roadmap section has been read and the design decision is reflected in code comments or architecture documentation.
- [ ] The implementation has a focused unit, integration, or regression test.
- [ ] Error paths and cleanup paths are exercised; no feature relies on a commented-out critical operation.
- [ ] Public or cross-subsystem API changes are documented.
- [ ] Performance-sensitive work has before/after measurements on the agreed benchmark suite.
- [ ] The change passes the relevant fast tests and the full test suite when the phase gate requires it.
- [ ] This file is updated with the status, evidence command, and any known follow-up.

---

# 5. Phased Implementation Roadmap

The top-level roadmap describes approximately 70 weeks of focused work, while subsystem roadmaps contain more detailed 12-, 14-, 30-, or 36-week plans. The week numbers below preserve the repository’s intended ordering, not a binding calendar promise. A phase may be split into multiple pull requests; it must not be skipped without recording the dependency and risk.

---

## Phase 0 — Baseline, Safety Nets, and Inventory

**Timing:** Before Week 1  
**Goal:** Establish a trustworthy starting point and prevent the engineer from implementing against stale or contradictory assumptions.  
**Read first:** R2, R3, R0, Makefile, README.

### TODO

- [ ] Complete the baseline tasks in §2.1 and attach the results to the first implementation issue.
- [ ] Verify that all source subtrees compile or are intentionally excluded with a documented reason.
- [ ] Reconcile the file inventories in R5–R16 with the current tree. Track missing files under the phase where their interface is designed.
- [ ] Add or confirm deterministic test fixtures for `tests/core_suite.wal` and any C-level unit tests required by the core roadmap.
- [ ] Record warnings, sanitizers, architecture flags, and unsupported host capabilities in a baseline report.
- [ ] Add a lightweight benchmark runner and store baseline numbers for interpreter execution, compilation, allocation/GC, and integrated tests.
- [ ] Add debug-only telemetry hooks for phase timing, allocations, GC cycles, and failures without changing release semantics.
- [ ] Define the branch/PR naming and review policy from R2 for the remainder of the project.
- [ ] Decide the policy for experimental features such as GPU backends, distributed execution, machine-learning repair, and bare-metal hardware access; experimental work must be capability-gated and must not silently become a release dependency.

**Exit gate G0:** The project builds or has a documented baseline failure, tests produce a reproducible result, benchmarks run, and all later work has a known verification command.

---

## Phase 1 — Language Core Foundation

**Timing:** Weeks 1–7  
**Goal:** Make the value/memory/object/GC model, bytecode, interpreter, frontend, compiler, and utility structures correct and observable before optimizing them.  
**Read first:** R5, R1, R2, and the `src/core/` file implementations.

### 1A. Foundation layer: values, memory, objects, GC, and tables

- [ ] `src/core/common.h`: make NaN-boxing runtime-switchable for debug builds; replace global error state with VM-local or thread-local state; add optimization-level flags; implement an error-context stack.
- [ ] `src/core/value.h` and `value.c`: complete value introspection and full object printing; benchmark `IS_*` macros; prototype SIMD `ValueArray` operations only when correctness and portability are demonstrated; document any pointer-compression decision.
- [ ] `src/core/memory.h` and `memory.c`: document heap layout; replace hard termination on exhaustion with pressure callbacks or a controlled VM failure; add lazy initialization; align card-table structures; design arena/size-class allocation and remembered-set integration.
- [ ] `src/core/object.h` and `object.c`: add profiling metadata, object pooling where profiling justifies it, SSO where invariants are clear, weak-reference hooks, upvalue layout improvements, and carefully documented copy-on-write/versioning experiments.
- [ ] `src/core/gc.h` and `gc.c`: instrument current mark/sweep; make gray-stack behavior robust; add finalization APIs; design and implement nursery/tenured promotion; wire card marking; add incremental collection with a pause budget; add weak references and optional compaction only after the baseline collector is regression-tested.
- [ ] `src/core/table.h` and `table.c`: add safe iteration, tombstone cleanup, statistics, and GC-safe resizing; evaluate Robin Hood hashing, non-string keys, weak keys/values, and persistent tables as separate reviewable changes.
- [ ] Add tests for value encoding, heap exhaustion, object reachability, weak references, table resize/iteration, GC pause telemetry, and cleanup after failures.

### 1B. Bytecode and interpreter

- [ ] `src/core/opcode.h`: audit and classify the existing opcode set; specify specialized string/array operations, atomics, SIMD operations, debugging opcodes, and superinstructions without breaking serialization or compatibility.
- [ ] `src/core/chunk.h` and `chunk.c`: deduplicate constants; improve source positions with columns and module IDs; add bytecode verification; add peephole/constant-folding passes; define serialization/signing and optional profiling metadata.
- [ ] `src/core/hash.h` and `hash.c`: add streaming/incremental hashing, a fast non-cryptographic hash for non-security paths, caching, and content-addressable helpers while retaining a cryptographic path for integrity checks.
- [ ] `src/core/vm.h`, `vm.c`, and `vm_interpret.c`: document computed-goto versus switch dispatch; preserve a correct fallback; improve closure/upvalue access; add stack-overflow policy; expose watchdog and trace hooks; create differential tests for every optimized dispatch path.
- [ ] Write a bytecode disassembler and verifier test suite; hand-author bytecode fixtures for literals, calls, closures, classes, effects, and failure cases.

### 1C. Frontend and compiler

- [ ] `src/core/token.h`: add template/raw/multiline strings, numeric bases/suffixes, character literals, documentation comments, and attributes according to an approved grammar.
- [ ] `src/core/scanner.h` and `scanner.c`: add columns and source IDs, UTF-8 validation, scientific/base-separated numbers, nested block comments if accepted, compact keyword dispatch, and scanner recovery.
- [ ] `src/core/parser.h` and `parser.c`: use a data-driven precedence table; improve synchronization and multi-error reporting; add diagnostic suggestions; separate syntax desugaring from parsing; add pattern exhaustiveness checks and incremental-parsing hooks.
- [ ] `src/core/ast.h` and `ast.c`: add arena allocation, source ranges, parent traversal, visitor/transformer APIs, serialization, cloning, validation, pretty-printing, and AST diffing.
- [ ] `src/core/compiler.h` and `compiler.c`: introduce multi-pass compilation, CFG/data-flow analysis, SSA lowering, constant folding, dead-code elimination, CSE, loop optimization, inlining, escape analysis, profile hooks, liveness analysis, and a global register allocator in dependency order.
- [ ] Add parser/compiler tests for valid syntax, malformed input, recovery, source locations, AST transforms, closure capture, units/dimensions, optimization equivalence, and compile-time diagnostics.

### Phase 1 exit gate G1

- [ ] Core invariants are documented: value representation, ownership/marking, object layout, table semantics, bytecode validity, parser locations, compiler scope rules, and error propagation.
- [ ] The baseline interpreter and any optimized interpreter paths agree on the core conformance suite.
- [ ] GC tests cover roots, cycles, promotion, incremental slices, weak references, and failure cleanup.
- [ ] A core benchmark report exists for execution, compilation, allocation, and GC, with no unmeasured optimization accepted.
- [ ] Core test coverage and known critical-bug status are recorded against the targets in R5/R2.

---

## Phase 2 — Advanced VM Runtime

**Timing:** Weeks 8–13  
**Goal:** Add optimization infrastructure and advanced runtime features on top of a stable core contract.  
**Read first:** R6, then R5 VM tasks and the relevant `src/vm/` files.

### 2A. Dispatch, shapes, operators, and method caches

- [ ] `src/vm/vm_shape.*`: implement shape statistics, versioning, migration, validation, serialization, deprecation, and garbage collection with tests for class mutation.
- [ ] `src/vm/vm_method_cache.*` and `vm_dispatch.*`: add LRU/per-class caches, monomorphic/PIC/megamorphic paths, cache warming, versioning, statistics, profiling, and debug validation.
- [ ] `src/vm/vm_magic_names.*` and `vm_operators.*`: complete the magic-method and operator protocols, including truthiness, formatting, context management, async hooks, augmented/reversed operations, matrix/power/floor-division/bitwise semantics where approved by the language specification.
- [ ] `src/vm/vm_iterator.c`: implement synchronous, generator, async, lazy, compositional, and debuggable iterator behavior.

### 2B. Async and reactive execution

- [ ] `src/vm/vm_async_core.*`: implement multiple waiters, cancellation, timeouts, chaining, composition (`all`/`race`/`any`), profiling, and deterministic failure propagation.
- [ ] `src/vm/vm_entangle.*` and `vm_ripple.*`: add bounded source/group semantics, priorities, conditional dependencies, cycle detection, batching, cancellation, validation, visualization, and profiling. Treat “quantum” naming as a reactive dependency mechanism unless a separate technical specification says otherwise.
- [ ] `src/vm/vm_traps.*`: add trap composition, priority, validation, caching, serialization, and debugging.

### 2C. Systems and database fusion

- [ ] `src/vm/vm_ptr_ops.*` and `vm_sys_ops.*`: add pointer arithmetic/casting/validation, optional bounds checks, overflow detection, endianness, saturating arithmetic, atomics, barriers, and lock-free operations with platform tests.
- [ ] `src/vm/vm_db_ops.*` and `vm_pager_link.*`: add vectorized database operations, lazy loading, prefetching, batch operations, caching, compression/encryption hooks, validation, versioning, and replication/conflict boundaries.
- [ ] `src/vm/vm_native_compiler.*`: define the IR/native code contract, support the agreed opcode subset first, then register allocation, scheduling, inlining, constant folding, DCE, peephole optimization, profile-guided optimization, deoptimization, OSR, and code caching.
- [ ] `src/vm/vm_profiler.h`: implement sampling, type/branch/memory/cache profiling, adaptive thresholds, visualization export, and a stable profiler API.

### Phase 2 exit gate G2

- [ ] Every optimization has a correctness oracle against the baseline interpreter.
- [ ] Cache invalidation and deoptimization tests cover class mutation, type changes, exceptions, async cancellation, and database boundary failures.
- [ ] JIT-generated code is capability-gated, W^X-aware, and never required for correctness.
- [ ] Async/reactive operations have deterministic scheduling tests and cancellation cleanup.
- [ ] Profiling data can explain a benchmark result rather than merely being emitted.

---

## Phase 3 — Database Engine

**Timing:** Weeks 14–24  
**Goal:** Build a durable, testable storage and query engine in layers, starting with physical storage and ending with query/security/parallel execution.  
**Read first:** R7; use R10 only for the SQL surface that calls the database engine.

### 3A. Foundation fixes and observability

- [ ] Audit all `src/db/` files for pointer bugs, missing error handling, leaks, unchecked bounds, and incomplete cleanup; prioritize the `db_dispatch.c` worker-loop pointer issue identified by R7.
- [ ] Create database unit, integration, stress, correctness, crash-recovery, and benchmark suites before advanced storage work.
- [ ] Add page, buffer-pool, transaction, index, query, vector, and security metrics.
- [ ] Document page size, page header, ownership, locking, recovery, and memory-mapping invariants.

### 3B. Physical storage and transactions

- [ ] `db_common.*`, `db_pager.*`, `db_slotted.*`, `db_row.*`: complete type/page metadata, variable-length storage, checksums, compression/encryption metadata, overflow pages, slot compaction/versioning, row integrity, and columnar options.
- [ ] `db_shadow.*`: implement WAL, undo/redo, isolation levels, savepoints, transaction timeouts, deadlock detection, and a clear boundary for distributed transactions.
- [ ] `db_pool.*`: implement adaptive sizing, replacement policy, prefetching, NUMA behavior, lock-free paths where justified, migration, and statistics.
- [ ] Implement crash recovery and point-in-time correctness tests before exposing replication or remote storage.

### 3C. Indexes and data models

- [ ] `db_btree.*` and `db_hash.*`: add bulk loading, composite/variable-length keys, prefix/key compression, concurrent B-link or equivalent design, bloom filters, compaction, statistics, and collision-safe deletion.
- [ ] `db_auto_index.*`: add incremental/partial/covering/multi-column indexes, usage statistics, and safe automatic dropping; defer machine-learning prediction until a deterministic baseline exists.
- [ ] `db_schema.*`, `db_catalog.*`, `db_constraint.*`, `db_relational.*`, and `db_document.*`: implement schema evolution, catalog indexes/versioning, constraints, referential integrity, computed/default values, triggers, JSON paths, partial updates, validation schemas, and document versioning.

### 3D. Vector, tensor, and neural features

- [ ] `db_vector.*`, `db_vector_math.*`, `db_vector_match.*`, `db_hnsw.*`, `db_quantize.*`, and `db_neural_stream.*`: establish scalar correctness first, then SIMD, mixed precision, compression, adaptive/learned quantization, dynamic HNSW updates/deletion, graph optimization, approximate matching, cache warming, and predictive prefetching.
- [ ] `db_tensor_math.*` and `db_tensor_broadcast.*`: implement shape/broadcast correctness, sparse/mixed-precision behavior, fusion, compression, and optional GPU backends behind capability detection.
- [ ] Record recall, latency, memory, and indexing benchmarks; do not claim scale targets without a reproducible dataset and hardware description.

### 3E. Query, security, temporal, and parallel execution

- [ ] `db_sqe_sql.*` and `db_sqe_nosql.*`: implement query planning, index selection, cost/statistics hooks, joins, aggregation pushdown, streaming, change data capture, graph/full-text/geospatial/time-series extension points, and query caching.
- [ ] `db_sentry_jit.*` and `db_sentry_mapper.*`: add expression JIT, vectorized aggregations, runtime specialization, ORM relationship mapping, change tracking, and migration generation after the non-JIT execution path is correct.
- [ ] `db_sentry_security.*`: add attribute/row-level access control, audit logging, masking, multi-tenancy boundaries, encryption hooks, compliance-oriented policy tests, and fine-grained dynamic policy evaluation.
- [ ] `db_sentry_temporal.*`: implement snapshots, incremental/compressed storage, encryption/replication hooks, temporal joins, and bi-temporal semantics.
- [ ] `db_dispatch.*`: repair the critical pointer issue, then implement CPU affinity, NUMA/priority scheduling, backpressure, adaptive parallelism, pipelining, vectorized workers, and a safe distributed boundary.

### Phase 3 exit gate G3

- [ ] Storage survives restart/crash tests according to the documented durability contract.
- [ ] Transactions, isolation, constraints, indexes, and recovery have deterministic tests.
- [ ] Query results agree across scalar, SIMD, parallel, and optional JIT paths.
- [ ] Security and temporal semantics are tested independently of performance features.
- [ ] Database benchmarks report throughput, p99 latency, memory, and recovery time using reproducible fixtures.

---

## Phase 4 — Systems Programming Layer

**Timing:** Weeks 25–30  
**Goal:** Provide safe, capability-aware low-level primitives for allocation, FFI, hardware access, system types, and dimensional typing.  
**Read first:** R8 and the system-syntax references in R17.

### TODO

- [ ] `sys_allocator.*`: implement size classes, segregated free lists, pooling/slabs, thread-local caches, alignment, guards, profiling, and controlled failure behavior; defer NUMA/PMEM/encryption until the base allocator is verified.
- [ ] `sys_cache_align.h`, `sys_hardware_probe.*`, and `sys_layout.*`: detect cache/topology capabilities, expose portable alignment/prefetch helpers, validate layouts, and provide debug visualization without making unsupported CPU features mandatory.
- [ ] `sys_ffi_types.*`, `sys_ffi_alloc.*`, `sys_ffi_jit.*`, and `sys_ffi_dynamic.*`: define ABI/type rules, executable-memory W^X transitions, callbacks/variadics/struct passing where supported, code cache/relocation/signing boundaries, dynamic library versioning, unloading, isolation, and capability checks.
- [ ] `sys_assembler.*` and `sys_kernel.*`: expand instruction/syscall coverage only with validation, tracing, filtering, sandboxing, auditing, and architecture-specific tests.
- [ ] `sys_event_reactor.*`: implement the host baseline first, then io_uring/kqueue/IOCP adapters, registered buffers/files, zero-copy paths, async file/network/disk/timer operations, and fallback behavior.
- [ ] `sys_types.*` and `sys_parser_ext.*`: add vector/atomic/volatile/const/restrict/annotation support, type inference/checking/coercion, generics, bounds, unification, substitution, and diagnostics in coordination with the core compiler.
- [ ] `sys_units_registry.*`, `sys_units_math.*`, and `sys_units_check.*`: implement unit registration, systems/prefixes/aliases, conversion, normalization, dimensional algebra, compile-time/runtime checks, warnings, suggestions, examples, and benchmarks.
- [ ] Add sanitizer, fuzz, ABI, cross-architecture, and unsupported-capability tests.

**Exit gate G4:** Low-level operations fail closed, expose capabilities, preserve W^X and memory safety boundaries, and have host fallback tests. Unit/dimension semantics are integrated into compiler diagnostics without breaking ordinary numeric code.

---

## Phase 5 — Standard Library and Runtime APIs

**Timing:** Weeks 31–36  
**Goal:** Expose stable, ergonomic library APIs over the core, VM, database, systems, and web layers.  
**Read first:** R9 and the public language/reference material in R17.

### TODO

- [ ] `stdlib_core.c`: complete math, formatting, regex/glob, Unicode, interpolation, builders, lazy strings, and explicit security/compression boundaries.
- [ ] `lib_collections.*`, `lib_iterator.*`, and `lib_stream.*`: implement collections, persistent/immutable options, sorted/priority/deque/circular/trie/B-tree/LSM structures as justified, iterator adapters/fusion, lazy evaluation, stream backpressure, operators, windowing, joins, aggregation, replay, and branching.
- [ ] `lib_async_orchestrator.*` and `lib_timer.*`: add cancellation, timeouts, retries, error aggregation, circuit breakers, rate limiting, bulkheads, high-resolution timers, coalescing, migration, persistence, and recovery.
- [ ] `lib_web_io.c`: implement a safe networking baseline, then TLS, HTTP/2, WebSockets, pooling, keep-alive, zero-copy, multicast, and QUIC behind explicit capability checks.
- [ ] `lib_sql.*`, `lib_nosql.*`, `lib_sqe.*`, and `lib_sentry.*`: bind the public API to the tested database/query/security contracts with transactions, migrations, prepared statements, and error translation.
- [ ] `lib_vector.*`, `lib_tensor.*`, `lib_tensor_core.*`, `lib_evolution.*`, and `lib_evolution_sentry.*`: expose scalar-correct computation first, then autodiff/GPU/distributed/quantized/pruned/federated/RL features as opt-in extensions.
- [ ] `lib_reflection.*`, `lib_trait_bridge.*`, `lib_reactive.c`, `lib_ui_bridge.c`, and `lib_kernel_base.c`: define reflection, traits, reactivity, UI, and kernel bridges with stable ownership and lifecycle rules.
- [ ] `lib_trans_api.*`: add transcoding formats incrementally, with schema validation, streaming, compatibility, and error recovery.
- [ ] Add API-level examples, conformance tests, documentation snippets, and failure-injection tests.

**Exit gate:** Standard library APIs are versioned, documented, tested against stable lower-layer contracts, and do not leak internal allocator, GC, database, or platform assumptions into user code.

---

## Phase 6 — SQL Subsystem

**Timing:** Week 37, with the SQL roadmap’s 12-week feature sequence applied as dependency work  
**Goal:** Complete SQL parsing, compilation, optimization, execution, transaction control, and integration with the database and VM.  
**Read first:** R10, R7, R5, and the SQL-related library API in R9.

### TODO

- [ ] `src/sql/sql_parser.*`: implement DML (`INSERT`, `UPDATE`, `DELETE`), DDL, indexes, subqueries/CTEs, window functions, set operations, `CASE`, aggregates, `HAVING`, `LIMIT/OFFSET`, `DISTINCT`, `IN/EXISTS`, `LIKE`, date/time/string functions, casts, NULLs, constraints, triggers, views, and materialized views according to the approved grammar.
- [ ] `src/sql/sql_compiler.*`: compile scans, predicates, projections, joins, aggregates, and transactions into the database/VM execution contracts.
- [ ] Add index selection, nested-loop/hash/merge joins, cost/statistics collection, predicate/projection pushdown, constant folding, CSE, vectorized/SIMD execution, parallel/adaptive execution, query profiling, explain plans, hints, and plan stability.
- [ ] Add `BEGIN`, `COMMIT`, `ROLLBACK`, isolation, locking/MVCC, deadlock detection, cancellation/timeouts, resource limits, connection pooling, prepared statements, batch execution, cursors, streaming results, pagination, and result caching.
- [ ] Build parser conformance tests, compiler golden tests, optimizer equivalence tests, transaction/recovery tests, security tests, and explain-plan snapshots.

**Exit gate:** SQL parser, compiler, and execution paths produce correct results over the tested storage engine, and every optimizer can be disabled for differential comparison.

---

## Phase 7 — Web, Graphics, and UI Runtime

**Timing:** Weeks 38–41  
**Goal:** Build a correct host-side surface/graphics/event/transcoding layer before GPU and enterprise features.  
**Read first:** R11 and the web/system event references in R8/R9.

### TODO

- [ ] `wld_surface_common.*`, `web_surface.*`, and `web_simd_core.*`: implement surface lifecycle, multiple resolutions, color spaces, gamma/HDR, clipping, dirty rectangles, rotation/scaling, buffering/vsync, blend modes, transforms, convolution, gradients, and portable scalar fallbacks.
- [ ] `web_raster_tiler.*`: implement z-order, occlusion/frustum culling, transparency layers, LOD, instancing, batching, render queues, deferred rendering, and tile lighting with visual regression tests.
- [ ] `web_font_sdf.*`: implement MSDF, hinting, ligatures, kerning, shaping, BiDi, emoji, effects, variable fonts, fallback chains, and line layout with glyph and layout tests.
- [ ] `web_proxy.*` and `web_traits.*`: implement computed properties, watchers, deep observation, batching, transactions, undo/redo, time travel, reactive collections, dependency tracking, trait composition/conflict handling, requirements, aliases, exclusion, inheritance, and versioning.
- [ ] `wld_event_common.*` and `wld_event_loop.*`: implement event priorities/filtering/batching/replay/versioning, host event loop, adaptive polling, multi-threading, io_uring/IOCP adapters, pooling, TLS/HTTP2/WebSocket hooks, and hot reload boundaries.
- [ ] `web_trans_common.*`, `web_trans_lexer.*`, `web_trans_injector.*`, and `web_trans_wbin.*`: add UTF-8/escape/number handling, lazy/incremental parsing, schema validation, streaming injection, compression, encryption, checksums, delta encoding, flow control, multiplexing, and compatibility rules.
- [ ] Add visual regression, event determinism, font shaping, memory leak, stress, and performance tests before GPU backends.
- [ ] Add Vulkan/Metal/DirectX or other GPU backends only as optional adapters with a scalar/SIMD reference implementation.

**Exit gate:** The web layer meets its agreed host test matrix, preserves a portable fallback, and can explain frame time, event latency, memory, and rendering regressions.

---

## Phase 8 — Tooling, Oracle, Analysis, and Repair

**Timing:** Weeks 42–46  
**Goal:** Make the engineering system observable and safe enough to support continued language/runtime development.  
**Read first:** R12, R2, R3, and the existing `src/tooling/` implementations.

### TODO

- [ ] `tooling_nexus_probes.*`, `tooling_nexus_bus.*`, `tooling_nexus_main.*`, `tooling_nexus_canvas.*`, and `tooling_nexus_widgets.*`: implement typed probes, filtering/sampling, aggregation/compression/replay, multiple consumers and priorities, TUI layouts/widgets, input, views, screenshots, recording, and remote/dashboard boundaries.
- [ ] `tooling_oracle_lexer.*`, `tooling_oracle_gen.*`, and `tooling_oracle_runner.*`: parse executable documentation, run examples in isolation, report failures, measure documentation coverage, lint/version/search docs, and prevent documentation drift.
- [ ] `tooling_hash_scanner.*`, `tooling_registry.*`, `tooling_history.*`, and `tooling_ledger.*`: implement incremental/parallel/AST-aware hashing, symbol references/call graphs/search/versioning/backup, compressed/delta history, and categorized searchable fault records.
- [ ] `tooling_healer.*`, `tooling_diff.*`, and `tooling_report.*`: implement validated cure previews, rollback, cure tests, side-by-side/word/semantic/three-way diffs, metrics/trends/exports/templates/scheduling, and no silent code mutation.
- [ ] `tooling_simulator.*` and `tooling_mppa.*`: implement memory tracking, symbolic/concolic/fuzz/property/mutation testing hooks, structural/data-flow/integrity passes, caching, prioritization, scheduling, and parallel analysis with deterministic output.
- [ ] `tooling_ui.*` and `tooling_main.c`: provide a human-in-the-loop interface for ambiguity, accessible output, automation/recording/replay, and performance monitoring.
- [ ] Add security/privacy controls for telemetry, audit all auto-repair transformations, and require an explicit approval boundary before a cure modifies source.

**Exit gate:** Tooling can observe, analyze, report, and propose repairs without hiding failures or mutating code unexpectedly. Every automated transformation is reproducible, reviewable, and rollback-capable.

---

## Phase 9 — Root-Level Integration and Product Runtime

**Timing:** Weeks 47–50  
**Goal:** Fuse the subsystems into a deterministic executable, REPL, compiler pipeline, test runner, environment resolver, persistence layer, effect system, native bridge, telemetry layer, and kernel boundary.  
**Read first:** R3, R4, R0, and all lower-layer exit gates.

### 9A. Initialization and shutdown order

Implement and test the integration order from R3: hardware capability probe; diagnostic ledger; symbol registry; version/history; Oracle; VM/reactive setup; optional Nexus; project scan; MPPA analysis; Oracle execution; fault check; VM initialization; database opening; native registration; library initialization; execution/testing; and graceful shutdown.[^3]

- [ ] `src/main.c`: make initialization stages explicit, failure-aware, observable, and reversible; add plugin, hot-reload, checkpoint/restore, crash recovery, resource limits, and remote-debug hooks only after the core path is deterministic.
- [ ] `src/walia_repl.c`: add multiline input, history, completion, syntax highlighting, indentation, bracketed paste, undo/redo, session and remote boundaries.
- [ ] `src/walia_env.h` and `walia_env.c`: implement module caching, package/dependency resolution, remote-module policy, version locking, registries, signing, lazy loading, hot replacement, and bundling.

### 9B. Compilation, execution, and runtime services

- [ ] `src/analysis.*`: add type/flow/dead-code/unused-variable/constant/purity/effect/escape/alias/taint analyses and structured diagnostics.
- [ ] `src/compiler_pipe.*`: add SIMD/loop/cache/speculative/adaptive/profile-guided pipeline work with backpressure and distributed boundaries.
- [ ] `src/test_runner.*`: add filtering, parallelism, fixtures, assertions, property/snapshot/mutation tests, coverage, caching, and benchmark mode.
- [ ] `src/effect.*`: complete handlers, delimited/multi-shot continuations, shift/reset, continuation marks/barriers/prompts/composition, and serialization only where semantics are specified.
- [ ] `src/persistence.*`: add incremental checkpoints, copy-on-write snapshots, compression/encryption, multi-versioning, WAL, point-in-time recovery, backup, and replication.
- [ ] `src/native.*`: add scoped native modules/classes/properties/operators/iterators/async/generators/decorators/proxies with capability and lifetime rules.
- [ ] `src/telemetry.*`: add histograms, summaries, labels, aggregation, OpenTelemetry/export, distributed tracing, log correlation, federation, and alerting.
- [ ] `src/kernel.c`: define the host-to-WaliOS boundary and keep host builds safe when bare-metal services are unavailable.

### Phase 9 exit gate G6

- [ ] A normal run, REPL session, test run, compilation failure, runtime failure, persistence failure, and shutdown path are deterministic and observable.
- [ ] Error propagation no longer depends on a hidden global flag or a lost subsystem context.
- [ ] Subsystem ownership and allocation domains are explicit: transient C allocations, VM heap/persistent objects, and database mapped storage follow the R3 boundaries.
- [ ] Integration tests exercise the full source-to-execution path and all optional subsystems can be disabled safely.

---

## Phase 10 — WaliOS Build, Boot, and Kernel

**Timing:** Weeks 51–57  
**Goal:** Move from a prototype boot/build path to a verifiable 64-bit-capable kernel substrate without making the host language runtime depend on unverified hardware behavior.  
**Read first:** R13, R14, boot/linker/Makefile sources, and system-syntax references in R17.

### 10A. Build and boot substrate

- [ ] `waliaos/boot.S`: correct architecture claims, implement and test the 64-bit/long-mode transition plan, CPUID/capability checks, page-table setup, guarded stack, multiboot parsing, and exception failure reporting.
- [ ] `waliaos/Makefile` and build scripts: add dependency tracking, build variants, safer flags, parallel builds, cross-compilation configuration, static analysis, test integration, QEMU options, and documentation generation.
- [ ] `waliaos/linker.ld`: implement higher-half or approved memory layout, section permissions, guard pages, symbols, alignment/size assertions, debug data, per-CPU data, and module boundaries.

### 10B. Kernel hardware substrate

- [ ] `waliaos/core/kernel_init.c`: validate boot data, detect RAM dynamically, report failures, validate hardware, optimize startup, and harden boot security.
- [ ] `sys_gdt.c`, `sys_idt.c`, `sys_exception.c`: add long-mode/TSS/per-CPU GDT, complete IRQ/APIC/system-call gates, register dumps, error decoding, stack traces, recovery, and crash reports.
- [ ] `sys_pager_phys.c`: replace linear dedup search, add free/buddy lists, dynamic memory sizing, OOM handling, NUMA hints, and monitoring.
- [ ] `sys_topology.c`, `sys_cpu_features.c`, `sys_governor.c`, and `sys_watchdog.c`: validate ACPI checksums/tables, discover cores/cache topology, implement per-core thermal/power policy, calibrate watchdog timing, add NMI/recovery/rebalancing, and retain safe fallback mode.
- [ ] `sys_block_fabric.c`, `sys_final_fusion.c`, and the Walia kernel files: complete I/O scheduling, DMA boundaries, telemetry, input/output, service management, shell launch, memory sentry, security, and shutdown/error recovery.
- [ ] `kernel_main.wal`, `scheduler.wal`, `memory_sentry.wal`, `io_scheduler.wal`, `filesystem_sentry.wal`, `fabric_orchestrator.wal`, `nexus_shell.wal`, `scheduler_thermal.wal`, and `security_defender.wal`: replace hardcoded/stubbed behavior with explicit interfaces and testable state machines.

### Phase 10 exit gate

- [ ] The kernel builds reproducibly in every supported configuration and boots in the documented emulator profile.
- [ ] Boot failures produce diagnostics instead of silent infinite loops.
- [ ] Interrupt, memory, scheduling, I/O, filesystem, watchdog, thermal, and security boundaries have isolated tests or emulator assertions.
- [ ] Host-side integration remains buildable even when WaliOS-only features are excluded.

---

## Phase 11 — WaliOS Drivers

**Timing:** Weeks 58–61  
**Goal:** Replace driver stubs with capability-detected, interrupt/DMA-aware, error-recoverable implementations.  
**Read first:** R15 and R14’s hardware interfaces.

### TODO

- [ ] `apic_timer.c`: detect/calibrate LAPIC frequency, implement preemption/yield, multi-core routing, power-aware timing, and error handling.
- [ ] `driver_serial_metal.c`, `driver_vga_metal.c`, and `sys_keyboard_metal.c`: add interrupt-driven I/O, input/error/flow-control support, cursor and display modes, buffering, extended scancodes, modifiers, LEDs, repeat, and console integration.
- [ ] `sys_ahci_metal.c`: implement PRDT/FIS/DMA, interrupt completion, error paths, real read/write semantics, and performance-safe batching.
- [ ] `sys_nvme_metal.c`: implement controller configuration, admin/I/O queue creation, doorbells, completions, zero-copy boundaries, error recovery, and capability checks.
- [ ] `sys_pci_metal.c`: decode all BARs, enumerate multifunction/PCIe extended configuration, configure interrupts, enable devices, and implement power management.
- [ ] `sys_msr_guard.c` and `sys_thermal_metal.c`: add read support, complete allow/block lists, capability detection, production mode, per-core thermal control, interrupts, dynamic thresholds, graceful shutdown, and monitoring.
- [ ] Add driver mock tests, emulator tests, interrupt/DMA fault injection, and a hardware compatibility matrix.

**Exit gate:** Drivers never report successful writes without completion evidence, never access unsupported hardware without a capability check, and recover or fail safely on timeout, malformed device state, and DMA errors.

---

## Phase 12 — WaliOS System Libraries

**Timing:** Weeks 62–64  
**Goal:** Complete low-level IPC, compression, deduplication, fabric, storage, security, utility, and shell libraries that the kernel and runtime consume.  
**Read first:** R16, R14, R15, and the storage portions of R7.

### TODO

- [ ] `ipc_ring.c`: detect the correct core, add overflow handling, memory ordering, priorities, monitoring, and debug instrumentation.
- [ ] `sys_compress_simd.c`: implement decompression, metadata, non-zero compression strategies, variable/adaptive blocks, architecture fallbacks, and round-trip tests.
- [ ] `sys_dedup_fs.c` and `sys_dedup_table.c`: complete initialization/deletion, collision verification, resizing, deletion/tombstones, concurrency, statistics, and error handling.
- [ ] `sys_fabric_interconnect.c`: replace hardcoded mailbox/core assumptions, integrate IPI, allocate safely, handle overflow, support priorities, and add load/telemetry tests.
- [ ] `sys_lib_metal.c`: implement all stubs, bounds checks, thread-safe variants, safe strings, error handling, and performance tests.
- [ ] `sys_pmmo_engine.c`: detect disks, allocate LBAs, replace linear deduplication, perform real I/O through the driver contract, persist metadata, and recover from partial failure.
- [ ] `sys_security_gate.c`: fix hash comparison/copy semantics, replace core-halting failure behavior with policy-driven isolation, add signature verification, audit logging, and recovery tests.
- [ ] `sys_shell_core.c`: complete editing/backspace/cursor/history/completion, remove busy-wait assumptions where possible, and add terminal behavior tests.

**Exit gate:** System libraries are usable by kernel/runtime integration, round-trip and failure tests pass, and all critical stubs identified by R16 have either been implemented or explicitly rejected with a design record.

---

## Phase 13 — WaliOS Architecture and Portability

**Timing:** Week 65  
**Goal:** Make architecture-specific behavior explicit and prepare supported alternatives without weakening the reference x86_64 path.  
**Read first:** R0, R13–R16, `waliaos/arch/`, `waliaos/include/`, and R17 system syntax documentation.

### TODO

- [ ] Inventory `waliaos/arch/x86_64/` and classify architecture-specific assumptions across boot, ABI, interrupts, paging, drivers, atomics, SIMD, and assembly.
- [ ] Define a portability boundary for x86_64, ARM64, RISC-V, and WebAssembly where the roadmap claims support; do not claim a target until a minimal boot/build/test profile exists.
- [ ] Separate portable interfaces from architecture implementations for atomics, cache operations, page tables, timers, interrupt controllers, calling conventions, and SIMD.
- [ ] Add compile-time feature probes, runtime capability checks, and fallback implementations.
- [ ] Create architecture-specific CI/emulator targets and document unsupported features.

**Exit gate:** The reference architecture is reproducible, portability claims are evidence-backed, and unsupported paths fail at configuration time rather than at runtime.

---

## Phase 14 — Synthesis, Hardening, Documentation, and Release

**Timing:** Weeks 66–70  
**Goal:** Convert the implemented system into a maintainable release with aligned documentation, performance evidence, security review, examples, and a repeatable engineering process.  
**Read first:** R0, R3, R5–R17, `Documentation/`, README, and all phase evidence.

### 14A. Documentation and architecture

- [ ] Review all course and architecture material in `Documentation/` against the actual grammar, VM, database, systems, web, and WaliOS behavior.
- [ ] Create or update architecture diagrams for source-to-execution flow, memory domains, initialization order, database persistence, VM/JIT, tooling, and kernel/driver boundaries.
- [ ] Add code comments for invariants, algorithms, error ownership, security assumptions, and performance trade-offs.
- [ ] Complete contributor, build, test, debugging, benchmarking, language-reference, standard-library, and best-practices documentation.
- [ ] Update README claims so status, supported platforms, feature maturity, and commands match tested behavior.
- [ ] Resolve the empty `src/ROOT_INTEGRATION_ROADMAP.md` by either replacing it with a concise redirect to this master TODO plus R3 or maintaining it as a generated index; do not leave two competing root plans.

### 14B. Quality and security

- [ ] Run unit, integration, property, fuzz, mutation, snapshot, stress, sanitizer, emulator, and recovery suites appropriate to each subsystem.
- [ ] Review all unsafe operations: raw pointers, executable memory, syscalls, inline assembly, DMA, MMIO, persistence recovery, dynamic libraries, code signing, and auto-repair.
- [ ] Verify no known critical bug remains in the release scope and document accepted limitations.
- [ ] Audit telemetry for privacy, retention, authentication, encryption, and opt-out behavior.
- [ ] Verify release builds do not depend on debug-only tracing or unavailable hardware features.

### 14C. Performance and compatibility evidence

- [ ] Publish a reproducible benchmark report with host hardware, compiler, flags, dataset, warm-up, variance, and baseline comparison.
- [ ] Measure execution speed, memory use, compilation speed, GC pauses, database throughput/latency, vector recall/latency, event-loop latency, rendering frame time, boot time, and driver I/O behavior where applicable.
- [ ] Compare scalar/SIMD/JIT/GPU/parallel results and document when an optimized path is slower or unavailable.
- [ ] Verify supported architecture and operating-system profiles and remove unsupported claims from public documentation.

### Phase 14 exit gate G8

- [ ] All phase gates G0–G7 have evidence links or explicit accepted exceptions.
- [ ] Documentation examples execute successfully through the Oracle or equivalent test path.
- [ ] Release artifacts, source, generated documentation, benchmarks, and version metadata are reproducible.
- [ ] The team has a maintenance plan for roadmap items that remain experimental, deferred, or platform-specific.

---

## 6. Cross-Cutting Workstreams

These workstreams run through every phase and should be attached to the phase pull request rather than deferred until the end.

| Workstream | Required recurring tasks | Completion evidence |
|---|---|---|
| Correctness | Add regression tests for every bug fix; maintain differential tests when optimized paths exist; inject failures into cleanup and recovery paths. | Test command and result in the PR and this TODO. |
| Performance | Establish a baseline before optimization; measure hot paths, memory, cache, latency, and variance; reject claims without a reproducible fixture. | Benchmark artifact with environment and before/after numbers. |
| Memory and ownership | Keep transient C, VM/persistent, database-mapped, executable, DMA, and kernel memory domains explicit. | Ownership documentation, sanitizers, leak/failure tests. |
| Error handling | Carry file/line/column/phase/context; preserve causal errors across subsystem boundaries; avoid hidden global state and silent halts. | Structured error tests and representative failure traces. |
| Security | Capability-gate dangerous operations; preserve W^X; validate signatures/integrity; audit auto-repair, telemetry, FFI, syscalls, and persistence. | Security review notes and negative tests. |
| Observability | Emit structured logs, counters, histograms, traces, and phase diagnostics with debug/release policies. | Telemetry schema and sample report. |
| Documentation | Keep language, VM, database, systems, WaliOS, and user docs aligned with code; run executable examples. | Oracle/doc test output and updated references. |
| Portability | Detect CPU/OS/backend capabilities; keep scalar/reference fallbacks; document support by matrix. | Build/emulator matrix and configuration tests. |

---

## 7. First Implementation Proposal

The recommended first implementation is **Phase 0 — Baseline, Safety Nets, and Inventory**, followed immediately by a focused core slice. This is deliberately smaller than starting with generational GC, JIT, database sharding, or kernel migration. The repository’s own top-level roadmap says to begin with the core memory/value system, and the core task plan identifies benchmarks and telemetry as the remaining baseline work before the optimization phases.[^1] [^2]

### Proposed first PR: `core-baseline-and-safety-nets`

- [ ] Run and record `make clean && make`, `make test`, and `make debug`.
- [ ] Add a minimal benchmark target or script that measures one interpreter workload, one compile workload, and one GC/allocation workload.
- [ ] Add debug-only counters for phase timing, allocations, GC cycles, and test outcomes without changing release behavior.
- [ ] Replace or isolate the most dangerous global error-state assumptions in `src/core/common.h` and the directly dependent code, but do not begin a broad compiler refactor in this PR.
- [ ] Add regression coverage for the current baseline failures before fixing them.
- [ ] Update this document from `[ ]` to `[~]` or `[x]` with exact evidence commands and commit/PR references.

### Proposed second PR: `core-value-memory-contract`

After the baseline PR is green, implement the smallest end-to-end foundation slice across `src/core/common.h`, `value.*`, `memory.*`, `object.*`, `gc.*`, and `table.*`: document invariants, add pressure-safe failures, add value/object diagnostics, instrument the current collector, and add tests for roots, object reachability, table resizing, and heap exhaustion. Only after that slice passes should the project begin generational/incremental GC or bytecode/VM optimization.

> **Proposal:** Approve Phase 0 and the `core-baseline-and-safety-nets` first PR as the implementation starting point. The implementation should begin only after the baseline results are recorded, because those results define whether later roadmap claims represent real improvements.

---

## 8. Progress Log

Record one line per completed slice. Keep the newest entry at the top and link to the relevant commit or pull request.

| Date | Phase / work item | Evidence | Commit / PR | Notes |
|---|---|---|---|---|
| 2026-08-12 | Master TODO created; roadmap inventory consolidated. | Repository inspection; no source implementation started. | Pending | Empty root integration roadmap remains a known placeholder. |

---

## 9. References

[^1]: [`src/tooling/TASK.md`](src/tooling/TASK.md) — existing core modernization plan, working agreement, phase framing, and definition of done.
[^2]: [`REVOLUTIONIZE_ROADMAP.md`](REVOLUTIONIZE_ROADMAP.md) — repository-wide dependency order and timeline summary.
[^3]: [`src/ROOT_INTEGRATION_DEEP_ANALYSIS.md`](src/ROOT_INTEGRATION_DEEP_ANALYSIS.md) — root initialization order, error propagation, allocation domains, communication, persistence, and testing patterns.

Additional roadmap references are maintained in the [Roadmap Source Register](#3-roadmap-source-register) so each phase has a direct local link to its detailed implementation source.
