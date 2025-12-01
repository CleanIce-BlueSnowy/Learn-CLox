#pragma once

#include "common.h"

typedef float64 Value;

typedef struct {
    int32 capacity;
    int32 count;
    Value* values;
} ValueArray;

void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);
void print_value(Value value);
