#ifndef WALIA_COMPILER_H
#define WALIA_COMPILER_H

#include "ast.h"
#include "chunk.h"
#include "token.h"
#include "../sys/sys_types.h"

// ==========================================
// COMPILER CORE STRUCTURES
// ==========================================
struct ObjFunction; 

typedef struct {
    Token name;
    int depth;
    int reg;
    int offset;        
    WaliaSysType type; 
    WldDimensionVector dimensions;
    bool isCaptured;
} Local;

typedef struct {
    uint8_t index;
    bool isLocal;
} Upvalue;

typedef enum {
    TYPE_SCRIPT,
    TYPE_FUNCTION,
    TYPE_TEST,
    TYPE_LAMBDA
} FunctionType;

typedef struct SovereignCompiler {
    struct SovereignCompiler* enclosing;
    struct ObjFunction* function; 
    FunctionType type;

    Local locals[256];
    int localCount;
    int scopeDepth;
    int regTop;
    
    int packReg;
    int packOffset;
    
    Upvalue upvalues[256];
} SovereignCompiler;

// ==========================================
// SHARED COMPILER STATE (EXTERN)
// ==========================================
extern SovereignCompiler* current;
extern bool hadError;

// ==========================================
// UNIFIED EMISSION API
// ==========================================

// Backend Interface (Public for JIT Extensions)
Chunk* currentChunk();
void emitOp(int op, int a, int b, int c, int line);
void emitOpBC(int op, int a, int bc, int line);
int  emitJump(uint8_t instruction, int line);
void patchJump(int offset);
int  makeConstant(Value value);

// Targeted Emitters (Used by WaliaDB JIT)
void emitOpTo(Chunk* chunk, int op, int a, int b, int c, int line);
void emitOpBCTo(Chunk* chunk, int op, int a, int bc, int line);
int  emitJumpTo(Chunk* chunk, uint8_t instruction, int line);
void patchJumpTo(Chunk* chunk, int offset);

// Allocators & Lifecycle
int  allocReg();
void freeReg(int reg);
void initCompiler(SovereignCompiler* compiler, FunctionType type);
void endCompiler();
int  compileExpr(Expr* expr, int targetReg);
void compileStmt(Stmt* stmt);
bool compile(Stmt** statements, int count, Chunk* chunk);

// Resolution
void declareLocal(Token name, WaliaSysType type, int* reg, int* offset);
int  resolveLocal(SovereignCompiler* compiler, Token name);
int  resolveUpvalue(SovereignCompiler* compiler, Token name);

#endif
