#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

VM vm;

static void reset_stack() {
    vm.stack_top = vm.stack;
}

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args);
    vfprintf(stderr, format, args);
    va_end(args);

    fputs("\n", stderr);

    usize instruction = vm.ip - vm.chunk->code - 1;
    int32 line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    reset_stack();
}

void init_vm() {
    reset_stack();
}

void free_vm() {}

void push(Value value) {
    *vm.stack_top = value;
    vm.stack_top++;
}

Value pop() {
    vm.stack_top--;
    return *vm.stack_top;
}

static Value peek(int32 distance) {
    return vm.stack_top[-1 - distance];
}

static InterpretResult run() {
#define READ_BYTE() \
    (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
        float64 b = pop(); \
        float64 a = pop(); \
        push(a op b); \
    } while (false);

    while (true) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(vm.chunk, (int32) (vm.ip - vm.chunk->code));
        #endif

        uint8 instruction;
        switch (instruction = READ_BYTE()) {
            case OpConstant: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpNegate: {
                if (!IS_NUMBER(peek(0))) {
                    runtime_error("Operand must be a number.");
                    return InterpretRuntimeError;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            case OpAdd: {
                BINARY_OP(+);
                break;
            }
            case OpSubtract: {
                BINARY_OP(-);
                break;
            }
            case OpMultiply: {
                BINARY_OP(*);
                break;
            }
            case OpDivide: {
                BINARY_OP(/);
                break;
            }
            case OpReturn: {
                print_value(pop());
                printf("\n");
                return InterpretOk;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    Chunk chunk;

    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return InterpretCompileError;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
}
