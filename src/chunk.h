#pragma once

#include "common.h"

typedef enum {
    OpReturn,
} OpCode;

typedef struct {
    int32 count;
    int32 capacity;
    uint8* code;
} Chunk;

void init_chunk(Chunk* chunk);
void free_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8 byte);
