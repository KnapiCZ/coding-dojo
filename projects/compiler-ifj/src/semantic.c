/**
 * @file semantic.c
 * @author ...
 * @brief Semantic analysis for expressions and functions
 * @version 0.1
 * @date 2025-11-14
 */

#include "semantic.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "symtable.h"

/**
 * @brief Used to find exact symbol for function
 *
 * @param stack - symtable stack to search in
 * @param base - name of function
 * @param argc - count of arguments
 * @return Symbol* - NULL if not declared else Symbol*
 */
Symbol* findFunctionExact(SymTableStack* stack, char* base, int argc)
{
    char buff[256];
    snprintf(buff, sizeof(buff), "%s$%d", base, argc);
    return symTableStackFindSymbol(stack, buff);
}

/**
 * @brief  Used to check if there is function with same name but different arity
 *
 * @param stack - symtable stack searching function in
 * @param base - name of function we are looking for
 * @return Symbol* - NULL if not found, else Symbol*
 */
Symbol* findFunctionAnyArity(SymTableStack* stack, char* base)
{
    Scope* global = stack->scopes[0];

    for (int i = 0; i < global->size; i++)
    {
        Symbol* symbol = global->symbols[i];
        if (!symbol || symbol == TOMBSTONE) continue;

        if (symbol->kind == SYM_FUNC || symbol->kind == SYM_GET || symbol->kind == SYM_SET)
        {
            char* dollar = strchr(symbol->name, '$');
            if (!dollar) continue;

            size_t len = dollar - symbol->name;
            if (strncmp(symbol->name, base, len) == 0 && base[len] == '\0') return symbol;
        }
    }

    return NULL;
}

/**
 * @brief Used to find if there is getter with same name
 *
 * @param stack - symtable stack searching function in
 * @param base - name of function we are looking for
 * @return Symbol* - NULL if not found, else Symbol*
 */
Symbol* findGetter(SymTableStack* stack, char* base)
{
    char expected[256];
    snprintf(expected, sizeof(expected), "%s$set", base);
    return scopeFindSymbol(stack->scopes[0], expected);
}

/**
 * @brief Used to find if there is setter with same name
 *
 * @param stack - symtable stack searching function in
 * @param base - name of function we are looking for
 * @return Symbol* - NULL if not found, else Symbol*
 */
Symbol* findSetter(SymTableStack* stack, char* base)
{
    char expected[256];
    snprintf(expected, sizeof(expected), "%s$set", base);
    return scopeFindSymbol(stack->scopes[0], expected);
}

/* ------------------------------------------------------- */
/* OPERATIONS                                              */
/* ------------------------------------------------------- */

/**
 * @brief Checking if right and left operands are compatible in multiplication
 *
 * @param left - left operand to be checked
 * @param right - right operand to be checked
 * @return true - operation is valid
 * @return false - operation is invalid
 */
bool multiplication(AstNode* left, AstNode* right)
{
    if (left->expressionType & TYPE_STRING)
        return (right->expressionType & (TYPE_INT | TYPE_UNKNOWN)) != 0;

    if (left->expressionType & (TYPE_INT | TYPE_FLOAT))
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) != 0;

    if (left->expressionType & TYPE_UNKNOWN)
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) != 0;

    return false;
}

/**
 * @brief Checking if right and left operands are compatible in division and substraction
 *
 * @param left - left operand to be checked
 * @param right - right operand to be checked
 * @return true - operation is valid
 * @return false - operation is invalid
 */
bool divisionAndSubStraction(AstNode* left, AstNode* right)
{
    if (left->expressionType & (TYPE_FLOAT | TYPE_INT))
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) != 0;

    if (left->expressionType & TYPE_UNKNOWN)
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) != 0;

    return false;
}

/**
 * @brief Checking if right and left operands are compatible in addition
 *
 * @param left - left operand to be checked
 * @param right - right operand to be checked
 * @return true - operation is valid
 * @return false - operation is invalid
 */
