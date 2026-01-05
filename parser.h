#ifndef WALIA_PARSER_H
#define WALIA_PARSER_H

#include "ast.h"
#include "common.h"
#include "scanner.h"

// ==========================================
// PARSER STATE EXPOSURE
// ==========================================

typedef struct {
  Token current;
  Token previous;
  bool hadError; // Internal tracking for recovery
  bool panicMode;
} Parser;

extern Parser parser; 

// ==========================================
// NAVIGATION PRIMITIVES
// ==========================================

void advance();
void consume(TokenType type, const char* message);
bool check(TokenType type);
bool match(TokenType type);
bool isAtEnd();

// ==========================================
// PARSER API
// ==========================================

void initParser();
Stmt** parse(const char* source, int* count);

/**
 * @brief Internal declaration parser.
 * Exposed for Sentry ORM reflective analysis.
 */
Stmt* declaration();

/**
 * @brief Expression parser.
 * Exposed for external modules (sys_parser_ext).
 */
Expr* expression();

#endif // WALIA_PARSER_H
