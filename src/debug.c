#include <stdio.h>

#include "debug.h"

void disassemble_chunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int32 offset = 0; offset < chunk->count; ) {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int32 simple_instruction(const char* name, int32 offset) {
    printf("%s\n", name);
    return offset + 1;
}

int32 disassemble_instruction(Chunk* chunk, int32 offset) {
    printf("%04d ", offset);

    uint8 instruction = chunk->code[offset];
    switch (instruction) {
        case OpReturn: {
            return simple_instruction("Return", offset);
        }
        default: {
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }
}
