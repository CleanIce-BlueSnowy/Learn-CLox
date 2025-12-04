#pragma once

#include "common.h"
#include "object.h"

#define ALLOCATE(type, count) \
    (type*) reallocate(NULL, 0, sizeof(type) * (count))
#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)
#define GROW_ARRAY(type, pointer, old_count, new_count) \
    (type*) reallocate(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count))
#define FREE_ARRAY(type, pointer, old_count) \
    reallocate(pointer, sizeof(type) * (old_count), 0)

void* reallocate(void* pointer, usize old_size, usize new_size);
void free_objects();
