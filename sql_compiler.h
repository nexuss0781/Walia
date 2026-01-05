#ifndef WALIA_SQL_COMPILER_H
#define WALIA_SQL_COMPILER_H

#include "sql_parser.h"
#include "chunk.h"
#include "db_schema.h"

// ==========================================
// THE RELATIONAL JIT API
// ==========================================

/**
 * @brief Compiles a SQL Query AST into a Walia Bytecode Chunk.
 * 
 * The resulting bytecode is a high-speed execution plan that:
 * 1. Opens the B+ Tree for the source table.
 * 2. Iterates through leaf nodes (Sequential Scan) or seeks (Index Scan).
 * 3. Evaluates the WHERE expression for each row.
 * 4. Projects the requested columns into a Walia List.
 */
bool db_sql_compile(SqlQuery* query, WldTableDescriptor* schema, Chunk* outChunk);

/**
 * @brief Utility to generate a unique "Plan Cache Key" based on the SQL logic.
 * Ensures identical queries don't need to be re-compiled.
 */
void db_sql_generate_plan_hash(SqlQuery* query, uint8_t* outHash);

#endif // WALIA_SQL_COMPILER_H
