#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"

VM vm;

static void runtime_error(const char* format, ...);

static Value native_clock(int32 arg_count, [[maybe_unused]] Value* _args, bool* success) {
    if (arg_count != 0) {
        runtime_error("Expected 0 arguments but got %d.", arg_count);
        *success = false;
        return nil_val();
    }
    return number_val((float64) clock() / CLOCKS_PER_SEC);
}

static Value native_to_string(int32 arg_count, Value* args, bool* success) {
    if (arg_count != 1) {
        runtime_error("Expected 1 arguments but got %d.", arg_count);
        *success = false;
        return nil_val();
    }

    Value arg = args[0];
    if (is_number(arg)) {
        float64 number = as_number(arg);
        if (number == floor(number)) {
            char* str = ALLOCATE(char, 32);
            int32 length = snprintf(str, 32, "%.0f", number);
            if (length == -1) {
                FREE_ARRAY(char, str, 32);
                runtime_error("An error was thrown when parsing a number to string.");
                *success = false;
                return nil_val();
            }
            reallocate(str, 32, length + 1);
            return obj_val((Obj*) take_string(str, length));
        } else {
            char* str = ALLOCATE(char, 64);
            int32 length = snprintf(str, 64, "%g", number);
            if (length == -1) {
                FREE_ARRAY(char, str, 32);
                runtime_error("An error was thrown when parsing a number to string.");
                *success = false;
                return nil_val();
            }
            reallocate(str, 64, length + 1);
            return obj_val((Obj*) take_string(str, length));
        }
    } else if (is_nil(arg)) {
        return obj_val((Obj*) copy_string("nil", 3));
    } else if (is_bool(arg)) {
        bool val = as_bool(arg);
        if (val) {
            return obj_val((Obj*) copy_string("true", 4));
        } else {
            return obj_val((Obj*) copy_string("false", 5));
        }
    } else if (is_obj(arg)) {
        if (is_obj_type(arg, ObjectString)) {
            return arg;
        } else if (is_obj_type(arg, ObjectFunction)) {
            ObjFunction* function = as_function(arg);
            char* str = ALLOCATE(char, 1024);
            int32 length = snprintf(str, 1024, "<fn %s>", function->name->chars);
            if (length == -1) {
                FREE_ARRAY(char, str, 32);
                runtime_error("An error was thrown when parsing a function to string.");
                *success = false;
                return nil_val();
            }
            reallocate(str, 1024, length + 1);
            return obj_val((Obj*) take_string(str, length));
        } else if (is_obj_type(arg, ObjectNative)) {
            return obj_val((Obj*) take_string("<native fn>", 11));
        }
    }
    return nil_val();  // Unreachable.
}

static Value native_readline(int32 arg_count, [[maybe_unused]] Value* _args, bool* success) {
    if (arg_count != 0) {
        runtime_error("Expected 0 arguments but got %d.", arg_count);
        *success = false;
        return nil_val();
    }

    char* str = ALLOCATE(char, 1024);
    if (fgets(str, sizeof(char) * 1024, stdin) == NULL) {
        FREE_ARRAY(char, str, 1024);
        runtime_error("An error was thrown when reading a line from stdin.");
        *success = false;
        return nil_val();
    }
    str[strcspn(str, "\n")] = '\0';
    int32 length = strlen(str);
    reallocate(str, 1024, length + 1);
    return obj_val((Obj*) take_string(str, length));
}

static void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
    vm.open_upvalues = NULL;
}

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args);
    vfprintf(stderr, format, args);
    va_end(args);

    fputs("\n", stderr);

    for (int32 i = vm.frame_count - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->closure->function;
        usize instruction = frame->ip - function->chunk.code - 1;
        fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fprintf(stderr, "<script>\n");
        } else {
            fprintf(stderr, "%s()\n", function->name->chars);
        }
    }

    reset_stack();
}

