#ifndef WALIA_DB_SQE_SQL_H
#define WALIA_DB_SQE_SQL_H

#include "db_common.h"
#include "db_schema.h"
#include "chunk.h"
#include "object.h"
#include "sql_parser.h" // Fixed: Provides SqlQuery and SqlJoin definitions

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
void db_sqe_compile_pointer_join(Chunk* chunk, SqlJoin* join, int rowPtrReg);

#endif // WALIA_DB_SQE_SQL_H
