#pragma once

#include "common.h"

typedef enum {
    TokenLeftParen, TokenRightParen,
    TokenLeftBrace, TokenRightBrace,
    TokenComma, TokenDot, TokenMinus, TokenPlus,
    TokenSemicolon, TokenSlash, TokenStar,

    TokenBang, TokenBangEqual,
    TokenEqual, TokenEqualEqual,
    TokenGreater, TokenGreaterEqual,
    TokenLess, TokenLessEqual,

    TokenIdentifier, TokenString, TokenNumber,

    TokenAnd, TokenClass, TokenElse, TokenFalse,
    TokenFor, TokenFun, TokenIf, TokenNil,
    TokenOr, TokenPrint, TokenReturn, TokenSuper,
    TokenThis, TokenTrue, TokenVar, TokenWhile,

    TokenError, TokenEOF,
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int32 length;
    int32 line;
} Token;

void init_scanner(const char* source);
Token scan_token();
