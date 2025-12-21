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
    #ifdef NAN_BOXING

    if (is_bool(value)) {
        printf(as_bool(value) ? "true" : "false");
    } else if (is_nil(value)) {
        printf("nil");
    } else if (is_number(value)) {
        printf("%g", as_number(value));
    } else if (is_obj(value)) {
        print_object(value);
    }

    #else

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

    #endif
}

bool values_equal(Value a, Value b) {
    #ifdef NAN_BOXING

    if (is_number(a) && is_number(b)) {
        return as_number(a) == as_number(b);
    }
    return a == b;

    #else

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
            return as_obj(a) == as_obj(b);
        }
    }
    return false;  // unreachable

    #endif
}
