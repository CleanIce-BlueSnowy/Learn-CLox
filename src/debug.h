#pragma once

#include "chunk.h"

void disassemble_chunk(Chunk* chunk, const char* name);
int32 disassemble_instruction(Chunk* chunk, int32 offset);
