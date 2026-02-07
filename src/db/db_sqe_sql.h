#ifndef WALIA_DB_SQE_SQL_H
#define WALIA_DB_SQE_SQL_H

#include "db_common.h"
#include "db_schema.h"
#include "../core/chunk.h"
#include "../core/object.h"
#include "sql/sql_parser.h" // Fixed: Provides SqlQuery and SqlJoin definitions

// ==========================================
// RELATIONAL JIT SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a Compiled Query Plan.
 * Persistent object that caches the JIT logic for a specific SQL query.
 */
typedef struct {
    struct Obj obj;
    uint8_t    planHash[32]; 
    ObjFunction* bytecode;   
} WldQueryPlan;

// ==========================================
// SOVEREIGN JIT API
// ==========================================

WldQueryPlan* db_sqe_sql_jit(SqlQuery* ast, WldTableDescriptor* schema);
ObjFunction* db_sqe_jit_compile_filter(WldTableDescriptor* schema, int colIndex, Value threshold);
void db_sqe_execute(WldTableDescriptor* table, ObjFunction* jitFilter);
void db_sqe_compile_pointer_join(Chunk* chunk, SqlJoin* join, int rowPtrReg);

#endif // WALIA_DB_SQE_SQL_H
