
/**
 * @file parser.h
 * @author Filip Knapo (xknapof00)
 * @brief
 * @version 0.1
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "parser.h"
#include "semantic.h"

#define MAX_TYPE_STRING_LEN 100
/**
 * Duplicate a string (like strdup)
 * Returns a newly allocated copy of str, or NULL if allocation fails
 */
char* myStrdup(const char* str)
{
    if (!str) return NULL;

    size_t len = strlen(str) + 1;  // include null terminator
    char* copy = malloc(len);
    if (!copy) return NULL;

    memcpy(copy, str, len);
    return copy;
}

void printEscapedString(const char* str)
{
    if (!str) return;

    while (*str)
    {
        switch (*str)
        {
            case '\n':
                putchar('\\');
                putchar('n');
                break;
            case '\t':
                putchar('\\');
                putchar('t');
                break;
            case '\r':
                putchar('\\');
                putchar('r');
                break;
            case '\\':
                putchar('\\');
                putchar('\\');
                break;
            default:
                putchar(*str);
        }
        str++;
    }
}

/**
 * @brief Převede bitovou masku ExprType na popisný řetězec.
 * * @param type Bitová maska typů (např. TYPE_INT | TYPE_STRING).
 * @return const char* Staticky alokovaný řetězec.
 */
const char* exprTypeToStr(ExprType type)
{
    // Používáme statický buffer, protože tato funkce by neměla alokovat paměť
    // a caller by ji neměl uvolňovat.
    static char buffer[MAX_TYPE_STRING_LEN];
    buffer[0] = '\0';  // Inicializace prázdného řetězce
    bool first = true;

    // 1. Zpracování speciálního případu UNKNOWN
    if (type == TYPE_UNKNOWN)
    {
        return "UNKNOWN";
    }

    // 2. Procházení všech možných typů a kontrola bitové masky

    // Zpracování UNKNOWN, pokud je součástí širší masky (např. UNKNOWN | INT)
    if (type & TYPE_UNKNOWN)
    {
        strcat(buffer, "UNKNOWN");
        first = false;
    }

    // TYPE_INT
    if (type & TYPE_INT)
    {
        if (!first) strcat(buffer, " | ");
        strcat(buffer, "INT");
        first = false;
    }

    // TYPE_FLOAT
    if (type & TYPE_FLOAT)
    {
        if (!first) strcat(buffer, " | ");
        strcat(buffer, "FLOAT");
        first = false;
    }

    // TYPE_STRING
    if (type & TYPE_STRING)
    {
        if (!first) strcat(buffer, " | ");
        strcat(buffer, "STRING");
        first = false;
    }

    // TYPE_BOOL
    if (type & TYPE_BOOL)
    {
        if (!first) strcat(buffer, " | ");
        strcat(buffer, "BOOL");
        first = false;
    }

    // TYPE_NULL
    if (type & TYPE_NULL)
    {
        if (!first) strcat(buffer, " | ");
        strcat(buffer, "NULL");
        first = false;
    }

    // 3. Kontrola, zda se nenašel žádný platný typ
    if (buffer[0] == '\0')
    {
        return "N/A (Empty Mask)";
    }

    return buffer;
}

const char* tokenTypeStr(TokenType type)
{
    switch (type)
    {
        // --- KEYWORDS ---
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

        // --- SYMBOLS ---
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
            return "EOF";

        // --- OPERATORS ---
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

        // --- LITERALS / IDENTIFIERS ---
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

        case LOGICAL_AND:
            return "LOGICAL_AND";

        case LOGICAL_OR:
            return "LOGICAL_OR";
        case LOGICAL_NOT:
            return "LOGICAL_NOT";

        // --- ERROR ---
        case TOKEN_ERROR:
            return "TOKEN_ERROR";

        default:
            return "UNKNOWN";
    }
}

void printTokenValue(Token* token)
{
    if (!token->value)
    {
        printf("NULL");
        return;
    }

    switch (token->type)
    {
        case INT_LITERAL:
            printf("%lld", token->value->intVal);
            break;
        case FLOAT_LITERAL:
            printf("%f", token->value->floatVal);
            break;
        case IDENTIFIER:
        case GLOBAL_IDENTIFIER:
        case STRING_LITERAL:
        case MULTILINE_STRING_LITERAL:
            printEscapedString(token->value->stringVal);
            break;
        case KW_VAL_NULL:
        case KW_TYPE_NULL:
            printf("null");
            break;
        case EOF_TOKEN:
            printf("EOF");
            break;
        default:
            printf("N/A");  // For keywords, operators, symbols
            break;
    }
}

