#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sql/sql_parser.h"
#include "../core/scanner.h" // Utilize Walia's zero-copy scanner

// ==========================================
// INTERNAL PARSER STATE
// ==========================================

typedef struct {
    Token current;
    Token previous;
    bool hadError;
} SqlParser;

static SqlParser parser;

// Navigation helpers
static void advance() {
    parser.previous = parser.current;
    parser.current = scanToken();
}

static bool check(TokenType type) {
    return parser.current.type == type;
}

static bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

// ==========================================
// SQL GRAMMAR LOGIC (Recursive Descent)
// ==========================================

static SqlQuery* selectStatement() {
    SqlQuery* query = calloc(1, sizeof(SqlQuery));
    query->type = SQL_SELECT;

    // 1. Parse Projections (SELECT a, b, c)
    query->projections = malloc(sizeof(SqlProjection) * 32);
    query->projectionCount = 0;

    do {
        if (match(TOKEN_IDENTIFIER)) {
            SqlProjection* p = &query->projections[query->projectionCount++];
            strncpy(p->column, parser.previous.start, parser.previous.length);
            p->column[parser.previous.length] = '\0';
        }
    } while (match(TOKEN_COMMA));

    // 2. Parse Source (FROM table)
    if (match(TOKEN_FOR)) { // Using 'for' as a proxy for 'FROM' for now
        advance(); // table name
        strncpy(query->sourceTable, parser.previous.start, parser.previous.length);
        query->sourceTable[parser.previous.length] = '\0';
    }

    // 3. Parse Joins (JOIN table ON ...)
    // (Logic for identifying JOIN and ON keywords)

    // 4. Parse Filter (WHERE ...)
    if (match(TOKEN_IF)) { // Using 'if' as proxy for 'WHERE'
        // We reuse the existing Walia Language expression parser!
        // This is the "Dual-Core" integration: SQL uses Walia's logic for math.
        // query->whereClause = parseExpression(); 
    }

    return query;
}

// ==========================================
// PUBLIC API
// ==========================================

SqlQuery* db_sql_parse(const char* sql) {
    initScanner(sql);
    advance();

    if (match(TOKEN_IDENTIFIER)) {
        // Simple keyword check for SELECT
        if (memcmp(parser.previous.start, "SELECT", 6) == 0) {
            return selectStatement();
        }
    }

    return NULL;
}

void db_sql_free(SqlQuery* query) {
    if (!query) return;
    if (query->projections) free(query->projections);
    if (query->joins) free(query->joins);
    // freeExpr(query->whereClause);
    free(query);
}
