#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h" 
#include "ast.h"
#include "db_common.h"
#include "object.h"

// ==========================================
// SENTRY METADATA
// ==========================================

typedef enum {
    SENTRY_NONE,
    SENTRY_SQL,    
    SENTRY_NOSQL,  
    SENTRY_VECTOR  
} SentryType;

typedef struct {
    SentryType type;
    char       tableName[WLD_MAX_NAME_LEN];
    bool       isTemporal;
} SentryMetadata;

// ==========================================
// DECORATOR LOGIC
// ==========================================

/**
 * @brief Parses Walia @decorators.
 * Translates syntax like @sql into persistent schema metadata.
 */
static SentryMetadata parseDecorator() {
    SentryMetadata meta;
    meta.type = SENTRY_NONE;
    meta.isTemporal = false;
    memset(meta.tableName, 0, WLD_MAX_NAME_LEN);

    advance(); // Consume '@' using parser.h primitive

    if (match(TOKEN_IDENTIFIER)) {
        Token dName = parser.previous;

        if (memcmp(dName.start, "sql", 3) == 0) meta.type = SENTRY_SQL;
        else if (memcmp(dName.start, "nosql", 5) == 0) meta.type = SENTRY_NOSQL;
        else if (memcmp(dName.start, "vector", 6) == 0) meta.type = SENTRY_VECTOR;
        else if (memcmp(dName.start, "temporal", 8) == 0) meta.isTemporal = true;

        // Handle arguments: @sql("users")
        if (match(TOKEN_LEFT_PAREN)) {
            if (match(TOKEN_STRING)) {
                int len = parser.previous.length - 2;
                if (len >= WLD_MAX_NAME_LEN) len = WLD_MAX_NAME_LEN - 1;
                memcpy(meta.tableName, parser.previous.start + 1, len);
            }
            consume(TOKEN_RIGHT_PAREN, "Expect ')' after decorator.");
        }
    }

    return meta;
}

// ==========================================
// SOVEREIGN ENTRY POINT
// ==========================================

/**
 * @brief The Master Statement Interceptor.
 * This is the function called by the compiler's main loop.
 */
Stmt* sentryDeclaration() {
    SentryMetadata meta = { SENTRY_NONE, "", false };

    // Consume all leading decorators
    while (check(TOKEN_AT)) {
        SentryMetadata d = parseDecorator();
        if (d.type != SENTRY_NONE) meta.type = d.type;
        if (d.isTemporal) meta.isTemporal = true;
        if (d.tableName[0] != '\0') strcpy(meta.tableName, d.tableName);
    }

    // Hand over to standard language parsing logic in parser.c
    // In a fully integrated Sentry, 'meta' would be attached to the Stmt.
    return declaration(); 
}
