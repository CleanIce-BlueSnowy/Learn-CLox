#pragma once

#include <string.h>

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

#ifdef NAN_BOXING

#define SIGN_BIT ((uint64) 0x8000000000000000)
#define QNAN ((uint64) 0x7ffc000000000000)
#define TAG_NIL 1
#define TAG_FALSE 2
#define TAG_TRUE 3

typedef uint64 Value;

#else

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

#endif

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

#ifdef NAN_BOXING

static inline Value num_to_value(float64 num) {
    Value value;
    memcpy(&value, &num, sizeof(float64));
    return value;
}

static inline float64 value_to_num(Value value) {
    float64 num;
    memcpy(&num, &value, sizeof(Value));
    return num;
}

static inline Value true_val();
static inline Value false_val();
static inline bool is_bool(Value value) {
    return value == true_val() || value == false_val();
}

static inline Value nil_val();
static inline bool is_nil(Value value) {
    return value == nil_val();
}

static inline bool is_number(Value value) {
    return (value & QNAN) != QNAN;
}

static inline bool is_obj(Value value) {
    return (value & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT);
}

static inline Value true_val();
static inline bool as_bool(Value value) {
    return value == true_val();
}

static inline float64 as_number(Value value) {
    return value_to_num(value);
}

static inline Obj* as_obj(Value value) {
    return (Obj*) (uintptr) (value & ~(SIGN_BIT | QNAN));
}

static inline Value false_val() {
    return (Value) (uint64) (QNAN | TAG_FALSE);
}

static inline Value true_val() {
    return (Value) (uint64) (QNAN | TAG_TRUE);
}

static inline Value bool_val(bool b) {
    return b ? true_val() : false_val();
}

static inline Value nil_val() {
    return (Value) (uint64) (QNAN | TAG_NIL);
}

static inline Value number_val(float64 num) {
    return num_to_value(num);
}

static inline Value obj_val(Obj* obj) {
    return (Value) (SIGN_BIT | QNAN | (uint64) (uintptr) obj);
}

#else

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

#endif