void printToken(Token* token)
{
    printf("Token: ");
    printTokenValue(token);
    printf("\tType: %s\tLine: %d\n", tokenTypeStr(token->type), token->line);
}

/* ----------------------------------------------------- */
/* This part is only for testing (printing)              */
/* ----------------------------------------------------- */

/* Optional: human-readable names for AST nodes and tokens */
const char* astNodeTypeName(AstNodeType type)
{
    switch (type)
    {
        case AST_CLASS_DEC:
            return "CLASS_DEC";
        case AST_EXPRESSION:
            return "EXPRESSION";
        case AST_VAR_DEC:
            return "VAR_DEC";
        case AST_VAR_ASSIGN:
            return "VAR_ASSIGN";
        case AST_RETURN:
            return "RETURN";
        case AST_LITERAL:
            return "LITERAL";
        case AST_IDENTIFIER:
            return "IDENTIFIER";
        case AST_FUN_DEC:
            return "FUN_DEC";
        case AST_FUN_SET:
            return "FUN_SET";
        case AST_FUN_GET:
            return "FUN_GET";
        case AST_FUN_CALL:
            return "FUN_CALL";
        case AST_IF_STMT:
            return "IF_STMT";
        case AST_IF_ELSE_STMT:
            return "IF_ELSE_STMT";
        case AST_ELSE_STMT:
            return "ELSE_STMT";
        case AST_IFJ:
            return "IFJ";
        case AST_BLOCK:
            return "BLOCK";
        case AST_PARAMS:
            return "PARAMS";
        case AST_OPERATOR:
            return "OPERATOR";
        case AST_WHILE:
            return "WHILE";
        default:
            return "UNKNOWN";
    }
}