static void define_native(const char* name, NativeFn function) {
    push(obj_val((Obj*) copy_string(name, (int32) strlen(name))));
    push(obj_val((Obj*) new_native(function)));
    table_set(&vm.globals, as_string(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

void init_vm() {
    reset_stack();
    vm.objects = NULL;
    vm.bytes_allocated = 0;
    vm.next_gc = 1024 * 1024;

    vm.gray_count = 0;
    vm.gray_capacity = 0;
    vm.gray_stack = NULL;

    init_table(&vm.globals);
    init_table(&vm.strings);

    define_native("clock", native_clock);
    define_native("to_string", native_to_string);
    define_native("readline", native_readline);
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

static bool call(ObjClosure* closure, int32 arg_count) {
    if (arg_count != closure->function->arity) {
        runtime_error("Expected %d arguments but got %d.", closure->function->arity, arg_count);
        return false;
    }
    if (vm.frame_count == FRAMES_MAX) {
        runtime_error("Stack overflow.");
        return false;
    }
    CallFrame* frame = &vm.frames[vm.frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    frame->slots = vm.stack_top - arg_count - 1;
    return true;
}

static bool call_value(Value callee, int32 arg_count) {
    if (is_obj(callee)) {
        switch (obj_type(callee)) {
            case ObjectBoundMethod: {
                ObjBoundMethod* bound = as_bound_method(callee);
                return call(bound->method, arg_count);
            }
            case ObjectClass: {
                ObjClass* class = as_class(callee);
                vm.stack_top[-arg_count - 1] = obj_val((Obj*) new_instance(class));
                return true;
            }
            case ObjectClosure: {
                return call(as_closure(callee), arg_count);
            }
            case ObjectNative: {
                NativeFn native = as_native(callee);
                bool success = true;
                Value result = native(arg_count, vm.stack_top - arg_count, &success);
                vm.stack_top -= arg_count + 1;
                push(result);
                return success;
            }
            default: {
                break;
            }
        }
    }
    runtime_error("Can only call functions and classes.");
    return false;
}

static bool bind_method(ObjClass* class, ObjString* name) {
    Value method;
    if (!table_get(&class->methods, name, &method)) {
        runtime_error("Undefined property `%s`.", name->chars);
        return false;
    }
    ObjBoundMethod* bound = new_bound_method(peek(0), as_closure(method));
    pop();
    push(obj_val((Obj*) bound));
    return true;
}

static ObjUpvalue* capture_upvalue(Value* local) {
    ObjUpvalue* prev_upvalue = NULL;
    ObjUpvalue* upvalue = vm.open_upvalues;
    while (upvalue != NULL && upvalue->location > local) {
        prev_upvalue = upvalue;
        upvalue = upvalue->next;
    }

    ObjUpvalue* created_upvalue = new_upvalue(local);
    created_upvalue -> next = upvalue;

    if (prev_upvalue == NULL) {
        vm.open_upvalues = created_upvalue;
    } else {
        prev_upvalue->next = created_upvalue;
    }
    return created_upvalue;
}

static void close_upvalues(Value* last) {
    while (vm.open_upvalues != NULL && vm.open_upvalues->location >= last) {
        ObjUpvalue* upvalue = vm.open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.open_upvalues = upvalue->next;
    }
}

static void define_method(ObjString* name) {
    Value method = peek(0);
    ObjClass* class = as_class(peek(1));
    table_set(&class->methods, name, method);
    pop();
}

static bool is_falsy(Value value) {
    return is_nil(value) || (is_bool(value) && !as_bool(value));
}

static void concatenate() {
    ObjString* b = as_string(peek(0));
    ObjString* a = as_string(peek(1));

    int32 length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = take_string(chars, length);
    pop();
    pop();
    push(obj_val((Obj*) result));
}

static InterpretResult run() {
#define READ_BYTE() \
    (*frame->ip++)
#define READ_SHORT() \
    (frame->ip += 2, (uint16) ((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() \
    (frame->closure->function->chunk.constants.values[READ_BYTE()])
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

    CallFrame* frame = &vm.frames[vm.frame_count - 1];

    while (true) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(&frame->closure->function->chunk, (int32) (frame->ip - frame->closure->function->chunk.code));
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
                push(frame->slots[slot]);
                break;
            }
            case OpSetLocal: {
                uint8 slot = READ_BYTE();
                frame->slots[slot] = peek(0);
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
            case OpGetUpvalue: {
                uint8 slot = READ_BYTE();
                push(*frame->closure->upvalues[slot]->location);
                break;
            }
            case OpSetUpvalue: {
                uint8 slot = READ_BYTE();
                *frame->closure->upvalues[slot]->location = peek(0);
                break;
            }
            case OpGetProperty: {
                if (!is_instance(peek(0))) {
                    runtime_error("Only instances have properties.");
                    return InterpretRuntimeError;
                }
                ObjInstance* instance = as_instance(peek(0));
                ObjString* name = READ_STRING();
                Value value;
                if (table_get(&instance->fields, name, &value)) {
                    pop();
                    push(value);
                    break;
                }
                if (!bind_method(instance->class, name)) {
                    return InterpretRuntimeError;
                }
                break;
            }
            case OpSetProperty: {
                if (!is_instance(peek(1))) {
                    runtime_error("Only instances have properties.");
                    return InterpretRuntimeError;
                }
                ObjInstance* instance = as_instance(peek(1));
                table_set(&instance->fields, READ_STRING(), peek(0));
                Value value = pop();
                pop();
                push(value);
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
            case OpJump: {
                uint16 offset = READ_SHORT();
                frame->ip += offset;
                break;
            }
            case OpJumpIfFalse: {
                uint16 offset = READ_SHORT();
                if (is_falsy(peek(0))) {
                    frame->ip += offset;
                }
                break;
            }
            case OpLoop: {
                uint16 offset = READ_SHORT();
                frame->ip -= offset;
                break;
            }
            case OpCall: {
                int32 arg_count = READ_BYTE();
                if (!call_value(peek(arg_count), arg_count)) {
                    return InterpretRuntimeError;
                }
                frame = &vm.frames[vm.frame_count - 1];
                break;
            }
            case OpClosure: {
                ObjFunction* function = as_function(READ_CONSTANT());
                ObjClosure* closure = new_closure(function);
                push(obj_val((Obj*) closure));
                for (int32 i = 0; i < closure->upvalue_count; i++) {
                    bool is_local = READ_BYTE() != 0;
                    uint8 index = READ_BYTE();
                    if (is_local) {
                        closure->upvalues[i] = capture_upvalue(frame->slots + index);
                    } else {
                        closure->upvalues[i] = frame->closure->upvalues[index];
                    }
                }
                break;
            }
            case OpCloseUpvalue: {
                close_upvalues(vm.stack_top - 1);
                pop();
                break;
            }
            case OpReturn: {
                Value result = pop();
                close_upvalues(frame->slots);
                vm.frame_count--;
                if (vm.frame_count == 0) {
                    pop();
                    return InterpretOk;
                }
                vm.stack_top = frame->slots;
                push(result);
                frame = &vm.frames[vm.frame_count - 1];
                break;
            }
            case OpClass: {
                push(obj_val((Obj*) new_class(READ_STRING())));
                break;
            }
            case OpMethod: {
                define_method(READ_STRING());
                break;
            }
        }
    }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    ObjFunction* function = compile(source);
    if (function == NULL) {
        return InterpretCompileError;
    }

    push(obj_val((Obj*) function));
    ObjClosure* closure = new_closure(function);
    pop();
    push(obj_val((Obj*) function));
    call(closure, 0);

    return run();
}
