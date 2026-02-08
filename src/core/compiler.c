#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "memory.h"
#include "opcode.h"
#include "../vm/vm_db_ops.h"
#include "../vm/vm_sys_ops.h"
#include "object.h"
#include "table.h"
#include "hash.h"
#include "../sys/sys_types.h"
#include "../sys/sys_units_check.h"
#include "../compiler_pipe.h"

SovereignCompiler* current = NULL;

// ==========================================
// IDENTITY & FINGERPRINTING
// ==========================================

static void computeFunctionIdentity(ObjFunction* function) {
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)function->chunk.code, 
                     function->chunk.count * sizeof(Instruction));
    for (int i = 0; i < function->chunk.constants.count; i++) {
        Value val = function->chunk.constants.values[i];
        walia_hash_update(&ctx, (uint8_t*)&val, sizeof(Value));
    }
    walia_hash_final(&ctx, function->fingerprint);
}

// ==========================================
// ALLOCATION & PACKING
// ==========================================

int allocReg() {
    if (current->packOffset > 0) { current->regTop++; current->packOffset = 0; }
    if (current->regTop >= 250) { hadError = true; return 0; }
    return current->regTop++;
}

static void allocSysLocal(WaliaSysType type, int* outReg, int* outOffset) {
    const SysTypeInfo* info = sys_get_type_info(type);
    if (info->size >= 8) { *outReg = allocReg(); *outOffset = 0; return; }
    if (current->packOffset == 0) current->packReg = current->regTop;
    if (current->packOffset + info->size <= 8) {
        *outReg = current->packReg; *outOffset = current->packOffset;
        current->packOffset += info->size;
        if (current->packOffset == 8) { current->regTop++; current->packOffset = 0; }
    } else {
        current->regTop++; current->packReg = current->regTop;
        *outReg = current->packReg; *outOffset = 0; current->packOffset = info->size;
    }
}

void freeReg(int reg) {
    if (reg == current->regTop - 1) { current->regTop--; current->packOffset = 0; }
}

void declareLocal(Token name, WaliaSysType type, int* reg, int* offset) {
    allocSysLocal(type, reg, offset);
    Local* local = &current->locals[current->localCount++];
    local->name = name; local->depth = current->scopeDepth;
    local->reg = *reg; local->offset = *offset;
    local->type = type; local->isCaptured = false;
}

// ==========================================
// CORE JIT EMITTERS
// ==========================================

Chunk* currentChunk() { return &current->function->chunk; }
void emitOp(int op, int a, int b, int c, int line) { writeChunk(currentChunk(), PACK_INSTRUCTION(op, a, b, c), line); }
void emitOpBC(int op, int a, int bc, int line) { writeChunk(currentChunk(), PACK_INSTRUCTION(op, a, (bc >> 8) & 0xFF, bc & 0xFF), line); }
int makeConstant(Value value) { return addConstant(currentChunk(), value); }

int emitJump(uint8_t instruction, int line) {
    emitOpBC(instruction, 0, 0xffff, line);
    return currentChunk()->count - 1;
}

void patchJump(int offset) {
    int jump = currentChunk()->count - offset - 1;
    Instruction inst = currentChunk()->code[offset];
    currentChunk()->code[offset] = PACK_INSTRUCTION(GET_OP(inst), GET_A(inst), (jump >> 8) & 0xFF, jump & 0xFF);
}

// ==========================================
// EXPRESSION COMPILER
// ==========================================

