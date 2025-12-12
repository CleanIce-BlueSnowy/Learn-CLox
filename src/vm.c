#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
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
    fprintf(stderr, "[line %d] in script.\n", line);
    reset_stack();
}

void init_vm() {
    reset_stack();
    vm.objects = NULL;
    init_table(&vm.globals);
    init_table(&vm.strings);
}

void free_vm() {
    free_table(&vm.globals);
    free_table(&vm.strings);
    free_objects();
}

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

static bool is_falsy(Value value) {
    return is_nil(value) || (is_bool(value) && !as_bool(value));
}

static void concatenate() {
    ObjString* b = as_string(pop());
    ObjString* a = as_string(pop());

    int32 length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = take_string(chars, length);
    push(obj_val((Obj*) result));
}

static InterpretResult run() {
#define READ_BYTE() \
    (*vm.ip++)
#define READ_CONSTANT() \
    (vm.chunk->constants.values[READ_BYTE()])
#define READ_SHORT() \
    (vm.ip += 2, (uint16) ((vm.ip[-2] << 8) | vm.ip[-1]))
#define READ_STRING() \
    as_string(READ_CONSTANT())
#define BINARY_OP(value_type, op) \
    do { \
        if (!is_number(peek(0)) || !is_number(peek(1))) { \
            runtime_error("Operands must be numbers."); \
            return InterpretRuntimeError; \
        } \
        float64 b = as_number(pop()); \
        float64 a = as_number(pop()); \
        push(value_type(a op b)); \
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
            case OpNil: {
                push(nil_val());
                break;
            }
            case OpTrue: {
                push(bool_val(true));
                break;
            }
            case OpFalse: {
                push(bool_val(false));
                break;
            }
            case OpPop: {
                pop();
                break;
            }
            case OpGetLocal: {
                uint8 slot = READ_BYTE();
                push(vm.stack[slot]);
                break;
            }
            case OpSetLocal: {
                uint8 slot = READ_BYTE();
                vm.stack[slot] = peek(0);
                break;
            }
            case OpGetGlobal: {
                ObjString* name = READ_STRING();
                Value value;
                if (!table_get(&vm.globals, name, &value)) {
                    runtime_error("Undefined variable `%s`.", name->chars);
                    return InterpretRuntimeError;
                }
                push(value);
                break;
            }
            case OpDefineGlobal: {
                ObjString* name = READ_STRING();
                table_set(&vm.globals, name, peek(0));
                pop();
                break;
            }
            case OpSetGlobal: {
                ObjString* name = READ_STRING();
                if (table_set(&vm.globals, name, peek(0))) {
                    table_delete(&vm.globals, name);
                    runtime_error("Undefined variable `%s`.", name->chars);
                    return InterpretRuntimeError;
                }
                break;
            }
            case OpEqual: {
                Value b = pop();
                Value a = pop();
                push(bool_val(values_equal(a, b)));
                break;
            }
            case OpGreater: {
                BINARY_OP(bool_val, >);
                break;
            }
            case OpLess: {
                BINARY_OP(bool_val, <);
                break;
            }
            case OpAdd: {
                if (is_string(peek(0)) && is_string(peek(1))) {
                    concatenate();
                } else if (is_number(peek(0)) && is_number(peek(1))) {
                    float64 b = as_number(pop());
                    float64 a = as_number(pop());
                    push(number_val(a + b));
                } else {
                    runtime_error("Operands must be two numbers or two strings.");
                    return InterpretRuntimeError;
                }
                break;
            }
            case OpSubtract: {
                BINARY_OP(number_val, -);
                break;
            }
            case OpMultiply: {
                BINARY_OP(number_val, *);
                break;
            }
            case OpDivide: {
                BINARY_OP(number_val, /);
                break;
            }
            case OpNot: {
                push(bool_val(is_falsy(pop())));
                break;
            }
            case OpNegate: {
                if (!is_number(peek(0))) {
                    runtime_error("Operand must be a number.");
                    return InterpretRuntimeError;
                }
                push(number_val(-as_number(pop())));
                break;
            }
            case OpPrint: {
                print_value(pop());
                printf("\n");
                break;
            }
            case OpJumpIfFalse: {
                uint16 offset = READ_SHORT();
                if (is_falsy(peek(0))) {
                    vm.ip += offset;
                }
                break;
            }
            case OpReturn: {
                return InterpretOk;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef READ_STRING
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
