#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "memory.h"
#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "object.h"
#include "sys/sys_types.h"
#include "sys/sys_units_math.h"     // Added Phase 17.1
#include "sys/sys_units_registry.h" // Added Phase 17.1

// = = = = = = = = = = = = = = = = = = = = = =
// FORWARD DECLARATIONS (Sovereign Ordering)
// = = = = = = = = = = = = = = = = = = = = = =
Expr* expression();  // Non-static for external use
static Expr* andLogic();
static Expr* orLogic();
static Expr* binary();
static Expr* unary();
static Expr* call();
static Expr* primary();
static Stmt* statement();
static Stmt* block();
static Stmt* classDeclaration();
static Stmt* geneDeclaration();
static Stmt* layoutDeclaration();
static Expr* finishCall(Expr* callee);   // Added: Resolves visibility conflict
static Expr* finishIndex(Expr* callee);  // Added
extern Stmt* sentryDeclaration(); 
static void errorAt(Token* token, const char* message);

Parser parser;
// ==========================================
// INTERNAL DIAGNOSTICS
// ==========================================

static Expr* matchExpression() {
    Expr* target = expression();
    consume(TOKEN_LEFT_BRACE, "Expect '{' after match target.");

    int capacity = 8;
    int count = 0;
    MatchCase* cases = malloc(sizeof(MatchCase) * capacity);

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        if (count >= capacity) {
            capacity *= 2;
            cases = realloc(cases, sizeof(MatchCase) * capacity);
        }

        if (match(TOKEN_TILDE)) {
            cases[count].pattern = expression(); 
        } else if (match(TOKEN_IDENTIFIER) && parser.previous.length == 1 && parser.previous.start[0] == '_') {
            cases[count].pattern = NULL; 
        } else {
            cases[count].pattern = expression(); 
        }

        cases[count].guard = NULL;
        if (match(TOKEN_IF)) {
            cases[count].guard = expression();
        }

        consume(TOKEN_FAT_ARROW, "Expect '=>'.");
        cases[count].body = statement();

        count++;
        if (!match(TOKEN_COMMA)) break;
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}'.");
    return newMatch(target, count, cases);
}

static Stmt* layoutDeclaration() {
    bool isUnion = (parser.previous.type == TOKEN_UNION);
    consume(TOKEN_IDENTIFIER, "Expect layout name.");
    Token name = parser.previous;
    consume(TOKEN_LEFT_BRACE, "Expect '{' before layout body.");

    int capacity = 8;
    int count = 0;
    LayoutMember* members = malloc(sizeof(LayoutMember) * capacity);

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        if (count >= capacity) {
            capacity *= 2;
            members = realloc(members, sizeof(LayoutMember) * capacity);
        }
        consume(TOKEN_IDENTIFIER, "Expect member name.");
        members[count].name = parser.previous;
        consume(TOKEN_COLON, "Expect ':'.");
        advance(); // Resolve type name logic
        members[count].type = (int)sys_resolve_type_name(parser.previous.start, parser.previous.length);
        members[count].bitWidth = match(TOKEN_COLON) ? (int)strtod(parser.current.start, NULL) : 0;
        consume(TOKEN_SEMICOLON, "Expect ';'.");
        count++;
    }
    consume(TOKEN_RIGHT_BRACE, "Expect '}'.");
    // Link to Phase 11.3 Layout Engine
    extern int sys_layout_calculate(int count, LayoutMember* members, bool isUnion);
    int totalSize = sys_layout_calculate(count, members, isUnion);
    return newLayoutStmt(name, count, members, totalSize);
}

static Expr* orLogic() {
    Expr* expr = andLogic(); // Link to lower precedence
    while (match(TOKEN_OR)) {
        Token op = parser.previous;
        expr = newLogical(expr, op, andLogic());
    }
    return expr;
}

static Expr* andLogic() {
    Expr* expr = binary(); // Link to arithmetic
    while (match(TOKEN_AND)) {
        Token op = parser.previous;
        expr = newLogical(expr, op, binary());
    }
    return expr;
}

