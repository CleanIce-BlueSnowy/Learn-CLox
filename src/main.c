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

    constant = add_constant(&chunk, 3.4);
    write_chunk(&chunk, OpConstant, 1);
    write_chunk(&chunk, constant, 1);

    write_chunk(&chunk, OpAdd, 1);

    constant = add_constant(&chunk, 5.6);
    write_chunk(&chunk, OpConstant, 2);
    write_chunk(&chunk, constant, 2);

    write_chunk(&chunk, OpDivide, 2);
    write_chunk(&chunk, OpNegate, 3);

    write_chunk(&chunk, OpReturn, 2);

    disassemble_chunk(&chunk, "test chunk");
    interpret(&chunk);

    free_vm();
    free_chunk(&chunk);

    return 0;
}
