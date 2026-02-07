#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sql/sql_compiler.h"
#include "core/opcode.h"
#include "core/compiler.h"
#include "db/db_row.h"
#include "core/hash.h"

bool db_sql_compile(SqlQuery* query, WldTableDescriptor* schema, Chunk* chunk) {
    // 1. Initialize result set
    emitOpTo(chunk, OP_CALL, 0, 0, 0, 0);

    int pageReg = 1, rowPtrReg = 2, filterReg = 3;
    emitOpBCTo(chunk, OP_LOAD_CONST, pageReg, schema->btreeRoot, 0);

    int loopStart = chunk->count;
    emitOpTo(chunk, OP_DB_LOAD_COL, rowPtrReg, pageReg, 0, 0);

    // 2. Simple Predicate
    emitOpTo(chunk, OP_LOAD_BOOL, filterReg, 1, 0, 0);

    // 3. Conditional Jump (Uses emitJumpTo for 3 args)
    int jumpIfFalse = emitJumpTo(chunk, OP_JUMP_IF_FALSE, 0);
    chunk->code[jumpIfFalse] |= (filterReg << 8);

    for (int i = 0; i < query->projectionCount; i++) {
        emitOpTo(chunk, OP_DB_LOAD_COL, 4, rowPtrReg, i, 0);
    }

    patchJumpTo(chunk, jumpIfFalse);
    emitOpBCTo(chunk, OP_LOOP, 0, chunk->count - loopStart, 0);
    emitOpTo(chunk, OP_RETURN, 0, 0, 0, 0);

    return true;
}

void db_sql_generate_plan_hash(SqlQuery* query, uint8_t* outHash) {
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (uint8_t*)query->sourceTable, strlen(query->sourceTable));
    walia_hash_final(&ctx, outHash);
}