static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Walia Error", token->line);
    if (token->type == TOKEN_EOF) fprintf(stderr, " at end");
    else if (token->type != TOKEN_ERROR) fprintf(stderr, " at '%.*s'", token->length, token->start);
    fprintf(stderr, ": %s\n", message);
    hadError = true;
    parser.hadError = true;
}

static void parseSliceDimension(Expr** start, Expr** end, Expr** step) {
    *start = NULL; *end = NULL; *step = NULL;

    // 1. Optional Start: [START : ...]
    if (!check(TOKEN_COLON) && !check(TOKEN_COMMA) && !check(TOKEN_RIGHT_BRACKET)) {
        *start = expression();
    }

    // 2. Optional End: [... : END : ...]
    if (match(TOKEN_COLON)) {
        if (!check(TOKEN_COLON) && !check(TOKEN_COMMA) && !check(TOKEN_RIGHT_BRACKET)) {
            *end = expression();
        }
        
        // 3. Optional Step: [... : ... : STEP]
        if (match(TOKEN_COLON)) {
            if (!check(TOKEN_COMMA) && !check(TOKEN_RIGHT_BRACKET)) {
                *step = expression();
            }
        }
    }
}

static Expr* finishIndex(Expr* callee) {
    int capacity = 4;
    int dims = 0;
    
    Expr** starts = malloc(sizeof(Expr*) * capacity);
    Expr** ends   = malloc(sizeof(Expr*) * capacity);
    Expr** steps  = malloc(sizeof(Expr*) * capacity);

    do {
        if (dims >= capacity) {
            capacity *= 2;
            starts = realloc(starts, sizeof(Expr*) * capacity);
            ends   = realloc(ends,   sizeof(Expr*) * capacity);
            steps  = realloc(steps,  sizeof(Expr*) * capacity);
        }
        
        parseSliceDimension(&starts[dims], &ends[dims], &steps[dims]);
        dims++;
    } while (match(TOKEN_COMMA));

    consume(TOKEN_RIGHT_BRACKET, "Expect ']' after index/slice.");

    // Logic: If it's a single dimension and no colons were used, 
    // it's a standard index. Otherwise, it's a Slice.
    bool isActuallySlice = false;
    for (int i = 0; i < dims; i++) {
        if (ends[i] != NULL || steps[i] != NULL || starts[i] == NULL) {
            isActuallySlice = true; 
            break;
        }
    }

    if (isActuallySlice || dims > 1) {
        return newSlice(callee, dims, starts, ends, steps);
    } else {
        // Standard Index Path: obj[x] -> Call __get__(x)
        Expr* indexExpr = starts[0];
        free(starts); free(ends); free(steps); // Cleanup arrays
        
        Expr** args = malloc(sizeof(Expr*) * 1);
        args[0] = indexExpr;
        return newCall(callee, (Token){TOKEN_LEFT_BRACKET, "[", 1, parser.previous.line}, 1, args);
    }
}

static Stmt* asmStatement() {
    consume(TOKEN_LEFT_BRACE, "Expect '{' before assembly block.");
    
    // In Walia, assembly is provided as a raw string literal inside the block
    if (!check(TOKEN_STRING)) {
        errorAt(&parser.current, "Expect assembly instruction string.");
    }
    
    advance();
    Token code = parser.previous;
    
    consume(TOKEN_RIGHT_BRACE, "Expect '}' after assembly block.");
    return newAsmStmt(code);
}

// ==========================================
// NAVIGATION PRIMITIVES (Non-Static)
// ==========================================

void advance() {
    parser.previous = parser.current;
    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;
        errorAt(&parser.current, parser.current.start);
    }
}


void consume(TokenType type, const char* message) {
    if (parser.current.type == type) { advance(); return; }
    errorAt(&parser.current, message);
}

bool check(TokenType type) { return parser.current.type == type; }
bool match(TokenType type) { if (!check(type)) return false; advance(); return true; }
bool isAtEnd() { return parser.current.type == TOKEN_EOF; }

