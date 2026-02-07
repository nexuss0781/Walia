#ifndef SYSTEM_CHUNK_H
#define SYSTEM_CHUNK_H

#include "common.h"
#include "opcode.h"
#include "value.h"

// ==========================================
// LINE ENCODING (RLE)
// ==========================================
// Compressed debug info. 
// "Line 10 repeats 5 times" -> {10, 5}

typedef struct {
    int line;
    int count;
} LineStart;

typedef struct {
    int count;
    int capacity;
    LineStart* lines;
} LineInfo;

// ==========================================
// THE CHUNK (Bytecode Container)
// ==========================================

typedef struct {
    // The Code: Array of 32-bit instructions
    int count;
    int capacity;
    Instruction* code;

    // The Data: Pool of numbers, strings, etc.
    ValueArray constants;

    // The Debug Info: Source line mapping
    LineInfo lines;
} Chunk;

// ==========================================
// API
// ==========================================

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);

// Writes a single 32-bit instruction to the chunk.
// Also records which source line this instruction belongs to.
void writeChunk(Chunk* chunk, Instruction instruction, int line);

// Adds a constant to the pool and returns its index.
// Used by OP_LOAD_CONST.
int addConstant(Chunk* chunk, Value value);

// Helpers for RLE line info
void addLine(Chunk* chunk, int line);
int getLine(Chunk* chunk, int instructionIndex);

// Debugging
void disassembleChunk(Chunk* chunk, const char* name);
void disassembleInstruction(Chunk* chunk, int offset);

#endif // SYSTEM_CHUNK_H
