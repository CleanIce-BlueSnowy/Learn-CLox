#pragma once

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

typedef enum {
    ObjectClass,
    ObjectClosure,
    ObjectFunction,
    ObjectInstance,
    ObjectNative,
    ObjectString,
    ObjectUpvalue,
} ObjType;

struct Obj {
    ObjType type;
    bool is_marked;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int32 arity;
    int32 upvalue_count;
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

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int32 upvalue_count;
} ObjClosure;

typedef struct {
    Obj obj;
    ObjString* name;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass* class;
    Table fields;
} ObjInstance;

ObjClass* new_class(ObjString* name);
ObjClosure* new_closure(ObjFunction* function);
ObjFunction* new_function();
ObjInstance* new_instance(ObjClass* class);
ObjNative* new_native(NativeFn function);
ObjString* take_string(char* chars, int32 length);
ObjString* copy_string(const char* chars, int32 length);
ObjUpvalue* new_upvalue(Value* slot);
void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return is_obj(value) && as_obj(value)->type == type;
}

static inline ObjType obj_type(Value value) {
    return as_obj(value)->type;
}

static inline bool is_class(Value value) {
    return is_obj_type(value, ObjectClass);
}

static inline bool is_closure(Value value) {
    return is_obj_type(value, ObjectClosure);
}

static inline bool is_function(Value value) {
    return is_obj_type(value, ObjectFunction);
}

static inline bool is_instance(Value value) {
    return is_obj_type(value, ObjectInstance);
}

static inline bool is_native(Value value) {
    return is_obj_type(value, ObjectNative);
}

static inline bool is_string(Value value) {
    return is_obj_type(value, ObjectString);
}

static inline ObjClass* as_class(Value value) {
    return (ObjClass*) as_obj(value);
}

static inline ObjClosure* as_closure(Value value) {
    return (ObjClosure*) as_obj(value);
}

static inline ObjFunction* as_function(Value value) {
    return (ObjFunction*) as_obj(value);
}

static inline ObjInstance* as_instance(Value value) {
    return (ObjInstance*) as_obj(value);
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
