#ifndef WALIA_DB_SENTRY_JIT_H
#define WALIA_DB_SENTRY_JIT_H

#include "db_common.h"
#include "db_schema.h"
#include "chunk.h"
#include "object.h"

// ==========================================
// PREDICATE JIT SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a JIT-compiled closure filter.
 * 
 * Unlike a standard Walia function, a Sentry Predicate 
 * is unrolled to process 8 rows per cycle using SIMD.
 */
typedef struct {
    struct Obj obj;
    ObjClosure* originalClosure; // The source Walia lambda
    ObjFunction* optimizedFunc;  // The SIMD-unrolled JIT version
    uint32_t    targetColOffset; // The physical byte offset in the .wld row
} WldSentryPredicate;

// ==========================================
// SENTRY JIT API
// ==========================================

/**
 * @brief Analyzes a Walia Closure and determines if it can be unrolled.
 * 
 * Criteria for UFO-grade Unrolling:
 * 1. The closure must take exactly 1 argument (the row).
 * 2. It must perform a simple comparison (>, <, ==, !=).
 * 3. It must reference a field present in the Table Schema.
 * 
 * @param closure The lambda provided by the developer.
 * @param schema The table descriptor for the target table.
 * @return WldSentryPredicate* The compiled predicate object.
 */
WldSentryPredicate* db_sentry_compile_predicate(ObjClosure* closure, WldTableDescriptor* schema);

/**
 * @brief The "Unroller" Core.
 * Translates standard VM instructions (OP_GET_PROPERTY, OP_GREATER)
 * into bulk database instructions (OP_DB_SIMD_FILTER_GT).
 */
void db_sentry_unroll_logic(Chunk* outChunk, ObjFunction* source, WldTableDescriptor* schema);

#endif // WALIA_DB_SENTRY_JIT_H
