#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "memory.h"

#define ALLOC_EXPR(type, enumType) (type*)allocateExpr(sizeof(type), enumType)
#define ALLOC_STMT(type, enumType) (type*)allocateStmt(sizeof(type), enumType)

static Expr* allocateExpr(size_t size, ExprType type) {
    Expr* expr = (Expr*)reallocate(NULL, 0, size);
    if (expr == NULL) exit(1);
    expr->type = type;
    expr->line = 0;
    memset(&expr->dimensions, 0, sizeof(WldDimensionVector));
    return expr;
}

static Stmt* allocateStmt(size_t size, StmtType type) {
    Stmt* stmt = (Stmt*)reallocate(NULL, 0, size);
    if (stmt == NULL) exit(1);
    stmt->type = type;
    stmt->line = 0;
    return stmt;
}

// ==========================================
// EXPRESSION CONSTRUCTORS
// ==========================================

Expr* newBinary(Expr* left, Token op, Expr* right) {
    BinaryExpr* expr = ALLOC_EXPR(BinaryExpr, EXPR_BINARY);
    expr->left = left; expr->operator = op; expr->right = right;
    expr->obj.line = op.line;
    return (Expr*)expr;
}

Expr* newGrouping(Expr* expression) {
    GroupingExpr* expr = ALLOC_EXPR(GroupingExpr, EXPR_GROUPING);
    expr->expression = expression;
    expr->obj.line = expression->line;
    return (Expr*)expr;
}

Expr* newLiteral(Value value) {
    LiteralExpr* expr = ALLOC_EXPR(LiteralExpr, EXPR_LITERAL);
    expr->value = value;
    return (Expr*)expr;
}

Expr* newUnary(Token op, Expr* right) {
    UnaryExpr* expr = ALLOC_EXPR(UnaryExpr, EXPR_UNARY);
    expr->operator = op; expr->right = right;
    expr->obj.line = op.line;
    return (Expr*)expr;
}

Expr* newVariable(Token name) {
    VariableExpr* expr = ALLOC_EXPR(VariableExpr, EXPR_VARIABLE);
    expr->name = name;
    expr->obj.line = name.line;
    return (Expr*)expr;
}

Expr* newAssign(Token name, Expr* value) {
    AssignExpr* expr = ALLOC_EXPR(AssignExpr, EXPR_ASSIGN);
    expr->name = name; expr->value = value;
    expr->obj.line = name.line;
    return (Expr*)expr;
}

Expr* newEntangle(Token name, Expr* source) {
    EntangleExpr* expr = ALLOC_EXPR(EntangleExpr, EXPR_ENTANGLE);
    expr->name = name; expr->source = source;
    expr->obj.line = name.line;
    return (Expr*)expr;
}

Expr* newPipe(Expr* left, Expr* right) {
    PipeExpr* expr = ALLOC_EXPR(PipeExpr, EXPR_PIPE);
    expr->left = left; expr->right = right;
    expr->obj.line = left->line;
    return (Expr*)expr;
}

Expr* newMatch(Expr* target, int caseCount, MatchCase* cases) {
    MatchExpr* expr = ALLOC_EXPR(MatchExpr, EXPR_MATCH);
    expr->target = target; expr->caseCount = caseCount; expr->cases = cases;
    expr->obj.line = target->line;
    return (Expr*)expr;
}

Expr* newRange(Expr* min, Expr* max) {
    RangeExpr* expr = ALLOC_EXPR(RangeExpr, EXPR_RANGE);
    expr->min = min; expr->max = max;
    expr->obj.line = min->line;
    return (Expr*)expr;
}

Expr* newLogical(Expr* left, Token op, Expr* right) {
    LogicalExpr* expr = ALLOC_EXPR(LogicalExpr, EXPR_LOGICAL);
    expr->left = left; expr->operator = op; expr->right = right;
    expr->obj.line = op.line;
    return (Expr*)expr;
}

Expr* newCall(Expr* callee, Token paren, int argCount, Expr** arguments) {
    CallExpr* expr = ALLOC_EXPR(CallExpr, EXPR_CALL);
    expr->callee = callee; expr->paren = paren; expr->argCount = argCount; expr->arguments = arguments;
    expr->obj.line = paren.line;
    return (Expr*)expr;
}

Expr* newPerform(Value effectName) {
    PerformExpr* expr = ALLOC_EXPR(PerformExpr, EXPR_PERFORM);
    expr->effectName = effectName;
    return (Expr*)expr;
}