const char* tokenTypeName(TokenType type)
{
    switch (type)
    {
        case INT_LITERAL:
            return "";
        case FLOAT_LITERAL:
            return "";
        case STRING_LITERAL:
            return "";
        case MULTILINE_STRING_LITERAL:
            return "";
        case IDENTIFIER:
            return "";
        case GLOBAL_IDENTIFIER:
            return "";
        case EOF_TOKEN:
            return "EOF";
        case DIVIDE:
            return "'/'";
        case MULTIPLY:
            return "'*'";
        case PLUS:
            return "'+'";
        case MINUS:
            return "'-'";
        case IS_SMALLER:
            return "'<'";
        case IS_BIGGER:
            return "'>'";
        case IS_SMALLER_OR_EQUAL:
            return "'<='";
        case IS_BIGGER_OR_EQUAL:
            return "'>='";
        case IS_EQUAL:
            return "'=='";
        case IS_NOT_EQUAL:
            return "'!='";
        case LOGICAL_AND:
            return "'&&'";
        case LOGICAL_OR:
            return "'||'";
        case LOGICAL_NOT:
            return "'!'";
        case ASSIGN:
            return "'='";
        default:
            return "OTHER";
    }
}
void printASTTree(AstNode* node, int level, int isLast, int* prefix)
{
    if (!node) return;

    // Print the vertical branch lines
    for (int i = 1; i < level; i++) printf("%s", prefix[i - 1] ? "│  " : "   ");

    // Print branch connector
    if (level > 0) printf("%s─ ", isLast ? "└" : "├");

    // Print node info
    printf("[%s ", astNodeTypeName(node->type));
    if (node->token.type != NONE)
    {
        printf("%s ", tokenTypeName(node->token.type));
        printTokenValue(&node->token);
    }

    printf(" | %s", exprTypeToStr(node->expressionType));

    printf("]\n");

    // Collect children
    AstNode** children = NULL;
    int childCount = 0;
    if (node->children)
    {
        switch (node->type)
        {
            case AST_OPERATOR:
            case AST_VAR_ASSIGN:
            case AST_IF_STMT:
            case AST_FUN_DEC:
            case AST_EXPRESSION:
            case AST_RETURN:
            case AST_FUN_SET:
            case AST_FUN_GET:
            case AST_FUN_CALL:
            case AST_VAR_DEC:
            case AST_ELSE_STMT:
            case AST_IF_ELSE_STMT:
            case AST_IFJ:
            case AST_WHILE:
            {
                AstBin* bin = (AstBin*)node->children;
                if (bin->left) childCount++;
                if (bin->right) childCount++;
                if (childCount > 0)
                {
                    children = malloc(childCount * sizeof(AstNode*));
                    int idx = 0;
                    if (bin->left) children[idx++] = bin->left;
                    if (bin->right) children[idx++] = bin->right;
                }
                break;
            }

            case AST_CLASS_DEC:
            case AST_BLOCK:
            case AST_PARAMS:
            {
                AstN* nchildren = (AstN*)node->children;
                ListItem* item =
                    nchildren->childrenList ? nchildren->childrenList->firstItem : NULL;

                // Count children
                ListItem* temp = item;
                while (temp)
                {
                    if (temp->item) childCount++;
                    temp = temp->next;
                }

                if (childCount > 0)
                {
                    children = malloc(childCount * sizeof(AstNode*));
                    int idx = 0;
                    while (item)
                    {
                        if (item->item) children[idx++] = item->item;
                        item = item->next;
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    // Recurse into children
    for (int i = 0; i < childCount; i++)
    {
        prefix[level] = (i < childCount - 1);
        printASTTree(children[i], level + 1, i == childCount - 1, prefix);
    }

    if (children) free(children);
}

/* ------------------------------------------ */
/* Helper print functions for codegen         */
/* ------------------------------------------ */

void emit(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void emitIndent(int indent)
{
    for (int i = 0; i < indent; i++) printf("    ");  // 4 mezery
}

void emitLine(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

#include "symtable.h"

/**
 * @brief Print a single symbol in human-readable form
 */
static void printSymbol(Symbol* sym)
{
    if (!sym || sym == TOMBSTONE)
    {
        printf("    <tombstone>\n");
        return;
    }

    printf("    Symbol:\n");
    printf("      name: %s\n", sym->name);
    printf("      kind: %d\n", sym->kind);
    printf("      type: %d\n", sym->expressionType);
    printf("      numOfParams: %d\n", sym->numOfParams);
}

/**
 * @brief Print entire scope (hash table)
 */
void printScope(Scope* scope)
{
    if (!scope)
    {
        printf("  <null scope>\n");
        return;
    }

    printf("  Scope (capacity=%d, used=%d)\n", scope->size, scope->currentSize);

    for (int i = 0; i < scope->size; i++)
    {
        Symbol* s = scope->symbols[i];
        if (s && s != TOMBSTONE)
        {
            printf("  [idx %d]:\n", i);
            printSymbol(s);
        }
    }
}

/**
 * @brief Print entire symbol table stack (all scopes)
 */
void printSymTableStack(SymTableStack* stack)
{
    if (!stack)
    {
        printf("<NULL stack>\n");
        return;
    }

    printf("======= Symbol Table Stack =======\n");
    printf("Stack size: %d\n", stack->top + 1);
    printf("Capacity: %d\n\n", stack->capacity);

    for (int i = stack->top; i >= 0; i--)
    {
        printf("-----------------------------------\n");
        printf("Scope #%d:\n", i);
        printScope(stack->scopes[i]);
    }

    printf("===================================\n");
}

#include "symtable.h"

void printSymTableGrid(SymTableStack* stack)
{
    if (!stack)
    {
        printf("<NULL stack>\n");
        return;
    }

    if (stack->top < 0)
    {
        printf("<Empty symtable stack>\n");
        return;
    }

    // Determine widest scope (all scopes should have same size, but just in case)
    int cols = stack->scopes[0]->size;

    printf("\n======= SYMBOL TABLE GRID VIEW =======\n\n");

    // Print header (column indices)
    printf("         ");
    for (int c = 0; c < cols; c++)
    {
        printf("[%2d] ", c);
    }
    printf("\n");

    // Print each scope as one row
    for (int i = 0; i <= stack->top; i++)
    {
        Scope* sc = stack->scopes[i];
        printf("Scope %d: ", i);

        for (int c = 0; c < sc->size; c++)
        {
            Symbol* s = sc->symbols[c];

            if (!s || s == TOMBSTONE)
            {
                printf("  .   ");  // empty slot
            }
            else
            {
                // print name only
                printf("%5s ", s->name);
            }
        }

        printf("\n");
    }

    printf("\n======================================\n");
}
