/**
 * @file error.h
 * @author Filip Knapo
 * @brief Error handling and token-aware syntax errors
 * @version 0.1
 * @date 2025-11-05
 *
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"  // for Token and TokenType

typedef enum
{
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    SEM_UNDEF = 3,
    SEM_REDEF = 4,
    SEM_ARG = 5,
    SEM_TYPE = 6,
    SEM_OTHER = 10,
    INTERNAL_ERROR = 99,
    RUN_TYPE_ARG = 25,
    RUN_TYPE_COMPAT = 26
} ErrorCode;

// Token type to string
const char *tokenTypeToStr(TokenType type);

// Exit with error code and optional token info
void errorExit(ErrorCode code, const char *msg, long line, Token *token);

// Print syntax error like "expected xxx but got yyy"
void syntaxError(Token *got, TokenType expected);

#endif
