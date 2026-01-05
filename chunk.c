#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    initValueArray(&chunk->constants);

    // Initialize Line Info
    chunk->lines.count = 0;
    chunk->lines.capacity = 0;
    chunk->lines.lines = NULL;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(Instruction, chunk->code, chunk->capacity);
    freeValueArray(&chunk->constants);
    
    FREE_ARRAY(LineStart, chunk->lines.lines, chunk->lines.capacity);
    
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, Instruction instruction, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(Instruction, chunk->code, 
            oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = instruction;
    
    // Add line info (compressed)
    addLine(chunk, line);

    chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
    // Phase 2 Optimization: Check if constant already exists?
    // For now, simple append. Phase 3 adds deduplication.
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

// ==========================================
// RUN-LENGTH ENCODING LOGIC
// ==========================================

void addLine(Chunk* chunk, int line) {
    LineInfo* info = &chunk->lines;

    // If this is the same line as the previous instruction, just increment count.
    if (info->count > 0 && info->lines[info->count - 1].line == line) {
        info->lines[info->count - 1].count++;
        return;
    }

    // Otherwise, start a new run.
    if (info->capacity < info->count + 1) {
        int oldCapacity = info->capacity;
        info->capacity = GROW_CAPACITY(oldCapacity);
        info->lines = GROW_ARRAY(LineStart, info->lines, oldCapacity, info->capacity);
    }

    LineStart* start = &info->lines[info->count++];
    start->line = line;
    start->count = 1;
}

int getLine(Chunk* chunk, int instructionIndex) {
    int start = 0;
    for (int i = 0; i < chunk->lines.count; i++) {
        LineStart* run = &chunk->lines.lines[i];
        start += run->count;
        if (start > instructionIndex) {
            return run->line;
        }
    }
    return -1; // Should not happen
}

// ==========================================
// DISASSEMBLER (Debug View)
// ==========================================

void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count; offset++) {
        disassembleInstruction(chunk, offset);
    }
}

// Helper to print registers like R0, R1...
#define REG(idx) printf(" R%d", idx)

void disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);

    int line = getLine(chunk, offset);
    if (offset > 0 && line == getLine(chunk, offset - 1)) {
        printf("   | ");
    } else {
        printf("%4d ", line);
    }

    Instruction inst = chunk->code[offset];
    int op = GET_OP(inst);
    int a = GET_A(inst);
    int b = GET_B(inst);
    int c = GET_C(inst);
    int bc = GET_BC(inst);

    switch (op) {
        case OP_LOAD_CONST:
            printf("%-16s R%d ", "LOAD_CONST", a);
            printValue(chunk->constants.values[bc]);
            printf("\n");
            break;
        case OP_LOAD_BOOL:
            printf("%-16s R%d %s\n", "LOAD_BOOL", a, a == 1 ? "true" : "false");
            break;
        case OP_LOAD_NIL:
            printf("%-16s R%d\n", "LOAD_NIL", a);
            break;
        case OP_MOVE:
            printf("%-16s R%d R%d\n", "MOVE", a, b);
            break;
        case OP_ADD:
            printf("%-16s R%d R%d R%d\n", "ADD", a, b, c);
            break;
        case OP_SUB:
            printf("%-16s R%d R%d R%d\n", "SUB", a, b, c);
            break;
        case OP_MUL:
            printf("%-16s R%d R%d R%d\n", "MUL", a, b, c);
            break;
        case OP_DIV:
            printf("%-16s R%d R%d R%d\n", "DIV", a, b, c);
            break;
        case OP_PRINT:
            printf("%-16s R%d\n", "PRINT", a);
            break;
        case OP_RETURN:
            printf("%-16s R%d\n", "RETURN", a);
            break;
        default:
            printf("Unknown opcode %d\n", op);
    }
}