int compileExpr(Expr* expr, int targetReg) {
    int dst = (targetReg == -1) ? allocReg() : targetReg;
    int line = expr->line;

    switch (expr->type) {
        case EXPR_LITERAL: {
            Value val = ((LiteralExpr*)expr)->value;
            if (IS_NIL(val)) emitOp(OP_LOAD_NIL, dst, 0, 0, line);
            else if (IS_BOOL(val)) emitOp(OP_LOAD_BOOL, dst, AS_BOOL(val), 0, line);
            else emitOpBC(OP_LOAD_CONST, dst, makeConstant(val), line);
            break;
        }
        case EXPR_BINARY: {
            BinaryExpr* bin = (BinaryExpr*)expr;
            if (!sys_units_validate_binary(bin)) hadError = true;
            int r1 = compileExpr(bin->left, -1);
            int r2 = compileExpr(bin->right, -1);
            int op;
            switch (bin->operator.type) {
                case TOKEN_PLUS:  op = OP_ADD; break;
                case TOKEN_MINUS: op = OP_SUB; break;
                case TOKEN_STAR:  op = OP_MUL; break;
                case TOKEN_SLASH: op = OP_DIV; break;
                case TOKEN_EQUAL_EQUAL: op = OP_EQUAL; break;
                case TOKEN_GREATER: op = OP_GREATER; break;
                case TOKEN_LESS: op = OP_LESS; break;
                default: op = OP_ADD; break;
            }
            emitOp(op, dst, r1, r2, line);
            freeReg(r2); freeReg(r1);
            break;
        }
        case EXPR_VARIABLE: {
            Token name = ((VariableExpr*)expr)->name;
            int src = resolveLocal(current, name);
            if (src != -1) {
                emitOp(OP_MOVE, dst, src, 0, line);
                expr->dimensions = current->locals[src].dimensions;
            } else if ((src = resolveUpvalue(current, name)) != -1) {
                emitOpBC(OP_GET_UPVALUE, dst, src, line);
            } else {
                emitOpBC(OP_GET_GLOBAL, dst, makeConstant(OBJ_VAL(copyString(name.start, name.length))), line);
            }
            break;
        }
        case EXPR_PIPE: 
            walia_compiler_fuse_pipeline((PipeExpr*)expr, dst); 
            break;
        case EXPR_ENTANGLE: {
            EntangleExpr* e = (EntangleExpr*)expr;
            SovereignCompiler thunkCompiler;
            initCompiler(&thunkCompiler, TYPE_LAMBDA);
            int resReg = compileExpr(e->source, 0); 
            emitOp(OP_RETURN, resReg, 0, 0, line);
            ObjFunction* thunk = thunkCompiler.function;
            endCompiler();
            int sinkReg = resolveLocal(current, e->name);
            if (sinkReg != -1) emitOpBC(OP_ENTANGLE, sinkReg, makeConstant(OBJ_VAL(thunk)), line);
            break;
        }
        case EXPR_AWAIT: {
            AwaitExpr* a = (AwaitExpr*)expr;
            int fut = compileExpr(a->target, -1);
            emitOpBC(OP_PERFORM, dst, makeConstant(OBJ_VAL(copyString("ASYNC_WAIT", 10))), line);
            freeReg(fut);
            break;
        }
        case EXPR_SYSCALL: {
            SyscallExpr* s = (SyscallExpr*)expr;
            int id = compileExpr(s->id, -1);
            int start = allocReg();
            for(int i=0; i<s->argCount; i++) compileExpr(s->arguments[i], start+i);
            emitOp(OP_SYSCALL, dst, id, s->argCount, line);
            for(int i=0; i<s->argCount; i++) freeReg(start+i);
            freeReg(start); freeReg(id);
            break;
        }
        case EXPR_ALLOC: {
            AllocExpr* a = (AllocExpr*)expr;
            int sz = compileExpr(a->size, -1);
            emitOp(OP_MALLOC, dst, sz, 0, line);
            freeReg(sz);
            break;
        }
        case EXPR_DEREF: {
            DerefExpr* d = (DerefExpr*)expr;
            int addr = compileExpr(d->target, -1);
            emitOp(OP_PTR_READ, dst, addr, 0, line);
            freeReg(addr);
            break;
        }
        case EXPR_MATCH: {
            MatchExpr* m = (MatchExpr*)expr;
            int target = compileExpr(m->target, -1);
            emitOpBC(OP_MATCH_SIMILAR, target, m->caseCount, line);
            freeReg(target);
            break;
        }
        default: break;
    }
    return dst;
}

// ==========================================
// STATEMENT COMPILER
// ==========================================