bool addition(AstNode* left, AstNode* right)
{
    if (left->expressionType & TYPE_STRING)
        return (right->expressionType & (TYPE_STRING | TYPE_UNKNOWN)) != 0;

    if (left->expressionType & (TYPE_INT | TYPE_FLOAT))
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) != 0;

    if (left->expressionType & TYPE_UNKNOWN)
        return (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_STRING | TYPE_UNKNOWN)) != 0;

    return false;
}

/* --------------------------------------------------------- */
/*  Expression control                                       */
/* --------------------------------------------------------- */

/**
 * @brief Used to decide what operation to check
 *
 * @param left - left operand to be checked
 * @param op - operator used to decide which operation to check
 * @param right - right operand to be checked
 * @return true - operation is valid
 * @return false - operation is invalid
 */
bool checkBinaryTypes(AstNode* left, AstNode* op, AstNode* right)
{
    switch (op->token.type)
    {
        case MULTIPLY:
            return multiplication(left, right);

        case DIVIDE:
        case MINUS:
            return divisionAndSubStraction(left, right);

        case PLUS:
            return addition(left, right);

        case IS_EQUAL:
        case IS_NOT_EQUAL:
            return true;

        case IS_BIGGER:
        case IS_BIGGER_OR_EQUAL:
        case IS_SMALLER:
        case IS_SMALLER_OR_EQUAL:
            return (left->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN)) &&
                   (right->expressionType & (TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN));

        case LOGICAL_AND:
        case LOGICAL_OR:
            return (left->expressionType & TYPE_BOOL) && (right->expressionType & TYPE_BOOL);

        case KW_IS:
            return true;

        default:
            return false;
    }
}

/**
 * @brief Get the Binary Result Type of operation
 *
 * @param left - left operand to be checked
 * @param op - operator used to decide which operation to check
 * @param right - right operand to be checked
 * @return ExprType - return type of expression
 */
ExprType getBinaryResultType(AstNode* left, AstNode* op, AstNode* right)
{
    switch (op->token.type)
    {
        case MULTIPLY:
        case DIVIDE:
        case PLUS:
        case MINUS:
            if (left->expressionType & TYPE_FLOAT && right->expressionType & TYPE_FLOAT)
                return TYPE_FLOAT;
            if (left->expressionType & TYPE_INT && right->expressionType & TYPE_INT)
                return TYPE_INT;
            if (left->expressionType & TYPE_STRING && right->expressionType & TYPE_INT)
                return TYPE_STRING;
            if (left->expressionType & TYPE_NULL &&
                right->expressionType & (TYPE_NULL | TYPE_FLOAT | TYPE_INT))
                return TYPE_STRING;

            return TYPE_UNKNOWN;

        case IS_EQUAL:
        case IS_NOT_EQUAL:
        case IS_BIGGER:
        case IS_BIGGER_OR_EQUAL:
        case IS_SMALLER:
        case IS_SMALLER_OR_EQUAL:
        case LOGICAL_AND:
        case LOGICAL_OR:
        case KW_IS:
            return TYPE_BOOL;

        default:
            return TYPE_UNKNOWN;
    }
}

/* --------------------------------------------------------- */
/*  ZÁKLADNÍ SEMANTIKA VÝRAZU                                */
/* --------------------------------------------------------- */

void semanticExpression(AstNode* node)
{
    if (!node) return;

    switch (node->type)
    {
        case AST_LITERAL:
        case AST_IDENTIFIER:
            return;

        case AST_EXPRESSION:
        {
            AstBin* children = (AstBin*)node->children;
            AstNode* left = children->left;
            AstNode* right = children->right;

            // NOT
            if (node->token.type == LOGICAL_NOT)
            {
                semanticExpression(right);
                if (!(right->expressionType & TYPE_BOOL) && right->expressionType != TYPE_UNKNOWN)
                    errorExit(SEM_TYPE, "Operand of '!' must be boolean", node->token.line,
                              &node->token);
                node->expressionType = TYPE_BOOL;
                return;
            }

            semanticExpression(left);
            semanticExpression(right);

            if (!checkBinaryTypes(left, node, right))
                errorExit(SEM_TYPE, "Type mismatch in expression", node->token.line, &node->token);

            node->expressionType = getBinaryResultType(left, node, right);
            return;
        }

        default:
            return;
    }
}

