#include <stdio.h>

#include "common.h"
#include "vm.h"

VM vm;

void init_vm() {}

void free_vm() {}

static InterpretResult run() {
#define READ_BYTE() \
    (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])

    while (true) {
        uint8 instruction;
        switch (instruction = READ_BYTE()) {
            case OpConstant: {
                Value constant = READ_CONSTANT();
                print_value(constant);
                printf("\n");
                break;
            }
            case OpReturn: {
                return InterpretOk;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
