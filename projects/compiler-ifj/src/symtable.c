/**
 * @file symtable.c
 * @author Ondrej Prazak (xprazao00)
 * @brief symtable for IFJcode25
 *
 * @copyrigscope Copyrigscope (c) 2025
 *
 */

#include "symtable.h"

#include "utils.h"

/* --------------------------------------------------------------- */
/* SYMTABLESTACK FUNCTIONS                                         */
/* --------------------------------------------------------------- */

SymTableStack *symTableStackCreate(int initial_capacity)
{
    SymTableStack *stack = malloc(sizeof(SymTableStack));
    if (!stack)
    {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    stack->scopes = malloc(sizeof(Scope *) * initial_capacity);
    if (!stack->scopes)
    {
        free(stack);
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    stack->capacity = initial_capacity;
    stack->top = -1;
    return stack;
}

void symTableStackPush(SymTableStack *stack)
{
    Scope *newScope = scopeCreate(16);
    if (stack->top + 1 >= stack->capacity)
    {
        int newCapacity = stack->capacity * 2;
        Scope **newTables = realloc(stack->scopes, sizeof(Scope *) * newCapacity);
        if (!newTables)
        {
            fprintf(stderr, "Allocation error\n");
            exit(1);
        }
        stack->scopes = newTables;
        stack->capacity = newCapacity;
    }
    stack->scopes[++stack->top] = newScope;
}

void symTableStackPop(SymTableStack *stack)
{
    if (stack->top < 0)
    {
        return;
    }
    freeScope(stack->scopes[stack->top--]);
}

Scope *symTableStackGetScope(SymTableStack *stack, int index)
{
    if (index < 0 || index > stack->top)
    {
        return NULL;
    }
    return stack->scopes[index];
}

Symbol *symTableStackFindSymbol(SymTableStack *stack, char *name)
{
    for (int i = stack->top; i > -1; i--)
    {
        Symbol *sym = scopeFindSymbol(stack->scopes[i], name);
        if (sym != NULL)
        {
            return sym;
        }
    }
    return NULL;
}

void freeSymTableStack(SymTableStack *stack)
{
    if (!stack) return;

    for (int i = 0; i <= stack->top; i++)
    {
        if (stack->scopes[i] != NULL)
        {
            freeScope(stack->scopes[i]);
        }
    }

    free(stack->scopes);
    free(stack);
}

/* --------------------------------------------------------------- */
/* SCOPE FUNCTIONS                                                 */
/* --------------------------------------------------------------- */

Scope *scopeCreate(int size)
{
    Scope *scope = malloc(sizeof(Scope));
    if (!scope)
    {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    scope->size = size;
    scope->currentSize = 0;
    scope->symbols = calloc(size, sizeof(Symbol *));
    if (!scope->symbols)
    {
        fprintf(stderr, "Allocation error\n");
        free(scope);
        exit(1);
    }

    return scope;
}

int scopeDoubleCapacity(Scope *scope)
{
    int newSize = scope->size * 2;

    Symbol **newTable = calloc(newSize, sizeof(Symbol *));
    if (!newTable)
    {
        fprintf(stderr, "Allocation error\n");
        free(scope->symbols);
        exit(1);
    }

    for (int i = 0; i < scope->size; i++)
    {
        Symbol *sym = scope->symbols[i];
        if (sym != NULL && sym != TOMBSTONE)  // rehash valid symbols
        {
            unsigned int index = getHash(sym->name, newSize);

            while (newTable[index] != NULL)
            {
                index = (index + 1) % newSize;
            }
            newTable[index] = sym;
        }
    }

    free(scope->symbols);

    scope->symbols = newTable;
    scope->size = newSize;

    return 0;
}

int getHash(char *str, int size)
{
    unsigned long hash = 5381;
    char c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return (unsigned int)(hash % size);
}

int scopeAddSymbol(Scope *scope, Symbol *sym)
{
    if (!sym || !sym->name)
    {
        return -1;
    }

    if (scope->currentSize + 1 > scope->size * 0.7)
    {
        if (scopeDoubleCapacity(scope) != 0)
        {
            return -1;
        }
    }

    unsigned int index = getHash(sym->name, scope->size);

    for (int i = 0; i < scope->size; i++)
    {
        unsigned int try = (index + i) % scope->size;

        if (scope->symbols[try] == NULL || scope->symbols[try] == TOMBSTONE)  // insert
        {
            scope->symbols[try] = sym;
            scope->currentSize++;
            return 0;
        }

        if (strcmp(scope->symbols[try]->name, sym->name) == 0)  // update
        {
            symbolUpdate(scope->symbols[try], sym);
            return 0;
        }
    }

    return -1;
}

Symbol *scopeFindSymbol(Scope *scope, char *name)
{
    if (scope == NULL || name == NULL)
    {
        return NULL;
    }

    unsigned int index = getHash(name, scope->size);

    for (int i = 0; i < scope->size; i++)
    {
        unsigned int try = (index + i) % scope->size;

        if (scope->symbols[try] == NULL)
        {
            return NULL;
        }

        if (scope->symbols[try] == TOMBSTONE)
        {
            // Může být dále v tabulce
            continue;
        }

        if (strcmp(scope->symbols[try]->name, name) == 0)
        {
            return scope->symbols[try];
        }
    }

    // symbol nenalezen
    return NULL;
}

int freeScope(Scope *scope)
{
    if (scope == NULL)
    {
        return -1;
    }

    for (int i = 0; i < scope->size; i++)
    {
        if (scope->symbols[i] != NULL && scope->symbols[i] != TOMBSTONE)  // free valid pointers
        {
            freeSymbol(scope->symbols[i]);
        }
    }

    free(scope->symbols);
    free(scope);

    return 0;
}

/* --------------------------------------------------------------- */
/* SYMBOL FUNCTIONS                                                */
/* --------------------------------------------------------------- */

Symbol *symbolCreate(char *name, ExprType type, SymbolKind kind, int numOfParams)
{
    Symbol *new = malloc(sizeof(Symbol));
    if (!new)
    {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    // Deep copy basic name
    size_t len = strlen(name);
    new->name = malloc(len + 1);
    if (!new->name)
    {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }
    memcpy(new->name, name, len + 1);

    if (kind == SYM_FUNC || kind == SYM_GET || kind == SYM_SET)
    {
        // Create unique overload name
        char *unique = symbolGetUniqueName(name, kind, numOfParams);
        if (!unique)
        {
            fprintf(stderr, "Allocation error\n");
            exit(1);
        }

        free(new->name);
        new->name = unique;
    }

    new->expressionType = type;
    new->kind = kind;
    new->declared = false;
    new->numOfParams = numOfParams;

    if (numOfParams > 0)
    {
        new->paramTypes = malloc(sizeof(ExprType) * numOfParams);
        for (int i = 0; i < numOfParams; i++) new->paramTypes[i] = TYPE_UNKNOWN;
    }
    else
    {
        new->paramTypes = NULL;
    }

    return new;
}

void symbolUpdate(Symbol *dest, Symbol *source)
{
    if (dest != NULL && source != NULL)
    {
        dest->kind = source->kind;
        dest->numOfParams = source->numOfParams;
        dest->expressionType = source->expressionType;
    }
}

char *symbolGetUniqueName(const char *name, SymbolKind kind, int paramCount)
{
    // Determine suffix length:
    // - getter: "#get"
    // - setter: "#set"
    // - normal function: "$<digits>"
    const char *suffix = NULL;
    char numericSuffix[16];  // enough for "$" + up to 10 digits + '\0'

    if (kind == SYM_GET)
    {
        suffix = "#get";
    }
    else if (kind == SYM_SET)
    {
        suffix = "#set";
    }
    else
    {
        // normal function → append "$<paramCount>"
        snprintf(numericSuffix, sizeof(numericSuffix), "$%d", paramCount);
        suffix = numericSuffix;
    }

    // Allocate memory
    size_t len = strlen(name) + strlen(suffix) + 1;  // +1 for '\0'
    char *uniqueName = malloc(len);
    if (!uniqueName)
    {
        fprintf(stderr, "Allocation failed\n");
        return NULL;
    }

    // Build final name
    strcpy(uniqueName, name);
    strcat(uniqueName, suffix);

    return uniqueName;
}

void freeSymbol(Symbol *symbol)
{
    if (symbol != NULL)
    {
        free(symbol->name);
        free(symbol);
    }
}