/* --------------------------------------------------------- */
/*  ZÁKLADNÍ SEMANTIKA PŘIŘAZENÍ                            */
/* --------------------------------------------------------- */

/**
 * @brief Semantika pro přiřazení.
 *
 * Zatím:
 *  - spočítá typ pravé strany
 *  - zkusí přenést typ na levou stranu
 *  - neřeší symtable ani setter/getter
 *
 * Později můžeš napojit na SymTableStack a rozlišovat proměnné vs setter.
 */
void semanticAssignment(AstNode* node)
{
    if (!node) return;

    AstBin* children = (AstBin*)node->children;
    AstNode* left = children->left;
    AstNode* right = children->right;

    // Vyhodnotíme typ pravé strany
    semanticExpression(right);

    // Jednoduchá typová kontrola – pokud jsou oba známé a různé, je to chyba
    if (left->expressionType != TYPE_UNKNOWN && right->expressionType != TYPE_UNKNOWN &&
        left->expressionType != right->expressionType)
    {
        errorExit(SEM_TYPE, "Invalid assignment: incompatible types", left->token.line,
                  &left->token);
    }

    // Zkopírujeme typ z pravé strany, pokud levá strana neznámá
    if (left->expressionType == TYPE_UNKNOWN)
    {
        left->expressionType = right->expressionType;
    }
}

/* --------------------------------------------------------- */
/*  CHECK-LATER & KONTROLA FUNKCÍ                           */
/* --------------------------------------------------------- */

/**
 * @brief Zkontroluje, že v globálním scope nejsou nedefinované funkce.
 *
 * Předpoklad: všechny funkce/gettery/settery mají:
 *  - kind = SYM_FUNC / SYM_GET / SYM_SET
 *  - flag `declared` nastavený na true při definici
 *
 * Pokud narazí na funkci s declared == false, vyhodí SEM_UNDEF.
 */
bool checkFunDec(Scope* globalScope)
{
    if (!globalScope) return true;

    for (int i = 0; i < globalScope->size; ++i)
    {
        Symbol* sym = globalScope->symbols[i];
        if (!sym || sym == TOMBSTONE) continue;

        if ((sym->kind == SYM_FUNC || sym->kind == SYM_GET || sym->kind == SYM_SET) &&
            !sym->declared)
        {
            errorExit(SEM_UNDEF, "Function/getter/setter declared but not defined", 0, NULL);
        }
    }

    return true;
}
// ... [Previous includes and helper functions remain the same: findFunctionExact, etc.] ...

/* --------------------------------------------------------- */
/* RESOLVE LATER LOGIC                                      */
/* --------------------------------------------------------- */

/**
 * @brief Logic to resolve a single AST_FUN_CALL node.
 * Checks existence, updates types, and validates arguments.
 */
