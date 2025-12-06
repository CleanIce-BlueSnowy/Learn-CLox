#pragma once

#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
    Table strings;
    Obj* objects;
} VM;

typedef enum {
    InterpretOk,
    InterpretCompileError,
    InterpretRuntimeError,
} InterpretResult;

extern VM vm;

void init_vm();
void free_vm();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();
