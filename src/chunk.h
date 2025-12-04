#pragma once

#include "common.h"
#include "value.h"

typedef enum {
    OpConstant,
    OpNil,
    OpTrue,
    OpFalse,
    OpEqual,
    OpGreater,
    OpLess,
    OpAdd,
    OpSubtract,
    OpMultiply,
    OpDivide,
    OpNot,
    OpNegate,
    OpReturn,
} OpCode;

typedef struct {
    int32 count;
    int32 capacity;
    uint8* code;
    int32* lines;
    ValueArray constants;
} Chunk;

void init_chunk(Chunk* chunk);
void free_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8 byte, int32 line);
int32 add_constant(Chunk* chunk, Value value);