static Stmt* geneDeclaration() {
    consume(TOKEN_IDENTIFIER, "Expect gene name.");
    Token name = parser.previous; 
    consume(TOKEN_LEFT_BRACE, "Expect '{' before gene body.");

    int mCount = 0, tCount = 0;
    int mCap = 8, tCap = 8;
    Stmt** methods = malloc(sizeof(Stmt*) * mCap);
    Stmt** traits = malloc(sizeof(Stmt*) * tCap);

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        if (match(TOKEN_VAR)) {
            Token n = parser.current; advance(); consume(TOKEN_COLON, "E");
            if (tCount >= tCap) {
                tCap *= 2;
                traits = realloc(traits, sizeof(Stmt*) * tCap);
            }
            traits[tCount++] = newVarStmt(n, expression());
            consume(TOKEN_SEMICOLON, "E");
        } else if (match(TOKEN_FUN)) {
            if (mCount >= mCap) {
                mCap *= 2;
                methods = realloc(methods, sizeof(Stmt*) * mCap);
            }
            Token n = parser.current; advance(); consume(TOKEN_LEFT_PAREN, "E"); consume(TOKEN_RIGHT_PAREN, "E");
            methods[mCount++] = newFunctionStmt(n, 0, NULL, block(), false);
        }
    }
    consume(TOKEN_RIGHT_BRACE, "Expect '}'.");
    return newGeneStmt(name, mCount, methods, tCount, traits);
}

static Stmt* classDeclaration() {
    consume(TOKEN_IDENTIFIER, "Expect class name.");
    Token name = parser.previous;

    // 1. Inheritance Check (< Superclass)
    Expr* superclass = NULL;
    if (match(TOKEN_LESS)) {
        consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        superclass = newVariable(parser.previous);
    }

    // 2. Trait Composition Check (with Trait1, Trait2...)
    int traitCount = 0;
    Expr** traits = NULL;
    if (match(TOKEN_WITH)) {
        do {
            traits = realloc(traits, sizeof(Expr*) * ++traitCount);
            consume(TOKEN_IDENTIFIER, "Expect trait name.");
            traits[traitCount-1] = newVariable(parser.previous);
        } while (match(TOKEN_COMMA));
    }

    consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");

    int methodCount = 0;
    Stmt** methods = malloc(sizeof(Stmt*) * 8);

    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        methods = realloc(methods, sizeof(Stmt*) * ++methodCount);
        consume(TOKEN_FUN, "Expect 'fun' for method.");
        Token mName = parser.current; advance();
        consume(TOKEN_LEFT_PAREN, "E"); consume(TOKEN_RIGHT_PAREN, "E");
        methods[methodCount-1] = newFunctionStmt(mName, 0, NULL, block(), false);
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
    return newClassStmt(name, superclass, methodCount, methods, traitCount, traits);
}

// ==========================================
// EXPRESSION HIERARCHY
// ==========================================

