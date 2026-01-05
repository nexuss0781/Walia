#ifndef WALIA_SQL_PARSER_H
#define WALIA_SQL_PARSER_H

#include "db_common.h"
#include "ast.h" // Reuse Walia Expr for WHERE clauses

// ==========================================
// SQL AST STRUCTURES
// ==========================================

typedef enum {
    SQL_SELECT,
    SQL_INSERT,
    SQL_UPDATE,
    SQL_DELETE,
    SQL_CREATE_TABLE
} SqlStatementType;

typedef struct {
    char column[64];
    char alias[64];
} SqlProjection;

typedef struct {
    char table[64];
    char alias[64];
    char joinCol[64];
    char targetCol[64];
    bool isLeftJoin;
} SqlJoin;

typedef struct {
    SqlStatementType type;
    
    // SELECT specific
    SqlProjection* projections;
    int projectionCount;
    
    char sourceTable[64];
    SqlJoin* joins;
    int joinCount;
    
    Expr* whereClause; // Reuse Walia's expression logic for filters
    
    char groupBy[64];
    char orderBy[64];
    bool orderAscending;

    // INSERT/CREATE specific
    // (Metadata for schema and value lists would go here)
} SqlQuery;

// ==========================================
// SQL PARSER API
// ==========================================

/**
 * @brief Parses a SQL string into a Sovereign Query Object.
 * 
 * Supports: 
 * SELECT [cols] FROM [table] 
 * JOIN [table2] ON [col1] = [col2] 
 * WHERE [condition]
 */
SqlQuery* db_sql_parse(const char* sql);

/**
 * @brief Frees the memory associated with a SQL AST.
 */
void db_sql_free(SqlQuery* query);

#endif // WALIA_SQL_PARSER_H
