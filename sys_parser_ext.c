#include <stdio.h>
#include <stdlib.h>

#include "sys_parser_ext.h"
#include "scanner.h"
#include "parser.h"

// Access global parser state from parser.c
extern Parser parser;

// We need to add TOKEN_COLON to token.h to support this syntax.
// Assuming it will be added in Phase 11.1 Integration.
#ifndef TOKEN_COLON
#define TOKEN_COLON TOKEN_ERROR // Placeholder until integration
#endif

// ==========================================
// TYPE PARSING LOGIC
// ==========================================

WaliaSysType sys_parse_type_annotation() {
    // Check for colon ':'
    if (match(TOKEN_COLON)) {
        // Expect a type identifier
        if (match(TOKEN_IDENTIFIER)) {
            Token typeToken = parser.previous;
            
            // Resolve string to Enum
            WaliaSysType type = sys_resolve_type_name(typeToken.start, typeToken.length);
            
            if (type == SYS_TYPE_VALUE) {
                // If it's not a primitive, it might be a Class name (future phase)
                // For now, warn if it looks like a system type but failed resolution
                // errorAtCurrent("Unknown system type.");
            }
            return type;
        } else {
            // errorAtCurrent("Expect type name after ':'.");
        }
    }
    
    // Default to Managed Mode (Dynamic Typing)
    return SYS_TYPE_VALUE; 
}

// ==========================================
// DECLARATION OVERRIDE
// ==========================================

Stmt* sys_parse_var_declaration() {
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    Token name = parser.previous;

    // 1. Capture the system type (u8, i32, etc.)
    WaliaSysType type = sys_parse_type_annotation();

    // 2. Resolve the initializer expression
    Expr* init = NULL;
    if (match(TOKEN_EQUAL)) {
        // UFO SPEED: Direct recursive descent into the expression engine
        extern Expr* expression(); 
        init = expression();
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

    // 3. Physical AST Injection
    VarStmt* stmt = (VarStmt*)newVarStmt(name, init);
    stmt->sysType = (int)type; // Physically bind the hardware type

    return (Stmt*)stmt;
}
