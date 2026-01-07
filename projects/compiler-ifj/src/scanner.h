/**
 * @file scanner.h
 * @author Samuel Vajda (xvajdas00)
 * @brief Scanner (lexical analyzer) for IFJ25 compiler
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include <stdio.h>

typedef enum
{
    // --- KEYWORDS ---
    KW_CLASS,
    KW_IF,
    KW_ELSE,
    KW_IS,
    KW_VAL_NULL,
    KW_RETURN,
    KW_VAR,
    KW_WHILE,
    KW_IFJ,
    KW_STATIC,
    KW_IMPORT,
    KW_FOR,
    KW_TYPE_NUM,
    KW_TYPE_STRING,
    KW_TYPE_NULL,
    KW_TYPE_BOOL,
    KW_VAL_TRUE,
    KW_VAL_FALSE,

    // --- SYMBOLS ---
    LPAR,
    RPAR,
    LCURLY,
    RCURLY,
    COMMA,
    ASSIGN,
    DOT,
    EOL,
    EOF_TOKEN,
    TERNARY_QUESTION,
    TERNARY_COLON,

    // --- OPERATORS ---
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    IS_SMALLER,
    IS_BIGGER,
    IS_SMALLER_OR_EQUAL,
    IS_BIGGER_OR_EQUAL,
    IS_EQUAL,
    IS_NOT_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,

    // --- LITERALS / IDENTIFIERS ---
    IDENTIFIER,
    GLOBAL_IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    MULTILINE_STRING_LITERAL,

    // --- ERROR ---
    TOKEN_ERROR,
    NONE

} TokenType;

typedef struct
{
    char *stringVal;
    long long intVal;
    double floatVal;
} TokenValue;

typedef struct
{
    TokenType type;
    TokenValue *value;
    int line;  // To know where we ended, when error occurs (copied from Scanner during analysis)
} Token;

typedef struct
{
    FILE *source;       // Input stream
    int line;           // Current line number (tracked during scanning)
    bool prologueRead;  // Flag if prologue was read
    Token lookAheadToken;
    bool isLookingAhead;
    bool lastWasEOL;
} Scanner;

int getNextToken(Scanner *scanner, Token *token);
TokenType identifyKeyword(const char *str);
int readPrologue(Scanner *scanner);
void initScanner(Scanner *scanner, FILE *source);
void disposeScanner(Scanner *scanner);
int lookAhead(Scanner *scanner, Token *token);
void freeToken(Token *token);

#endif  // SCANNER_H