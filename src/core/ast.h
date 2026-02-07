#ifndef WALIA_AST_H
#define WALIA_AST_H

#include "common.h"
#include "token.h"
#include "value.h"

// ==========================================
// FORWARD DECLARATIONS
// ==========================================
typedef struct Stmt Stmt;
typedef struct Expr Expr;
typedef struct { int8_t units[7]; } WldDimensionVector;

// ==========================================
// EXPRESSION NODES
// ==========================================

typedef enum {
    EXPR_BINARY, EXPR_GROUPING, EXPR_LITERAL, EXPR_UNARY,
    EXPR_VARIABLE, EXPR_ASSIGN, EXPR_LOGICAL, EXPR_CALL,
    EXPR_PERFORM, EXPR_ADDRESS, EXPR_DEREF, EXPR_SYSCALL,
    EXPR_ALLOC, EXPR_SIZEOF, EXPR_SLICE, EXPR_AWAIT,
    EXPR_ENTANGLE, EXPR_PIPE, EXPR_MATCH, EXPR_RANGE
} ExprType;

struct Expr {
    ExprType type;
    int line;
    WldDimensionVector dimensions;
};

typedef struct { Expr* pattern; Expr* guard; Stmt* body; } MatchCase;
typedef struct { Expr obj; Expr* target; MatchCase* cases; int caseCount; } MatchExpr;
typedef struct { Expr obj; Expr* min; Expr* max; } RangeExpr;
typedef struct { Expr obj; Expr* left; Token operator; Expr* right; } BinaryExpr;
typedef struct { Expr obj; Expr* expression; } GroupingExpr;
typedef struct { Expr obj; Value value; } LiteralExpr;
typedef struct { Expr obj; Token operator; Expr* right; } UnaryExpr;
typedef struct { Expr obj; Token name; } VariableExpr;
typedef struct { Expr obj; Token name; Expr* value; } AssignExpr;
typedef struct { Expr obj; Expr* left; Token operator; Expr* right; } LogicalExpr;
typedef struct { Expr obj; Expr* callee; Token paren; int argCount; Expr** arguments; } CallExpr;
typedef struct { Expr obj; Value effectName; } PerformExpr;
typedef struct { Expr obj; Expr* target; } AddressExpr;
typedef struct { Expr obj; Expr* target; } DerefExpr;
typedef struct { Expr obj; Expr* id; int argCount; Expr** arguments; } SyscallExpr;
typedef struct { Expr obj; Expr* target; int dimCount; Expr** starts; Expr** ends; Expr** steps; } SliceExpr;
typedef struct { Expr obj; Expr* size; } AllocExpr;
typedef struct { Expr obj; Token typeName; } SizeofExpr;
typedef struct { Expr obj; Expr* target; } AwaitExpr;
typedef struct { Expr obj; Token name; Expr* source; } EntangleExpr;
typedef struct { Expr obj; Expr* left;  Expr* right; } PipeExpr;

// ==========================================
// STATEMENT NODES
// ==========================================

typedef enum {
    STMT_EXPRESSION, STMT_PRINT, STMT_VAR, STMT_BLOCK,
    STMT_IF, STMT_WHILE, STMT_FUNCTION, STMT_RETURN,
    STMT_HANDLE, STMT_RESUME, STMT_TEST, STMT_LAYOUT, STMT_ASM,
    STMT_RELEASE, STMT_CLASS, STMT_GENE
} StmtType;

struct Stmt {
    StmtType type;
    int line;
};

typedef struct { Stmt obj; Expr* expression; } ExpressionStmt;
typedef struct { Stmt obj; Expr* expression; } PrintStmt;
typedef struct { Stmt obj; Token name; Expr* initializer; int sysType; WldDimensionVector dimConstraint; } VarStmt;
typedef struct { Stmt obj; int count; Stmt** statements; } BlockStmt;
typedef struct { Stmt obj; Expr* condition; Stmt* thenBranch; Stmt* elseBranch; } IfStmt;
typedef struct { Stmt obj; Expr* condition; Stmt* body; } WhileStmt;
typedef struct { Stmt obj; Token name; int arity; Token* params; Stmt* body; bool isAsync; } FunctionStmt;
typedef struct { Stmt obj; Token keyword; Expr* value; } ReturnStmt;
typedef struct { Stmt obj; Value effectName; Stmt* body; } HandleStmt;
typedef struct { Stmt obj; Expr* continuation; Expr* value; } ResumeStmt;
typedef struct { Stmt obj; Value description; Stmt* body; } TestStmt;
typedef struct { Token name; int type; int offset; int bitWidth; } LayoutMember;
typedef struct { Stmt obj; Token name; int memberCount; LayoutMember* members; int totalSize; } LayoutStmt;
typedef struct { Stmt obj; Token code; } AsmStmt;
typedef struct { Stmt obj; Expr* target; } ReleaseStmt;

typedef struct {
    Stmt obj;
    Token name;
    Expr* superclass;  // Added for Inheritance
    int methodCount;
    Stmt** methods;
    int traitCount;    // Added for Phase 19.3
    Expr** traits;
} ClassStmt;

typedef struct {
    Stmt obj;
    Token name;
    int methodCount;
    Stmt** methods;
    int traitCount;
    Stmt** traits;
} GeneStmt;

// ==========================================
// CONSTRUCTORS
// ==========================================

Expr* newBinary(Expr* left, Token op, Expr* right);
Expr* newGrouping(Expr* expression);
Expr* newLiteral(Value value);
Expr* newUnary(Token op, Expr* right);
Expr* newVariable(Token name);
Expr* newAssign(Token name, Expr* value);
Expr* newEntangle(Token name, Expr* source);
Expr* newPipe(Expr* left, Expr* right);
Expr* newMatch(Expr* target, int caseCount, MatchCase* cases);
Expr* newRange(Expr* min, Expr* max);
Expr* newLogical(Expr* left, Token op, Expr* right);
Expr* newCall(Expr* callee, Token paren, int argCount, Expr** arguments);
Expr* newPerform(Value effectName);
Expr* newAddress(Expr* target);
Expr* newDeref(Expr* target);
Expr* newSyscall(Expr* id, int argCount, Expr** arguments);
Expr* newSlice(Expr* target, int dims, Expr** starts, Expr** ends, Expr** steps);
Expr* newAlloc(Expr* size);
Expr* newSizeof(Token typeName);
Expr* newAwait(Expr* target);

Stmt* newExpressionStmt(Expr* expression);
Stmt* newPrintStmt(Expr* expression);
Stmt* newVarStmt(Token name, Expr* initializer);
Stmt* newBlockStmt(Stmt** statements, int count);
Stmt* newIfStmt(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
Stmt* newWhileStmt(Expr* condition, Stmt* body);
Stmt* newFunctionStmt(Token name, int arity, Token* params, Stmt* body, bool isAsync);
Stmt* newReturnStmt(Token keyword, Expr* value);
Stmt* newHandleStmt(Value effectName, Stmt* body);
Stmt* newResumeStmt(Expr* continuation, Expr* value);
Stmt* newTestStmt(Value description, Stmt* body);
Stmt* newLayoutStmt(Token name, int count, LayoutMember* members, int size);
Stmt* newAsmStmt(Token code);
Stmt* newClassStmt(Token name, Expr* superclass, int methodCount, Stmt** methods, int traitCount, Expr** traits);
Stmt* newGeneStmt(Token name, int methodCount, Stmt** methods, int traitCount, Stmt** traits);
Stmt* newReleaseStmt(Expr* target);

void freeExpr(Expr* expr);
void freeStmt(Stmt* stmt);

#endif