void compileStmt(Stmt* stmt) {
    int line = stmt->line;
    switch (stmt->type) {
        case STMT_VAR: {
            VarStmt* var = (VarStmt*)stmt;
            WaliaSysType type = (WaliaSysType)var->sysType;
            int reg, offset;
            if (current->scopeDepth > 0) {
                declareLocal(var->name, type, &reg, &offset);
                current->locals[current->localCount-1].dimensions = var->dimConstraint;
                if (var->initializer) {
                    int temp = allocReg();
                    compileExpr(var->initializer, temp);
                    if (!sys_units_validate_assignment(var)) hadError = true;
                    if (type != SYS_TYPE_VALUE) emitOp(OP_SYS_STORE_U64, reg, temp, offset, line);
                    else emitOp(OP_MOVE, reg, temp, 0, line);
                    freeReg(temp);
                }
            } else {
                int temp = allocReg();
                if (var->initializer) compileExpr(var->initializer, temp);
                emitOpBC(OP_DEFINE_GLOBAL, temp, makeConstant(OBJ_VAL(copyString(var->name.start, var->name.length))), line);
                freeReg(temp);
            }
            break;
        }
        case STMT_FUNCTION: {
            FunctionStmt* node = (FunctionStmt*)stmt;
            SovereignCompiler funcCompiler;
            initCompiler(&funcCompiler, TYPE_FUNCTION);
            funcCompiler.function->name = copyString(node->name.start, node->name.length);
            funcCompiler.function->isAsync = node->isAsync;
            funcCompiler.scopeDepth++;
            for (int i = 0; i < node->arity; i++) { int r, o; declareLocal(node->params[i], SYS_TYPE_VALUE, &r, &o); }
            compileStmt(node->body);
            ObjFunction* function = funcCompiler.function;
            endCompiler();
            int fReg = allocReg();
            emitOpBC(OP_CLOSURE, fReg, makeConstant(OBJ_VAL(function)), line);
            if (current->scopeDepth == 0) emitOpBC(OP_DEFINE_GLOBAL, fReg, makeConstant(OBJ_VAL(function->name)), line);
            freeReg(fReg);
            break;
        }
        case STMT_IF: {
            IfStmt* s = (IfStmt*)stmt;
            int r = compileExpr(s->condition, -1);
            int thenJ = emitJump(OP_JUMP_IF_FALSE, line);
            currentChunk()->code[thenJ] |= (r << 8);
            compileStmt(s->thenBranch);
            int elseJ = emitJump(OP_JUMP, line);
            patchJump(thenJ);
            if (s->elseBranch) compileStmt(s->elseBranch);
            patchJump(elseJ);
            freeReg(r);
            break;
        }
        case STMT_CLASS: {
            ClassStmt* c = (ClassStmt*)stmt;
            int cReg = allocReg();
            emitOpBC(OP_CLASS, cReg, makeConstant(OBJ_VAL(copyString(c->name.start, c->name.length))), line);
            for(int i=0; i<c->methodCount; i++) compileStmt(c->methods[i]);
            freeReg(cReg);
            break;
        }
        case STMT_GENE: {
            GeneStmt* g = (GeneStmt*)stmt;
            int gReg = allocReg();
            emitOpBC(OP_GENE, gReg, makeConstant(OBJ_VAL(copyString(g->name.start, g->name.length))), line);
            for(int i=0; i<g->methodCount; i++) compileStmt(g->methods[i]);
            freeReg(gReg);
            break;
        }
        case STMT_ASM: {
            AsmStmt* a = (AsmStmt*)stmt;
            extern ObjCodeBlob* sys_assemble(const char*);
            emitOpBC(OP_ASM_EXEC, 0, makeConstant(OBJ_VAL(sys_assemble(a->code.start))), line);
            break;
        }
        case STMT_RETURN: {
            ReturnStmt* r = (ReturnStmt*)stmt;
            if (r->value) compileExpr(r->value, 0);
            emitOp(OP_RETURN, 0, 0, 0, line);
            break;
        }
        case STMT_BLOCK: {
            BlockStmt* b = (BlockStmt*)stmt;
            current->scopeDepth++;
            for(int i=0; i<b->count; i++) compileStmt(b->statements[i]);
            current->scopeDepth--;
            break;
        }
        case STMT_PRINT: {
            int r = compileExpr(((PrintStmt*)stmt)->expression, -1);
            emitOp(OP_PRINT, r, 0, 0, line);
            freeReg(r);
            break;
        }
        default: break;
    }
}

