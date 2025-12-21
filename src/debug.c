#include <stdio.h>

#include "debug.h"
#include "object.h"
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

static int32 byte_instruction(const char* name, Chunk* chunk, int32 offset) {
    uint8 slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

static int32 jump_instruction(const char* name, int32 sign, Chunk* chunk, int32 offset) {
    uint16 jump = (uint16) (chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
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
        case OpNil: {
            return simple_instruction("Nil", offset);
        }
        case OpTrue: {
            return simple_instruction("True", offset);
        }
        case OpFalse: {
            return simple_instruction("False", offset);
        }
        case OpPop: {
            return simple_instruction("Pop", offset);
        }
        case OpGetLocal: {
            return byte_instruction("GetLocal", chunk, offset);
        }
        case OpSetLocal: {
            return byte_instruction("SetLocal", chunk, offset);
        }
        case OpGetGlobal: {
            return constant_instruction("GetGlobal", chunk, offset);
        }
        case OpDefineGlobal: {
            return constant_instruction("DefGlobal", chunk, offset);
        }
        case OpSetGlobal: {
            return constant_instruction("SetGlobal", chunk, offset);
        }
        case OpGetUpvalue: {
            return byte_instruction("GetUpvalue", chunk, offset);
        }
        case OpGetProperty: {
            return constant_instruction("GetProperty", chunk, offset);
        }
        case OpSetProperty: {
            return constant_instruction("SetProperty", chunk, offset);
        }
        case OpSetUpvalue: {
            return byte_instruction("SetUpvalue", chunk, offset);
        }
        case OpEqual: {
            return simple_instruction("Equal", offset);
        }
        case OpGreater: {
            return simple_instruction("Greater", offset);
        }
        case OpLess: {
            return simple_instruction("Less", offset);
        }
        case OpAdd: {
            return simple_instruction("Add", offset);
        }
        case OpSubtract: {
            return simple_instruction("Subtract", offset);
        }
        case OpMultiply: {
            return simple_instruction("Multiply", offset);
        }
        case OpDivide: {
            return simple_instruction("Divide", offset);
        }
        case OpNot: {
            return simple_instruction("Not", offset);
        }
        case OpNegate: {
            return simple_instruction("Negate", offset);
        }
        case OpPrint: {
            return simple_instruction("Print", offset);
        }
        case OpJump: {
            return jump_instruction("Jump", 1, chunk, offset);
        }
        case OpJumpIfFalse: {
            return jump_instruction("JumpFalse", 1, chunk, offset);
        }
        case OpLoop: {
            return jump_instruction("Loop", -1, chunk, offset);
        }
        case OpCall: {
            return byte_instruction("Call", chunk, offset);
        }
        case OpClosure: {
            offset++;
            uint8 constant = chunk->code[offset++];
            printf("%-16s %4d ", "Closure", constant);
            print_value(chunk->constants.values[constant]);
            printf("\n");

            ObjFunction* function = as_function(chunk->constants.values[constant]);
            for (int32 j = 0; j < function->upvalue_count; j++) {
                bool is_local = chunk->code[offset++] != 0;
                int32 index = chunk->code[offset++];
                printf("%04d      |                     %s %d\n", offset - 2, is_local ? "local" : "upvalue", index);
            }

            return offset;
        }
        case OpCloseUpvalue: {
            return simple_instruction("CloseUpvalue", offset);
        }
        case OpReturn: {
            return simple_instruction("Return", offset);
        }
        case OpClass: {
            return constant_instruction("Class", chunk, offset);
        }
        case OpMethod: {
            return constant_instruction("Method", chunk, offset);
        }
        default: {
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
        }
    }
}