Expr* newAddress(Expr* target) {
    AddressExpr* expr = ALLOC_EXPR(AddressExpr, EXPR_ADDRESS);
    expr->target = target;
    expr->obj.line = target->line;
    return (Expr*)expr;
}

Expr* newDeref(Expr* target) {
    DerefExpr* expr = ALLOC_EXPR(DerefExpr, EXPR_DEREF);
    expr->target = target;
    expr->obj.line = target->line;
    return (Expr*)expr;
}

Expr* newSyscall(Expr* id, int argCount, Expr** arguments) {
    SyscallExpr* expr = ALLOC_EXPR(SyscallExpr, EXPR_SYSCALL);
    expr->id = id; expr->argCount = argCount; expr->arguments = arguments;
    expr->obj.line = id->line;
    return (Expr*)expr;
}

Expr* newSlice(Expr* target, int dims, Expr** starts, Expr** ends, Expr** steps) {
    SliceExpr* expr = ALLOC_EXPR(SliceExpr, EXPR_SLICE);
    expr->target = target; expr->dimCount = dims; expr->starts = starts; expr->ends = ends; expr->steps = steps;
    expr->obj.line = target->line;
    return (Expr*)expr;
}

Expr* newAlloc(Expr* size) {
    AllocExpr* expr = ALLOC_EXPR(AllocExpr, EXPR_ALLOC);
    expr->size = size; expr->obj.line = size->line;
    return (Expr*)expr;
}

Expr* newSizeof(Token typeName) {
    SizeofExpr* expr = ALLOC_EXPR(SizeofExpr, EXPR_SIZEOF);
    expr->typeName = typeName; expr->obj.line = typeName.line;
    return (Expr*)expr;
}

Expr* newAwait(Expr* target) {
    AwaitExpr* expr = ALLOC_EXPR(AwaitExpr, EXPR_AWAIT);
    expr->target = target; expr->obj.line = target->line;
    return (Expr*)expr;
}

// ==========================================
// STATEMENT CONSTRUCTORS
// ==========================================

Stmt* newExpressionStmt(Expr* expression) {
    ExpressionStmt* stmt = ALLOC_STMT(ExpressionStmt, STMT_EXPRESSION);
    stmt->expression = expression; stmt->obj.line = expression->line;
    return (Stmt*)stmt;
}

Stmt* newPrintStmt(Expr* expression) {
    PrintStmt* stmt = ALLOC_STMT(PrintStmt, STMT_PRINT);
    stmt->expression = expression; stmt->obj.line = expression->line;
    return (Stmt*)stmt;
}

Stmt* newVarStmt(Token name, Expr* initializer) {
    VarStmt* stmt = ALLOC_STMT(VarStmt, STMT_VAR);
    stmt->name = name; stmt->initializer = initializer;
    stmt->obj.line = name.line; stmt->sysType = 0;
    memset(&stmt->dimConstraint, 0, sizeof(WldDimensionVector));
    return (Stmt*)stmt;
}

Stmt* newBlockStmt(Stmt** statements, int count) {
    BlockStmt* stmt = ALLOC_STMT(BlockStmt, STMT_BLOCK);
    stmt->statements = statements; stmt->count = count;
    return (Stmt*)stmt;
}

Stmt* newIfStmt(Expr* condition, Stmt* thenBranch, Stmt* elseBranch) {
    IfStmt* stmt = ALLOC_STMT(IfStmt, STMT_IF);
    stmt->condition = condition; stmt->thenBranch = thenBranch; stmt->elseBranch = elseBranch;
    stmt->obj.line = condition->line;
    return (Stmt*)stmt;
}

Stmt* newWhileStmt(Expr* condition, Stmt* body) {
    WhileStmt* stmt = ALLOC_STMT(WhileStmt, STMT_WHILE);
    stmt->condition = condition; stmt->body = body;
    stmt->obj.line = condition->line;
    return (Stmt*)stmt;
}

Stmt* newFunctionStmt(Token name, int arity, Token* params, Stmt* body, bool isAsync) {
    FunctionStmt* stmt = ALLOC_STMT(FunctionStmt, STMT_FUNCTION);
    stmt->name = name; stmt->arity = arity; stmt->params = params; stmt->body = body;
    stmt->isAsync = isAsync; stmt->obj.line = name.line;
    return (Stmt*)stmt;
}

