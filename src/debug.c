#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassemble_chunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for (int32 offset = 0; offset < chunk->count; ) {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int32 constant_instruction(const char* name, Chunk* chunk, int32 offset) {
    uint8 constant = chunk->code[offset + 1];
    printf("%-16s %4d `", name, constant);
    print_value(chunk->constants.values[constant]);
    printf("`\n");
    return offset + 2;
}

static int32 simple_instruction(const char* name, int32 offset) {
    printf("%s\n", name);
    return offset + 1;
}

int32 disassemble_instruction(Chunk* chunk, int32 offset) {
    printf("%04d ", offset);

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8 instruction = chunk->code[offset];
    switch (instruction) {
        case OpConstant: {
            return constant_instruction("Constant", chunk, offset);
        }
        case OpReturn: {
            return simple_instruction("Return", offset);
        }
        default: {
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }
}
