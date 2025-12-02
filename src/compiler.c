#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool had_error;
} Parser;

Parser parser;

static error_at(Token* token, const char* message) {
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TokenEOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TokenError) {
        // empty.
    } else {
        fprintf(stderr, " at `%.*s`", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error(const char* message) {
    error_at(&parser.previous, message);
}

static void error_at_current(const char* message) {
    error_at(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    while (true) {
        parser.current = scan_token();
        if (parser.current.type != TokenError) {
            break;
        }
        error_at_current(parser.current.start);
    }
}

bool compile(const char* source, Chunk* chunk) {
    init_scanner(source);

    advance();
    expression();
    consume(TokenEOF, "Expect end of expression.");

    return !parser.had_error;
}