Stmt* newReturnStmt(Token keyword, Expr* value) {
    ReturnStmt* stmt = ALLOC_STMT(ReturnStmt, STMT_RETURN);
    stmt->keyword = keyword; stmt->value = value; stmt->obj.line = keyword.line;
    return (Stmt*)stmt;
}

Stmt* newHandleStmt(Value effectName, Stmt* body) {
    HandleStmt* stmt = ALLOC_STMT(HandleStmt, STMT_HANDLE);
    stmt->effectName = effectName; stmt->body = body;
    return (Stmt*)stmt;
}

Stmt* newResumeStmt(Expr* continuation, Expr* value) {
    ResumeStmt* stmt = ALLOC_STMT(ResumeStmt, STMT_RESUME);
    stmt->continuation = continuation; stmt->value = value;
    return (Stmt*)stmt;
}

Stmt* newTestStmt(Value description, Stmt* body) {
    TestStmt* stmt = ALLOC_STMT(TestStmt, STMT_TEST);
    stmt->description = description; stmt->body = body;
    return (Stmt*)stmt;
}

Stmt* newLayoutStmt(Token name, int count, LayoutMember* members, int size) {
    LayoutStmt* stmt = ALLOC_STMT(LayoutStmt, STMT_LAYOUT);
    stmt->name = name; stmt->memberCount = count; stmt->members = members; stmt->totalSize = size;
    stmt->obj.line = name.line;
    return (Stmt*)stmt;
}

Stmt* newAsmStmt(Token code) {
    AsmStmt* stmt = ALLOC_STMT(AsmStmt, STMT_ASM);
    stmt->code = code; stmt->obj.line = code.line;
    return (Stmt*)stmt;
}

Stmt* newClassStmt(Token name, Expr* superclass, int methodCount, Stmt** methods, int traitCount, Expr** traits) {
    ClassStmt* stmt = ALLOC_STMT(ClassStmt, STMT_CLASS);
    stmt->name = name;
    stmt->superclass = superclass;
    stmt->methodCount = methodCount;
    stmt->methods = methods;
    stmt->traitCount = traitCount;
    stmt->traits = traits;
    stmt->obj.line = name.line;
    return (Stmt*)stmt;
}

Stmt* newGeneStmt(Token name, int methodCount, Stmt** methods, int traitCount, Stmt** traits) {
    GeneStmt* stmt = ALLOC_STMT(GeneStmt, STMT_GENE);
    stmt->name = name; stmt->methodCount = methodCount; stmt->methods = methods;
    stmt->traitCount = traitCount; stmt->traits = traits;
    stmt->obj.line = name.line;
    return (Stmt*)stmt;
}

Stmt* newReleaseStmt(Expr* target) {
    ReleaseStmt* stmt = ALLOC_STMT(ReleaseStmt, STMT_RELEASE);
    stmt->target = target; stmt->obj.line = target->line;
    return (Stmt*)stmt;
}

// ==========================================
// RECURSIVE CLEANUP
// ==========================================