void checkFunctionCallNode(AstNode* node, SymTableStack* stack)
{
    if (!node || node->type != AST_FUN_CALL) return;

    char* name = node->token.value->stringVal;

    // 1. Get argument count
    AstNode* paramsNode = ((AstBin*)node->children)->right;
    int argc = 0;
    if (paramsNode && paramsNode->type == AST_PARAMS)
        argc = ((AstN*)paramsNode->children)->childrenList->count;

    // 2. Try to find Exact Match (name$argc)
    Symbol* exact = findFunctionExact(stack, name, argc);
    if (exact)
    {
        // Check argument types against definition
        if (paramsNode)
        {
            ListItem* argItem = ((AstN*)paramsNode->children)->childrenList->firstItem;
            for (int i = 0; i < argc; i++)
            {
                if (!argItem) break;
                AstNode* arg = argItem->item;

                // Argument types must match
                ExprType expected = exact->paramTypes[i];
                ExprType got = arg->expressionType;

                if ((expected & got) == 0 && expected != TYPE_UNKNOWN)
                {
                    errorExit(SEM_ARG, "Invalid argument type for function", arg->token.line,
                              &arg->token);
                }
                argItem = argItem->next;
            }
        }

        // Success: Update node type
        node->expressionType = exact->expressionType;
        return;
    }

    // 3. Try Getter
    Symbol* getter = findGetter(stack, name);
    if (getter)
    {
        if (argc != 0)
            errorExit(SEM_ARG, "Getter cannot take arguments", node->token.line, &node->token);

        node->expressionType = getter->expressionType;
        return;
    }

    // 4. Try Setter
    Symbol* setter = findSetter(stack, name);
    if (setter)
    {
        errorExit(SEM_TYPE, "Setter cannot be called as a function", node->token.line,
                  &node->token);
    }

    // 5. Wrong arity (Overload exists but wrong args)
    Symbol* overload = findFunctionAnyArity(stack, name);
    if (overload)
    {
        errorExit(SEM_ARG, "Wrong number of arguments", node->token.line, &node->token);
    }

    // 6. Undefined
    errorExit(SEM_UNDEF, "Call to undefined function", node->token.line, &node->token);
}

/**
 * @brief Recursively traverses the AST to resolve types that were unknown during parsing.
 * Handles Assignments, Expressions, and Function Calls.
 */
void resolveRecursive(AstNode* node, SymTableStack* stack)
{
    if (!node) return;

    switch (node->type)
    {
        case AST_FUN_CALL:
        {
            checkFunctionCallNode(node, stack);
            break;
        }

        case AST_VAR_ASSIGN:
        {
            AstBin* children = (AstBin*)node->children;
            resolveRecursive(children->right, stack);

            if (children->left->expressionType == TYPE_UNKNOWN)
            {
                children->left->expressionType = children->right->expressionType;

                if (children->left->type == AST_IDENTIFIER)
                {
                    Symbol* s =
                        symTableStackFindSymbol(stack, children->left->token.value->stringVal);
                    if (s)
                        s->expressionType = children->right->expressionType;
                    else
                    {
                        Symbol* newSym = symbolCreate(children->left->token.value->stringVal,
                                                      children->right->expressionType, SYM_VAR, 0);
                        scopeAddSymbol(stack->scopes[stack->top], newSym);
                    }
                }
            }

            semanticAssignment(node);
            node->expressionType = children->left->expressionType;
            break;
        }

        case AST_EXPRESSION:
        {
            AstBin* children = (AstBin*)node->children;
            resolveRecursive(children->left, stack);
            resolveRecursive(children->right, stack);

            if (!checkBinaryTypes(children->left, node, children->right))
            {
                errorExit(SEM_TYPE, "Type mismatch in expression (resolved later)",
                          node->token.line, &node->token);
            }
            node->expressionType = getBinaryResultType(children->left, node, children->right);
            break;
        }

        case AST_LITERAL:
            return;

        case AST_IDENTIFIER:
            Symbol* symbol = symTableStackFindSymbol(stack, node->token.value->stringVal);
            if (symbol) node->expressionType = symbol->expressionType;
            break;
        case AST_IFJ:
            node->expressionType = ((AstBin*)node->children)->right->expressionType;

            break;
        default:
            return;
    }
}

/**
 * @brief Iterates through checkLaterList and resolves all unknown types
 *
 * @param checkLaterList - list to be resolved
 * @param stack - symtable stack used for resolving symbol types
 */
void semanticResolveCheckLater(SLList* checkLaterList, SymTableStack* stack)
{
    if (!checkLaterList) return;

    ListItem* resolve = checkLaterList->firstItem;

    while (resolve)
    {
        resolveRecursive(resolve->item, stack);
        resolve = resolve->next;
    }
}