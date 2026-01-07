/**
 * @file error.c
 * @brief Implementation of error handling functions
 */

#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *tokenTypeToStr(TokenType type)
{
    switch (type)
    {
        case KW_CLASS:
            return "KW_CLASS";
        case KW_IF:
            return "KW_IF";
        case KW_ELSE:
            return "KW_ELSE";
        case KW_IS:
            return "KW_IS";
        case KW_VAL_NULL:
            return "KW_VAL_NULL";
        case KW_RETURN:
            return "KW_RETURN";
        case KW_VAR:
            return "KW_VAR";
        case KW_WHILE:
            return "KW_WHILE";
        case KW_IFJ:
            return "KW_IFJ";
        case KW_STATIC:
            return "KW_STATIC";
        case KW_IMPORT:
            return "KW_IMPORT";
        case KW_FOR:
            return "KW_FOR";
        case KW_TYPE_NUM:
            return "KW_TYPE_NUM";
        case KW_TYPE_STRING:
            return "KW_TYPE_STRING";
        case KW_TYPE_NULL:
            return "KW_TYPE_NULL";
        case KW_TYPE_BOOL:
            return "KW_TYPE_BOOL";
        case LPAR:
            return "LPAR";
        case RPAR:
            return "RPAR";
        case LCURLY:
            return "LCURLY";
        case RCURLY:
            return "RCURLY";
        case COMMA:
            return "COMMA";
        case ASSIGN:
            return "ASSIGN";
        case DOT:
            return "DOT";
        case EOL:
            return "EOL";
        case EOF_TOKEN:
            return "EOF_TOKEN";
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case MULTIPLY:
            return "MULTIPLY";
        case DIVIDE:
            return "DIVIDE";
        case IS_SMALLER:
            return "IS_SMALLER";
        case IS_BIGGER:
            return "IS_BIGGER";
        case IS_SMALLER_OR_EQUAL:
            return "IS_SMALLER_OR_EQUAL";
        case IS_BIGGER_OR_EQUAL:
            return "IS_BIGGER_OR_EQUAL";
        case IS_EQUAL:
            return "IS_EQUAL";
        case IS_NOT_EQUAL:
            return "IS_NOT_EQUAL";
        case IDENTIFIER:
            return "IDENTIFIER";
        case GLOBAL_IDENTIFIER:
            return "GLOBAL_IDENTIFIER";
        case INT_LITERAL:
            return "INT_LITERAL";
        case FLOAT_LITERAL:
            return "FLOAT_LITERAL";
        case STRING_LITERAL:
            return "STRING_LITERAL";
        case MULTILINE_STRING_LITERAL:
            return "MULTILINE_STRING_LITERAL";
        case TOKEN_ERROR:
            return "TOKEN_ERROR";
        default:
            return "UNKNOWN_TOKEN";
    }
}

void errorExit(ErrorCode code, const char *msg, long line, Token *token)
{
    fprintf(stderr, "Error (code %d) at line %ld: %s\n", code, line, msg);

    if (token)
    {
        const char *valueStr = "NULL";
        static char buffer[64];

        if (token->value)
        {
            switch (token->type)
            {
                case STRING_LITERAL:
                case MULTILINE_STRING_LITERAL:
                case IDENTIFIER:
                case GLOBAL_IDENTIFIER:
                    valueStr = token->value->stringVal ? token->value->stringVal : "NULL";
                    break;
                case INT_LITERAL:
                    snprintf(buffer, sizeof(buffer), "%lld", token->value->intVal);
                    valueStr = buffer;
                    break;
                case FLOAT_LITERAL:
                    snprintf(buffer, sizeof(buffer), "%f", token->value->floatVal);
                    valueStr = buffer;
                    break;
                default:
                    valueStr = token->value->stringVal ? token->value->stringVal : "N/A";
            }
        }

        fprintf(stderr, "\tToken Type: %s\n", tokenTypeToStr(token->type));
        fprintf(stderr, "\tToken Value: %s\n", valueStr);
    }

    exit(code);
}

void syntaxError(Token *got, TokenType expected)
{
    const char *expectedStr = tokenTypeToStr(expected);
    const char *gotStr = tokenTypeToStr(got->type);
    const char *gotVal = "NULL";
    static char buffer[64];

    if (got->value)
    {
        switch (got->type)
        {
            case STRING_LITERAL:
            case MULTILINE_STRING_LITERAL:
            case IDENTIFIER:
            case GLOBAL_IDENTIFIER:
                gotVal = got->value->stringVal ? got->value->stringVal : "NULL";
                break;
            case INT_LITERAL:
                snprintf(buffer, sizeof(buffer), "%lld", got->value->intVal);
                gotVal = buffer;
                break;
            case FLOAT_LITERAL:
                snprintf(buffer, sizeof(buffer), "%f", got->value->floatVal);
                gotVal = buffer;
                break;
            default:
                gotVal = got->value->stringVal ? got->value->stringVal : "N/A";
        }
    }

    fprintf(stderr, "Syntax error at line %d: expected %s but got %s \"%s\"\n", got->line,
            expectedStr, gotStr, gotVal);

    exit(SYNTAX_ERROR);
}