void freeExpr(Expr* expr) {
    if (expr == NULL) return;
    switch (expr->type) {
        case EXPR_BINARY: 
            freeExpr(((BinaryExpr*)expr)->left); 
            freeExpr(((BinaryExpr*)expr)->right); 
            break;
        case EXPR_GROUPING: 
            freeExpr(((GroupingExpr*)expr)->expression); 
            break;
        case EXPR_UNARY: 
            freeExpr(((UnaryExpr*)expr)->right); 
            break;
        case EXPR_ASSIGN: 
            freeExpr(((AssignExpr*)expr)->value); 
            break;
        case EXPR_LOGICAL: 
            freeExpr(((LogicalExpr*)expr)->left); 
            freeExpr(((LogicalExpr*)expr)->right); 
            break;
        case EXPR_CALL: {
            CallExpr* c = (CallExpr*)expr;
            freeExpr(c->callee);
            for(int i = 0; i < c->argCount; i++) freeExpr(c->arguments[i]);
            free(c->arguments);
            break;
        }
        case EXPR_ADDRESS: freeExpr(((AddressExpr*)expr)->target); break;
        case EXPR_DEREF:   freeExpr(((DerefExpr*)expr)->target); break;
        case EXPR_SYSCALL: {
            SyscallExpr* s = (SyscallExpr*)expr;
            freeExpr(s->id);
            for(int i = 0; i < s->argCount; i++) freeExpr(s->arguments[i]);
            free(s->arguments);
            break;
        }
        case EXPR_ALLOC: freeExpr(((AllocExpr*)expr)->size); break;
        case EXPR_SLICE: {
            SliceExpr* s = (SliceExpr*)expr;
            freeExpr(s->target);
            for(int i = 0; i < s->dimCount; i++) {
                if (s->starts[i]) freeExpr(s->starts[i]);
                if (s->ends[i])   freeExpr(s->ends[i]);
                if (s->steps[i])  freeExpr(s->steps[i]);
            }
            free(s->starts); free(s->ends); free(s->steps);
            break;
        }
        case EXPR_AWAIT:    freeExpr(((AwaitExpr*)expr)->target); break;
        case EXPR_ENTANGLE: freeExpr(((EntangleExpr*)expr)->source); break;
        case EXPR_PIPE:     freeExpr(((PipeExpr*)expr)->left); freeExpr(((PipeExpr*)expr)->right); break;
        case EXPR_RANGE:    freeExpr(((RangeExpr*)expr)->min); freeExpr(((RangeExpr*)expr)->max); break;
        case EXPR_MATCH: {
            MatchExpr* m = (MatchExpr*)expr;
            freeExpr(m->target);
            for (int i = 0; i < m->caseCount; i++) {
                if (m->cases[i].pattern) freeExpr(m->cases[i].pattern);
                if (m->cases[i].guard)   freeExpr(m->cases[i].guard);
                if (m->cases[i].body)    freeStmt(m->cases[i].body);
            }
            free(m->cases);
            break;
        }
        case EXPR_LITERAL:
        case EXPR_VARIABLE:
        case EXPR_PERFORM: 
        case EXPR_SIZEOF:
            break;
    }
    reallocate(expr, 0, 0);
}

void freeStmt(Stmt* stmt) {
    if (stmt == NULL) return;
    switch (stmt->type) {
        case STMT_EXPRESSION: freeExpr(((ExpressionStmt*)stmt)->expression); break;
        case STMT_PRINT:      freeExpr(((PrintStmt*)stmt)->expression); break;
        case STMT_VAR:        freeExpr(((VarStmt*)stmt)->initializer); break;
        case STMT_BLOCK: {
            BlockStmt* b = (BlockStmt*)stmt;
            for(int i=0; i<b->count; i++) freeStmt(b->statements[i]);
            free(b->statements);
            break;
        }
        case STMT_IF: 
            freeExpr(((IfStmt*)stmt)->condition); 
            freeStmt(((IfStmt*)stmt)->thenBranch); 
            if (((IfStmt*)stmt)->elseBranch) freeStmt(((IfStmt*)stmt)->elseBranch); 
            break;
        case STMT_WHILE: 
            freeExpr(((WhileStmt*)stmt)->condition); 
            freeStmt(((WhileStmt*)stmt)->body); 
            break;
        case STMT_FUNCTION: 
            freeStmt(((FunctionStmt*)stmt)->body); 
            if (((FunctionStmt*)stmt)->params) free(((FunctionStmt*)stmt)->params);
            break;
        case STMT_RETURN:  if (((ReturnStmt*)stmt)->value) freeExpr(((ReturnStmt*)stmt)->value); break;
        case STMT_HANDLE:  freeStmt(((HandleStmt*)stmt)->body); break;
        case STMT_RESUME:  freeExpr(((ResumeStmt*)stmt)->continuation); freeExpr(((ResumeStmt*)stmt)->value); break;
        case STMT_TEST:    freeStmt(((TestStmt*)stmt)->body); break;
        case STMT_LAYOUT:  free(((LayoutStmt*)stmt)->members); break;
        case STMT_RELEASE: freeExpr(((ReleaseStmt*)stmt)->target); break;
        case STMT_CLASS: {
            ClassStmt* c = (ClassStmt*)stmt;
            for (int i = 0; i < c->methodCount; i++) freeStmt(c->methods[i]);
            free(c->methods);
            break;
        }
        case STMT_GENE: {
            GeneStmt* g = (GeneStmt*)stmt;
            for (int i = 0; i < g->methodCount; i++) freeStmt(g->methods[i]);
            for (int i = 0; i < g->traitCount; i++) freeStmt(g->traits[i]);
            free(g->methods); free(g->traits);
            break;
        }
        case STMT_ASM: break;
    }
    reallocate(stmt, 0, 0);
}
