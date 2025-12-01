#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
    init_vm();

    Chunk chunk;
    init_chunk(&chunk);

    int32 constant = add_constant(&chunk, 1.2);
    write_chunk(&chunk, OpConstant, 1);
    write_chunk(&chunk, constant, 1);
    write_chunk(&chunk, OpReturn, 1);

    disassemble_chunk(&chunk, "test chunk");
    interpret(&chunk);

    free_vm();
    free_chunk(&chunk);

    return 0;
}
