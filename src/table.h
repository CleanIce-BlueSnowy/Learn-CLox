#pragma once

#include "common.h"
#include "value.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct {
    int32 count;
    int32 capacity;
    Entry* entries;
} Table;

void init_table(Table* table);
void free_table(Table* table);
bool table_get(Table* table, ObjString* key, Value* value);
bool table_set(Table* table, ObjString* key, Value value);
void table_add_all(Table* from, Table* to);
