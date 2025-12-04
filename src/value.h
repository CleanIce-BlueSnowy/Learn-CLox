#pragma once

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
    ValBool,
    ValNil,
    ValNumber,
    ValObj,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        float64 number;
        Obj* obj;
    } as;
} Value;

typedef struct {
    int32 capacity;
    int32 count;
    Value* values;
} ValueArray;

bool values_equal(Value a, Value b);
void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);
void print_value(Value value);

static inline bool is_bool(Value value) {
    return value.type == ValBool;
}

static inline bool is_nil(Value value) {
    return value.type == ValNil;
}

static inline bool is_number(Value value) {
    return value.type == ValNumber;
}

static inline bool is_obj(Value value) {
    return value.type == ValObj;
}

static inline bool as_bool(Value value) {
    return value.as.boolean;
}

static inline float64 as_number(Value value) {
    return value.as.number;
}

static inline Obj* as_obj(Value value) {
    return value.as.obj;
}

static inline Value bool_val(bool value) {
    return (Value) {
        .type = ValBool,
        .as = {
            .boolean = value,
        },
    };
}

static inline Value nil_val() {
    return (Value) {
        .type = ValNil,
        .as = {
            .number = 0,
        },
    };
}

static inline Value number_val(float64 value) {
    return (Value) {
        .type = ValNumber,
        .as = {
            .number = value,
        },
    };
}

static inline Value obj_val(Obj* object) {
    return (Value) {
        .type = ValObj,
        .as = {
            .obj = object,
        },
    };
}
