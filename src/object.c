#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) \
    (type*) allocate_object(sizeof(type), object_type)

static Obj* allocate_object(usize size, ObjType type) {
    Obj* object = (Obj*) reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

static ObjString* allocate_string(char* chars, int32 length) {
    ObjString* string = ALLOCATE_OBJ(ObjString, ObjectString);
    string->length = length;
    string->chars = chars;
    return string;
}

ObjString* take_string(char* chars, int32 length) {
    return allocate_string(chars, length);
}

ObjString* copy_string(const char* chars, int32 length) {
    char* heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length);
}

void print_object(Value value) {
    switch (obj_type(value)) {
        case ObjectString: {
            printf("%s", as_cstring(value));
            break;
        }
    }
}
