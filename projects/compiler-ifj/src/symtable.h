/**
 * @file symtable.h
 * @author Ondrej Prazak (xprazao00)
 * @brief symtable for IFJcode25
 *
 * @copyrigscope Copyrigscope (c) 2025
 *
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE ((Symbol *)1)
#define INITIAL_CAPACITY_STACK 32
#define INITIAL_CAPACITY_SCOPE 64
typedef enum
{
    SYM_VAR,
    SYM_PARAM,
    SYM_FUNC,
    SYM_GET,
    SYM_SET,
    SYM_CLASS,
    SYM_CONST,
} SymbolKind;

typedef enum
{
    TYPE_UNKNOWN = 1 << 0,  // 000001 // 1
    TYPE_INT = 1 << 1,      // 000010 // 2
    TYPE_STRING = 1 << 2,   // 000100 // 4
    TYPE_FLOAT = 1 << 3,    // 001000 // 8
    TYPE_NULL = 1 << 4,     // 010000 // 16
    TYPE_BOOL = 1 << 5      // 100000 // 32
} ExprType;

typedef struct
{
    char *name;
    SymbolKind kind;
    ExprType expressionType;
    ExprType *paramTypes;  // ADD THIS
    bool declared;
    int numOfParams;
} Symbol;

/**
 * @brief One table/list of symbols, scope u can say
 *
 */
typedef struct Scope
{
    Symbol **symbols;
    int size;
    int currentSize;
} Scope;

/**
 * @brief Main Structure holding all tables(lists)
 *
 */
typedef struct SymTableStack
{
    Scope **scopes;
    int capacity;
    int top;
} SymTableStack;

// TODO funkce ktere se nebudou pouzivat mimo symtable.c tady byt nemusi (ale muzou)
// TODO zkontrolovat implementaci funkci zda funguji
// TODO symtablestackPOP/PUSH by mel ridit logiku scopes, myslim ze by to usetrilo manipulaci v
// prseru a schovalo pro nej nepotrebnou logiku kdyby neco napis Ksnapeek

/* --------------------------------------------------------------- */
/* SYMTABLESTACK FUNCTIONS                                         */
/* --------------------------------------------------------------- */

/**
 * @brief Create Symtable Stack
 * @param initial_capacity set table count capacity
 *
 */
SymTableStack *symTableStackCreate(int initial_capacity);

/**
 *  @brief Push Scope/symtable on top of symtableStack
 *  @param stack stack to insert to
 *
 */
void symTableStackPush(SymTableStack *stack);

/**
 * @brief Deletes top scope on stack
 * @param stack
 */
void symTableStackPop(SymTableStack *stack);

/**
 * @brief returns Scope at specified index(scope depth)
 * @param stack
 * @param index
 */
Scope *symTableStackGetScope(SymTableStack *stack, int index);

/**
 * @brief  Searches Symbol in all scopes that are in stack
 *
 * @param stack - SymTableStack struct
 * @param name - name of symbol to find
 * @return Symbol* found symbol, null if not in table
 */
Symbol *symTableStackFindSymbol(SymTableStack *stack, char *name);

/**
 * @brief Unallocate SymTableStack structure
 * @param stack
 */
void freeSymTableStack(SymTableStack *stack);

/* --------------------------------------------------------------- */
/* SCOPE FUNCTIONS                                                 */
/* --------------------------------------------------------------- */

/**
 * @brief Creates (Scope *) of size
 *
 * @param size size of table
 * @return Scope*
 */
Scope *scopeCreate(int size);

/**
 * @brief Doubles capacity of Scope
 *
 * @param scope Scope
 * @return int
 */
int scopeDoubleCapacity(Scope *scope);

/**
 * @brief Get hash for a given string
 *
 * @param str receive hash for this string
 * @param size Specifies modulo of hash
 * @return int
 */
int getHash(char *str, int size);

/**
 * @brief Insert/Update given symbol
 *
 * @param scope Scope
 * @param sym symbol to Insert/Update
 * @return int
 */
int scopeAddSymbol(Scope *scope, Symbol *sym);

/**
 * @brief
 * @param scope search this table
 * @param name find item with this name
 */
Symbol *scopeFindSymbol(Scope *scope, char *name);

/**
 * @brief Delete and Unallocate given Scope
 *
 * @param scope Scope
 * @return int
 */
int freeScope(Scope *scope);

/* --------------------------------------------------------------- */
/* SYMBOL FUNCTIONS                                                */
/* --------------------------------------------------------------- */

/**
 * @brief Creates a new Symbol
 *
 * @param name Symbol->name
 * @param type Symbol->type
 * @param kind Symbol->kind
 * @param scope Symbol->scope
 * @param lineOfCode Symbol->lineOfCode
 * @param numOfParams Symbol->numOfParams
 * @return Symbol*
 */
Symbol *symbolCreate(char *name, ExprType type, SymbolKind kind, int numOfParams);

/**
 * @brief Update Symbol values
 *
 * @param dest input new values here
 * @param src get new values from here
 */
void symbolUpdate(Symbol *dest, Symbol *src);

/**
 * @brief //TODO DOPSAT POPIS
 *
 * @param name
 * @param scopeID
 * @return char*
 */
char *symbolGetUniqueName(const char *name, SymbolKind kind, int paramCount);
/**
 * @brief Delete and Unallocate given symbol
 *
 * @param symbol Delete this symbol
 */
void freeSymbol(Symbol *symbol);
#endif  // SYMTABLE_H