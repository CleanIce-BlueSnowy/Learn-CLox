#pragma once

#include "common.h"
#include "chunk.h"
#include "value.h"

typedef enum {
    ObjectFunction,
    ObjectNative,
    ObjectString,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int32 arity;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

typedef Value (*NativeFn)(int32 arg_count, Value* args, bool* success);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    int32 length;
    char* chars;
    uint32 hash;
};

ObjFunction* new_function();
ObjNative* new_native(NativeFn function);
ObjString* take_string(char* chars, int32 length);
ObjString* copy_string(const char* chars, int32 length);
void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return is_obj(value) && as_obj(value)->type == type;
}

static inline ObjType obj_type(Value value) {
    return as_obj(value)->type;
}

static inline bool is_function(Value value) {
    return is_obj_type(value, ObjectFunction);
}

static inline bool is_native(Value value) {
    return is_obj_type(value, ObjectNative);
}

static inline bool is_string(Value value) {
    return is_obj_type(value, ObjectString);
}

static inline ObjFunction* as_function(Value value) {
    return (ObjFunction*) as_obj(value);
}

static inline NativeFn as_native(Value value) {
    return ((ObjNative*) as_obj(value))->function;
}

static inline ObjString* as_string(Value value) {
    return (ObjString*) as_obj(value);
}

static inline char* as_cstring(Value value) {
    return as_string(value)->chars;
}