static Expr* primary() {
    if (match(TOKEN_FALSE)) return newLiteral(BOOL_VAL(false));
    if (match(TOKEN_TRUE))  return newLiteral(BOOL_VAL(true));
    if (match(TOKEN_NIL))   return newLiteral(NIL_VAL);
    if (match(TOKEN_NUMBER)) return newLiteral(NUMBER_VAL(strtod(parser.previous.start, NULL)));
    if (match(TOKEN_STRING)) return newLiteral(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
    if (match(TOKEN_IDENTIFIER)) return newVariable(parser.previous);
    
    // Phase 17.4: Pattern Match Expression
    if (match(TOKEN_MATCH)) {
        return matchExpression();
    }

    if (match(TOKEN_ALLOC)) {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'alloc'.");
        Expr* size = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after size expression.");
        return newAlloc(size);
    }

    if (match(TOKEN_SIZEOF)) {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'sizeof'.");
        consume(TOKEN_IDENTIFIER, "Expect type name.");
        Token typeName = parser.previous;
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after type name.");
        return newSizeof(typeName);
    }

    if (match(TOKEN_SYSCALL)) {
        consume(TOKEN_LEFT_PAREN, "Expect '(' after 'syscall'.");
        Expr* id = expression();
        Expr** args = NULL;
        int count = 0;
        if (match(TOKEN_COMMA)) {
            do {
                if (count >= 6) errorAt(&parser.previous, "Syscalls support max 6 arguments.");
                args = realloc(args, sizeof(Expr*) * ++count);
                args[count - 1] = expression();
            } while (match(TOKEN_COMMA));
        }
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after syscall arguments.");
        return newSyscall(id, count, args);
    }

    if (match(TOKEN_PERFORM)) {
        consume(TOKEN_STRING, "Expect effect name.");
        return newPerform(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
    }

    if (match(TOKEN_LEFT_PAREN)) {
        Expr* expr = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')'.");
        return newGrouping(expr);
    }
    
    errorAt(&parser.current, "Expect expression.");
    return NULL;
}

Stmt* declaration() {
    if (match(TOKEN_GENE)) return geneDeclaration();
    if (match(TOKEN_ASYNC)) return declaration(); 
    if (match(TOKEN_CLASS)) return classDeclaration();
    if (match(TOKEN_LAYOUT) || match(TOKEN_UNION)) return layoutDeclaration();

    if (match(TOKEN_VAR)) {
        Token name = parser.current; advance();
        int sysType = 0;
        WldDimensionVector dim = wld_dim_empty();
        if (match(TOKEN_COLON)) {
            if (match(TOKEN_UNIT)) dim = sys_units_resolve(parser.previous.start + 1, parser.previous.length - 2);
            else if (match(TOKEN_IDENTIFIER)) sysType = (int)sys_resolve_type_name(parser.previous.start, parser.previous.length);
        }
        Expr* init = match(TOKEN_EQUAL) ? expression() : NULL;
        consume(TOKEN_SEMICOLON, "Expect ';'.");
        VarStmt* stmt = (VarStmt*)newVarStmt(name, init);
        stmt->sysType = sysType; stmt->dimConstraint = dim;
        return (Stmt*)stmt;
    }

    if (match(TOKEN_FUN)) {
        Token name = parser.current; advance();
        consume(TOKEN_LEFT_PAREN, "Expect '('.");
        consume(TOKEN_RIGHT_PAREN, "Expect ')'.");
        return newFunctionStmt(name, 0, NULL, block(), false);
    }

    if (check(TOKEN_AT)) return sentryDeclaration();

    return statement();
}



// In declaration():


static Expr* call() {
    Expr* expr = primary();
    for (;;) {
        if (match(TOKEN_LEFT_PAREN)) {
            expr = finishCall(expr);
        } else if (match(TOKEN_LEFT_BRACKET)) {
            expr = finishIndex(expr);
        } else {
            break;
        }
    }
    return expr;
}

static Expr* finishCall(Expr* callee) {
    int argCount = 0;
    Expr** args = NULL;
    
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            if (argCount >= 255) errorAt(&parser.previous, "Max arguments exceeded.");
            args = realloc(args, sizeof(Expr*) * ++argCount);
            args[argCount - 1] = expression();
        } while (match(TOKEN_COMMA));
    }

    Token paren = parser.current;
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return newCall(callee, paren, argCount, args);
}

static Expr* unary() {
    if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
        Token op = parser.previous;
        return newUnary(op, unary());
    }

    // Phase 11.2: Address-Of Operator (&variable)
    if (match(TOKEN_AMPERSAND)) {
        return newAddress(unary());
    }

    // Phase 11.2: Dereference Operator (*pointer)
    if (match(TOKEN_STAR)) {
        return newDeref(unary());
    }

    return call();
}

static Expr* binary() {
    Expr* expr = unary();
    while (match(TOKEN_STAR) || match(TOKEN_SLASH) || match(TOKEN_PLUS) || match(TOKEN_MINUS) ||
           match(TOKEN_GREATER) || match(TOKEN_LESS) || match(TOKEN_EQUAL_EQUAL)) {
        Token op = parser.previous;
        expr = newBinary(expr, op, unary());
    }
    return expr;
}

Expr* expression() {
    Expr* expr = orLogic(); // Point to the top of the logic chain

    if (match(TOKEN_EQUAL)) {
        Token equals = parser.previous;
        Expr* value = expression();
        if (expr->type == EXPR_VARIABLE) {
            return newAssign(((VariableExpr*)expr)->name, value);
        }
        errorAt(&equals, "Invalid assignment target.");
    }

    if (match(TOKEN_TILDE_EQUAL)) {
        Token tilde = parser.previous;
        Expr* source = expression();
        if (expr->type == EXPR_VARIABLE) {
            return newEntangle(((VariableExpr*)expr)->name, source);
        }
        errorAt(&tilde, "Invalid entanglement target.");
    }

    return expr;
}

