#pragma once

#include "chunk.h"

typedef struct {
    Chunk* chunk;
    uint8* ip;
} VM;

typedef enum {
    InterpretOk,
    InterpretCompileError,
    InterpretRuntimeError,
} InterpretResult;

void init_vm();
void free_vm();
InterpretResult interpret(Chunk* chunk);
