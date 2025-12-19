#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) \
    (type*) allocate_object(sizeof(type), object_type)

static Obj* allocate_object(usize size, ObjType type) {
    Obj* object = (Obj*) reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;

    #ifdef DEBUG_LOG_GC
    printf("%p allocate %zu for %d\n", (void*) object, size, type);
    #endif

    return object;
}

ObjClosure* new_closure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalue_count);
    for (int32 i = 0; i < function->upvalue_count; i++) {
        upvalues[i] = NULL;
    }
    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, ObjectClosure);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}

ObjFunction* new_function() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, ObjectFunction);
    function->arity = 0;
    function->upvalue_count = 0;
    function->name = NULL;
    init_chunk(&function->chunk);
    return function;
}

ObjNative* new_native(NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, ObjectNative);
    native->function = function;
    return native;
}

static ObjString* allocate_string(char* chars, int32 length, uint32 hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, ObjectString);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    table_set(&vm.strings, string, nil_val());
    return string;
}

static uint32 hash_string(const char* key, int32 length) {
    uint32 hash = 2166136261;
    for (int32 i = 0; i < length; i++) {
        hash ^= (uint8) key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* take_string(char* chars, int32 length) {
    uint32 hash = hash_string(chars, length);

    ObjString* interned = table_find_string(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocate_string(chars, length, hash);
}

ObjString* copy_string(const char* chars, int32 length) {
    uint32 hash = hash_string(chars, length);

    ObjString* interned = table_find_string(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }

    char* heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length, hash);
}

ObjUpvalue* new_upvalue(Value* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, ObjectUpvalue);
    upvalue->closed = nil_val();
    upvalue->location = slot;
    upvalue->next = NULL;
    return upvalue;
}

static void print_function(ObjFunction* function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

void print_object(Value value) {
    switch (obj_type(value)) {
        case ObjectClosure: {
            print_function(as_closure(value)->function);
            break;
        }
        case ObjectFunction: {
            print_function(as_function(value));
            break;
        }
        case ObjectNative: {
            printf("<native fn>");
            break;
        }
        case ObjectString: {
            printf("%s", as_cstring(value));
            break;
        }
        case ObjectUpvalue: {
            printf("upvalue");
            break;
        }
    }
}
