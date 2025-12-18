#include <stdlib.h>

#include "memory.h"
#include "vm.h"

void* reallocate(void* pointer, [[maybe_unused]] usize old_size, usize new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, new_size);
    if (result == NULL) {
        exit(1);
    }
    return result;
}

static void free_object(Obj* object) {
    switch (object->type) {
        case ObjectClosure: {
            ObjClosure* closure = (ObjClosure*) object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalue_count);
            FREE(ObjClosure, object);
            break;
        }
        case ObjectFunction: {
            ObjFunction* function = (ObjFunction*) object;
            free_chunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case ObjectNative: {
            FREE(ObjNative, object);
            break;
        }
        case ObjectString: {
            ObjString* string = (ObjString*) object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
        case ObjectUpvalue: {
            FREE(ObjUpvalue, object);
            break;
        }
    }
}

void free_objects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        free_object(object);
        object = next;
    }
}
