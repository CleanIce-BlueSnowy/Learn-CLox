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
            return constant_instruction("constant", chunk, offset);
        }
        case OpNil: {
            return simple_instruction("nil", offset);
        }
        case OpTrue: {
            return simple_instruction("true", offset);
        }
        case OpFalse: {
            return simple_instruction("false", offset);
        }
        case OpEqual: {
            return simple_instruction("equal", offset);
        }
        case OpGreater: {
            return simple_instruction("greater", offset);
        }
        case OpLess: {
            return simple_instruction("less", offset);
        }
        case OpAdd: {
            return simple_instruction("add", offset);
        }
        case OpSubtract: {
            return simple_instruction("subtract", offset);
        }
        case OpMultiply: {
            return simple_instruction("multiply", offset);
        }
        case OpDivide: {
            return simple_instruction("divide", offset);
        }
        case OpNot: {
            return simple_instruction("not", offset);
        }
        case OpNegate: {
            return simple_instruction("negate", offset);
        }
        case OpReturn: {
            return simple_instruction("return", offset);
        }
        default: {
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }
}