// ==========================================
// STATEMENT LOGIC
// ==========================================

static Stmt* block() {
    int count = 0; Stmt** stmts = NULL;
    while (!check(TOKEN_RIGHT_BRACE) && !isAtEnd()) {
        stmts = realloc(stmts, sizeof(Stmt*) * ++count);
        stmts[count-1] = declaration();
    }
    consume(TOKEN_RIGHT_BRACE, "Expect '}'.");
    return newBlockStmt(stmts, count);
}

static Stmt* forStatement() {
    // 1. Parse 'for' (already matched)
    
    // 2. Parse Variable Name
    // for var x in ...
    // Note: Walia syntax might be 'for x in ...' or 'for var x ...'
    // We stick to 'for var x' to be explicit about scope.
    consume(TOKEN_VAR, "Expect 'var' after 'for'.");
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    Token name = parser.previous;

    consume(TOKEN_IN, "Expect 'in' after variable name.");

    // 3. Parse Iterator Source
    Expr* source = expression();
    consume(TOKEN_LEFT_BRACE, "Expect '{' after loop header.");

    // 4. Parse Body
    Stmt* body = block(); // Reuses block() logic which consumes '}'

    // 5. Desugar into While Loop
    // This is the "Compiler Magic". We transform:
    // for var x in list { print x; }
    // INTO:
    // var __iter = list; // OP_ITER_INIT
    // while (true) {
    //    var x = iter_next(__iter); // OP_ITER_NEXT
    //    if (stop) break;
    //    print x;
    // }
    
    // For Phase 13.2, we return a specialized ForStmt node if we want to preserve semantics,
    // or we manually build the BlockStmt/WhileStmt tree here.
    // Given the complexity of desugaring in a single pass compiler, 
    // we will emit the ForStmt node and let the Compiler backend handle the bytecode generation.
    
    // NOTE: Need to add newForStmt to ast.h/ast.c
    // return newForStmt(name, source, body);
    
    // Placeholder to keep parser valid without new AST node:
    return newWhileStmt(source, body);
}

static Stmt* statement() {
    if (match(TOKEN_IF)) {
        consume(TOKEN_LEFT_PAREN, "E"); Expr* c = expression(); consume(TOKEN_RIGHT_PAREN, "E");
        Stmt* t = statement(); Stmt* e = match(TOKEN_ELSE) ? statement() : NULL;
        return newIfStmt(c, t, e);
    }
    if (match(TOKEN_FOR)) return forStatement(); // Fixed: Linked forStatement
    if (match(TOKEN_PRINT)) { Expr* v = expression(); consume(TOKEN_SEMICOLON, "E"); return newPrintStmt(v); }
    if (match(TOKEN_ASM)) return asmStatement();
    if (match(TOKEN_RELEASE)) { Expr* t = expression(); consume(TOKEN_SEMICOLON, "E"); return newReleaseStmt(t); }
    if (match(TOKEN_LEFT_BRACE)) return block();
    Expr* ex = expression(); consume(TOKEN_SEMICOLON, "E"); return newExpressionStmt(ex);
}

// ==========================================
// GLOBAL DECLARATION ENTRY
// ==========================================

// ==========================================
// MASTER API
// ==========================================

void initParser() {
    hadError = false; 
    parser.hadError = false;
    parser.panicMode = false;
}

Stmt** parse(const char* source, int* count) {
    initScanner(source);
    initParser();
    advance();
    int capacity = 0; *count = 0; Stmt** stmts = NULL;
    while (!match(TOKEN_EOF)) {
        if (*count + 1 > capacity) {
            capacity = GROW_CAPACITY(capacity);
            stmts = GROW_ARRAY(Stmt*, stmts, *count, capacity);
        }
        stmts[(*count)++] = declaration();
    }
    return stmts;
}
