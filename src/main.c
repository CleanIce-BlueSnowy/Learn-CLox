#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static void repl() {
    char line[1024];
    while (true) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0, SEEK_END);
    usize file_size = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    usize bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(const char* path) {
    char* source = read_file(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == InterpretCompileError) {
        exit(65);
    } else if (result == InterpretRuntimeError) {
        exit(70);
    }
}

int main(int argc, const char* argv[]) {
    init_vm();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    free_vm();

    return 0;
}