// ==========================================
// MASTER LIFECYCLE
// ==========================================

void initCompiler(SovereignCompiler* compiler, FunctionType type) {
    compiler->enclosing = current; compiler->type = type;
    compiler->localCount = 0; compiler->scopeDepth = 0;
    compiler->regTop = 1; compiler->packReg = 0; compiler->packOffset = 0;
    compiler->function = newFunction(); current = compiler;
    Local* l = &current->locals[current->localCount++];
    l->depth = 0; l->isCaptured = false; l->reg = 0; l->offset = 0;
    l->type = SYS_TYPE_VALUE; l->name.start = ""; l->name.length = 0;
}

bool compile(Stmt** statements, int count, Chunk* chunk) {
    SovereignCompiler compiler;
    initCompiler(&compiler, TYPE_SCRIPT);
    for (int i = 0; i < count; i++) { if (statements[i]) compileStmt(statements[i]); }
    endCompiler();
    *chunk = compiler.function->chunk;
    return !hadError;
}

// ==========================================
// MISSING IMPLEMENTATIONS (UFO REINFORCEMENT)
// ==========================================

void endCompiler() {
    emitOp(OP_RETURN, 0, 0, 0, 0);
    
    // UFO Grade: Finalize identity and restoration
    computeFunctionIdentity(current->function);
    
    // Restore enclosing compiler (parent or NULL)
    current = current->enclosing;
}

int resolveLocal(SovereignCompiler* compiler, Token name) {
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if (local->name.length == name.length && 
            memcmp(local->name.start, name.start, name.length) == 0) {
            return local->reg;
        }
    }
    return -1;
}

static int addUpvalue(SovereignCompiler* compiler, uint8_t index, bool isLocal) {
    int upvalueCount = compiler->function->upvalueCount;

    for (int i = 0; i < upvalueCount; i++) {
        Upvalue* upvalue = &compiler->upvalues[i];
        if (upvalue->index == index && upvalue->isLocal == isLocal) {
            return i;
        }
    }

    if (upvalueCount == 256) {
        hadError = true;
        return 0;
    }

    compiler->upvalues[upvalueCount].isLocal = isLocal;
    compiler->upvalues[upvalueCount].index = index;
    return compiler->function->upvalueCount++;
}

int resolveUpvalue(SovereignCompiler* compiler, Token name) {
    if (compiler->enclosing == NULL) return -1;

    int local = resolveLocal(compiler->enclosing, name);
    if (local != -1) {
        compiler->enclosing->locals[local].isCaptured = true;
        return addUpvalue(compiler, (uint8_t)local, true);
    }

    int upvalue = resolveUpvalue(compiler->enclosing, name);
    if (upvalue != -1) {
        return addUpvalue(compiler, (uint8_t)upvalue, false);
    }

    return -1;
}

void emitOpTo(Chunk* chunk, int op, int a, int b, int c, int line) {
    writeChunk(chunk, PACK_INSTRUCTION(op, a, b, c), line);
}

void emitOpBCTo(Chunk* chunk, int op, int a, int bc, int line) {
    writeChunk(chunk, PACK_INSTRUCTION(op, a, (bc >> 8) & 0xFF, bc & 0xFF), line);
}

int emitJumpTo(Chunk* chunk, uint8_t instruction, int line) {
    emitOpBCTo(chunk, instruction, 0, 0xffff, line);
    return chunk->count - 1;
}

void patchJumpTo(Chunk* chunk, int offset) {
    int jump = chunk->count - offset - 1;
    Instruction inst = chunk->code[offset];
    chunk->code[offset] = PACK_INSTRUCTION(GET_OP(inst), GET_A(inst), (jump >> 8) & 0xFF, jump & 0xFF);
}
