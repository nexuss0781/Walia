#ifndef WALIA_TOOLING_NEXUS_PROBES_H
#define WALIA_TOOLING_NEXUS_PROBES_H

#include "tooling_nexus_bus.h"

// ==========================================
// SOVEREIGN TELEMETRY MACROS
// ==========================================
/**
 * These macros are injected into the core C source (vm.c, db_pager.c, etc.)
 * Logic: If the Command Nexus is active, emit an atomic pulse.
 */

#define PULSE_EMIT(type, target, payload) \
    do { \
        if (walia_pulse_is_active()) { \
            walia_pulse_emit(type, (uint32_t)(target), (uint64_t)(payload)); \
        } \
    } while (0)

// ==========================================
// DATA PROBES (Pager & DB)
// ==========================================

// Injected in db_pager_get_page()
#define PROBE_DB_READ(pageId) \
    PULSE_EMIT(PULSE_DB_PAGE_READ, pageId, 0)

// Injected in db_get_page_write()
#define PROBE_DB_WRITE(pageId) \
    PULSE_EMIT(PULSE_DB_PAGE_WRITE, pageId, 0)

// Injected in db_transaction_commit()
#define PROBE_DB_COMMIT(txId, pageCount) \
    PULSE_EMIT(PULSE_DB_COMMIT, txId, pageCount)

// ==========================================
// NEURAL PROBES (Vector Wing)
// ==========================================

// Injected in db_hnsw_search() loop
#define PROBE_NEURAL_HOP(nodeId, level) \
    PULSE_EMIT(PULSE_NEURAL_HOP, nodeId, level)

// Injected in db_vector_math.c SIMD loops
#define PROBE_NEURAL_SIMD(dimCount) \
    PULSE_EMIT(PULSE_NEURAL_MATH, dimCount, 0)

// ==========================================
// SOVEREIGN PROBES (GC & Persistence)
// ==========================================

// Injected in markObject()
#define PROBE_GC_MARK(objAddr) \
    PULSE_EMIT(PULSE_GC_MARK, (uintptr_t)objAddr % 0xFFFFFFFF, 0)

// Injected in sweep()
#define PROBE_GC_RECLAIM(bytes) \
    PULSE_EMIT(PULSE_GC_SWEEP, 0, bytes)

// ==========================================
// INTELLIGENCE PROBES (Sentinel & JIT)
// ==========================================

// Injected in tooling_hash_scanner.c
#define PROBE_SCAN_FILE(pathHash) \
    PULSE_EMIT(PULSE_SCAN_FILE, pathHash, 0)

// Injected in db_sqe_sql_jit()
#define PROBE_JIT_GEN(opcodeCount) \
    PULSE_EMIT(PULSE_JIT_COMPILE, opcodeCount, 0)

#endif // WALIA_TOOLING_NEXUS_PROBES_H
