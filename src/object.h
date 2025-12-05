#pragma once

#include "common.h"
#include "value.h"

typedef enum {
    ObjectString,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct ObjString {
    Obj obj;
    int32 length;
    char* chars;
    uint32 hash;
};

ObjString* take_string(char* chars, int32 length);
ObjString* copy_string(const char* chars, int32 length);
void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return is_obj(value) && as_obj(value)->type == type;
}

static inline ObjType obj_type(Value value) {
    return as_obj(value)->type;
}

static inline bool is_string(Value value) {
    return is_obj_type(value, ObjectString);
}

static inline ObjString* as_string(Value value) {
    return (ObjString*) as_obj(value);
}

static inline char* as_cstring(Value value) {
    return as_string(value)->chars;
}
