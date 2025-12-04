#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "value.h"

void init_value_array(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void write_value_array(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int32 old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void free_value_array(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value) {
    switch (value.type) {
        case ValBool: {
            printf(as_bool(value) ? "true" : "false");
            break;
        }
        case ValNil: {
            printf("nil");
            break;
        }
        case ValNumber: {
            printf("%g", as_number(value));
            break;
        }
        case ValObj: {
            print_object(value);
            break;
        }
    }
}

bool values_equal(Value a, Value b) {
    if (a.type != b.type) {
        return false;
    }
    switch (a.type) {
        case ValBool: {
            return as_bool(a) == as_bool(b);
        }
        case ValNil: {
            return true;
        }
        case ValNumber: {
            return as_number(a) == as_number(b);
        }
        case ValObj: {
            ObjString* a_string = as_string(a);
            ObjString* b_string = as_string(b);
            return a_string->length == b_string->length && memcmp(a_string->chars, b_string->chars, a_string->length) == 0;
        }
    }
}
