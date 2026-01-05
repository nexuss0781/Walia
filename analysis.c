#include <stdio.h>
#include <string.h>

#include "analysis.h"
#include "memory.h"
#include "compiler.h"
#include "tooling_registry.h" // Fixed: Added for walia_registry_resolve
#include "tooling_ledger.h"   // Fixed: Added for walia_ledger_log
#include "ast.h"

// ==========================================
// SEMANTIC ANALYSIS ENGINE
// ==========================================

static void analyzeExpr(Expr* expr) {
    if (expr == NULL) return;
    
    switch (expr->type) {
        case EXPR_BINARY: {
            BinaryExpr* b = (BinaryExpr*)expr;
            analyzeExpr(b->left);
            analyzeExpr(b->right);
            break;
        }
        case EXPR_ASSIGN: {
            AssignExpr* a = (AssignExpr*)expr;
            analyzeExpr(a->value);
            break;
        }
        case EXPR_CALL: {
            CallExpr* c = (CallExpr*)expr;
            analyzeExpr(c->callee);
            for(int i = 0; i < c->argCount; i++) {
                analyzeExpr(c->arguments[i]);
            }
            break;
        }
        case EXPR_VARIABLE: {
            // PHASE 10.2 INTEGRITY CHECK
            VariableExpr* v = (VariableExpr*)expr;
            
            // 1. Resolve through the unified Compiler scope
            if (resolveLocal(current, v->name) == -1 && resolveUpvalue(current, v->name) == -1) {
                
                // 2. Query the Project-Wide Sovereign Registry
                // Note: v->name.start is null-terminated during tokenization or 
                // we use a length-safe resolution.
                if (walia_registry_resolve(v->name.start) == NULL) {
                    
                    // 3. PHYSICAL FAULT LOGGING
                    char reason[256];
                    snprintf(reason, 256, "Undefined identifier '%.*s'.", v->name.length, v->name.start);
                    
                    // Blocks the build via the Diagnostic Ledger
                    walia_ledger_log(FAULT_CRITICAL, reason, "Semantic Analyzer", "logic", v->obj.line, NULL, CURE_NONE);
                }
            }
            break;
        }
        case EXPR_LOGICAL: {
            LogicalExpr* l = (LogicalExpr*)expr;
            analyzeExpr(l->left);
            analyzeExpr(l->right);
            break;
        }
        case EXPR_GROUPING: {
            analyzeExpr(((GroupingExpr*)expr)->expression);
            break;
        }
        default: break;
    }
}

static void analyzeStmt(Stmt* stmt) {
    if (stmt == NULL) return;
    
    switch (stmt->type) {
        case STMT_VAR: {
            VarStmt* v = (VarStmt*)stmt;
            if (v->initializer != NULL) analyzeExpr(v->initializer);
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* b = (BlockStmt*)stmt;
            for(int i = 0; i < b->count; i++) {
                analyzeStmt(b->statements[i]);
            }
            break;
        }
        case STMT_IF: {
            IfStmt* i = (IfStmt*)stmt;
            analyzeExpr(i->condition);
            analyzeStmt(i->thenBranch);
            if (i->elseBranch != NULL) analyzeStmt(i->elseBranch);
            break;
        }
        case STMT_WHILE: {
            WhileStmt* w = (WhileStmt*)stmt;
            analyzeExpr(w->condition);
            analyzeStmt(w->body);
            break;
        }
        case STMT_EXPRESSION: 
            analyzeExpr(((ExpressionStmt*)stmt)->expression); 
            break;
        case STMT_PRINT:
            analyzeExpr(((PrintStmt*)stmt)->expression);
            break;
        case STMT_RETURN: {
            ReturnStmt* r = (ReturnStmt*)stmt;
            if (r->value != NULL) analyzeExpr(r->value);
            break;
        }
        default: break;
    }
}

bool analyze(Stmt** statements, int count) {
    // UFO SPEED: Perform a parallel-ready pass over the AST
    // to validate semantic contracts before JIT emission.
    for (int i = 0; i < count; i++) {
        if (statements[i]) {
            analyzeStmt(statements[i]);
        }
    }
    
    // Return the global error state defined in parser.c/common.h
    return !hadError;
}