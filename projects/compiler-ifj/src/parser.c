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

#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "semantic.h"
#include "utils.h"

/* --------------------------------------------------------------- */
/* HELPER FUNCTIONS                                                */
/* --------------------------------------------------------------- */
void tokenDeepCopy(Token* src, Token* dst)
{
    if (!src || !dst) return;

    dst->type = src->type;
    dst->line = src->line;

    if (src->value)
    {
        dst->value = malloc(sizeof(TokenValue));
        if (!dst->value) return;

        dst->value->intVal = src->value->intVal;
        dst->value->floatVal = src->value->floatVal;

        if (src->value->stringVal)
        {
            dst->value->stringVal = malloc(strlen(src->value->stringVal) + 1);
            if (!dst->value->stringVal)
            {
                free(dst->value);
                dst->value = NULL;
                return;
            }
            strcpy(dst->value->stringVal, src->value->stringVal);
        }
        else
        {
            dst->value->stringVal = NULL;
        }
    }
    else
    {
        dst->value = NULL;
    }
}

/* --------------------------------------------------------------- */
/* PARSER FUNCTIONS                                                 */
/* --------------------------------------------------------------- */

/**
 * @brief Initialization of Parser Struct
 *
 * @param scanner - pointer to Scanner state
 * @return Parser* - pointer to initialized Parser struct
 */
Parser* parserInit(Scanner* scanner, SymTableStack* symStack, SLList* resolveLater)
{
    Parser* parser = malloc(sizeof(Parser));

    if (!parser) errorExit(INTERNAL_ERROR, "Unable to allocate space for Parser", 0, NULL);

    parser->scanner = scanner;
    parser->symStack = symStack;
    parser->resolveLater = resolveLater;
    parser->lookAhead.type = NONE;
    parser->current.type = NONE;
    parser->root = NULL;

    return parser;
}

void parserDispose(Parser* parser)
{
    if (parser)
    {
        if (parser->scanner) disposeScanner(parser->scanner);
        if (parser->root) astDispose(parser->root);
        if (parser->symStack) freeSymTableStack(parser->symStack);
        free(parser);
    }
}

/**
 * @brief Function used to get new token and set is as current in parser
 *
 * @param parser - pointer to parser struct
 */
void parserAdvance(Parser* parser)
{
    if (parser->current.type != NONE) freeToken(&parser->current);
    if (getNextToken(parser->scanner, &parser->current) == 1)
    {
        parserDispose(parser);
        errorExit(LEXICAL_ERROR, "Invalid lexical type", 0, NULL);
    }
}

void parserLookAhead(Parser* parser)
{
    if (lookAhead(parser->scanner, &parser->lookAhead) == 1)
    {
        parserDispose(parser);
        errorExit(LEXICAL_ERROR, "Invalid lexical type", 0, NULL);
    }
}

/**
 * @brief Function validating current token with TokenType
 *
 * @param parser - pointer to Parser struct
 * @param type  - TokenType used to validate Gramar
 * @return true - TokenType and current  token are SAME
 * @return false - TokenType and current token are NOT SAME
 */

void parserValidate(Parser* parser, TokenType type, char* errorMessage)
{
    if (parser->current.type != type)
    {
        parserDispose(parser);
        errorExit(SYNTAX_ERROR, errorMessage, parser->current.line, &parser->current);
    }
}

void parserValidateSequence(Parser* p, TokenType* t, char** msg, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
    {
        parserAdvance(p);
        parserValidate(p, t[i], msg[i]);
    }
}

/* -------------------------------------------------------------- */
/* SLL Functions                                                  */
/* -------------------------------------------------------------- */

SLList* listInit(void)
{
    SLList* list = malloc(sizeof(SLList));
    if (!list) return NULL;

    list->count = 0;
    list->firstItem = NULL;
    list->lastItem = NULL;
    return list;
}

bool listAppend(SLList* list, AstNode* node)
{
    ListItem* item = malloc(sizeof(ListItem));
    if (!item) return false;

    item->item = node;
    item->next = NULL;

    if (list->lastItem)
        list->lastItem->next = item;
    else
        list->firstItem = item;

    list->lastItem = item;
    list->count++;
    return true;
}

void listDispose(SLList* list)
{
    ListItem* listItem = list->firstItem;
    while (listItem)
    {
        astDispose(listItem->item);
        ListItem* temp = listItem->next;
        free(listItem);
        listItem = temp;
    }

    free(list);
    list = NULL;
}

/* -------------------------------------------------------------- */
/* AST Functions                                                  */
/* -------------------------------------------------------------- */

AstNode* astInit(void)
{
    return NULL;
}

void astDispose(AstNode* tree)
{
    if (tree == NULL) return;

    switch (tree->type)
    {
        case AST_VAR_DEC:
        case AST_VAR_ASSIGN:
        case AST_RETURN:
        case AST_EXPRESSION:
        case AST_FUN_DEC:
        case AST_FUN_SET:
        case AST_FUN_GET:
        case AST_FUN_CALL:
        case AST_IF_STMT:
        case AST_IF_ELSE_STMT:
        case AST_ELSE_STMT:
        case AST_IFJ:
        case AST_OPERATOR:
        case AST_WHILE:
        {
            AstBin* children = tree->children;
            if (!children) break;

            if (children->left) astDispose(children->left);
            if (children->right) astDispose(children->right);

            free(children);
            break;
        }

        case AST_CLASS_DEC:
        case AST_BLOCK:
        case AST_PARAMS:
        {
            AstN* children = tree->children;
            if (!children) break;

            // Uvoľníme uzly rekurzívne - NEPOUŽÍVAME listDispose, lebo ten uvoľňuje uzly,
            // ktoré sú už súčasťou AST stromu a uvoľnia sa rekurzívne
            SLList* list = children->childrenList;
            if (list)
            {
                ListItem* item = list->firstItem;
                while (item)
                {
                    if (item->item) astDispose(item->item);
                    ListItem* next = item->next;
                    free(item);
                    item = next;
                }
                free(list);
            }

            free(children);
            break;
        }

        case AST_TYPE:
        case AST_IDENTIFIER:
        case AST_LITERAL:
            break;

        default:
            // Neplatný typ uzla - možno už uvoľnený alebo poškodený
            // Uvoľníme len token a uzol, bez rekurzie, aby sme sa vyhli double free
            freeToken(&tree->token);
            free(tree);
            return;
    }
    freeToken(&tree->token);
    free(tree);
}

AstNode* astCreateNode(AstNodeType type)
{
    AstNode* node = malloc(sizeof(AstNode));
    if (!node) errorExit(INTERNAL_ERROR, "Unable to allocate space for Node", 0, NULL);
    node->token.type = NONE;
    node->token.value = NULL;
    node->type = type;
    node->expressionType = TYPE_UNKNOWN;
    switch (type)
    {
        case AST_VAR_DEC:
        case AST_VAR_ASSIGN:
        case AST_EXPRESSION:
        case AST_RETURN:
        case AST_FUN_DEC:
        case AST_FUN_SET:
        case AST_FUN_GET:
        case AST_FUN_CALL:
        case AST_IF_STMT:
        case AST_IF_ELSE_STMT:
        case AST_ELSE_STMT:
        case AST_IFJ:
        case AST_OPERATOR:
        case AST_WHILE:
        {
            AstBin* children = malloc(sizeof(AstBin));
            if (!children) errorExit(INTERNAL_ERROR, "Unable to allocate space for Node", 0, NULL);
            node->children = children;
            children->left = NULL;
            children->right = NULL;
            break;
        }

        case AST_CLASS_DEC:
        case AST_BLOCK:
        case AST_PARAMS:
        {
            SLList* list = listInit();
            if (!list) errorExit(INTERNAL_ERROR, "Unable to allocate space for SLList", 0, NULL);

            AstN* children = malloc(sizeof(AstN));
            if (!children) errorExit(INTERNAL_ERROR, "Unable to allocate space for Node", 0, NULL);
            node->children = children;
            children->childrenList = list;
            break;
        }
        case AST_TYPE:
        case AST_IDENTIFIER:
        case AST_LITERAL:
            node->children = NULL;
            break;

        default:
            errorExit(INTERNAL_ERROR, "Uknown type of AST node when creating", 0, NULL);
    }

    return node;
}

/* -------------------------------------------------------------- */
/* AST STACK FUNCTIONS                                            */
/* -------------------------------------------------------------- */

void assignTypeFromToken(AstNode* node, Parser* parser)
{
    switch (node->token.type)
    {
        case INT_LITERAL:
            node->expressionType = TYPE_INT;
            break;
        case FLOAT_LITERAL:
            node->expressionType = TYPE_FLOAT;
            break;
        case STRING_LITERAL:
            node->expressionType = TYPE_STRING;
            break;
        case KW_VAL_NULL:
            node->expressionType = TYPE_NULL;
            break;
        case KW_VAL_FALSE:
        case KW_VAL_TRUE:
            node->expressionType = TYPE_BOOL;
            break;
        case IDENTIFIER:
            Symbol* symbol =
                symTableStackFindSymbol(parser->symStack, node->token.value->stringVal);
            if (!symbol)
            {
                Symbol* getterSym =
                    symbolCreate(node->token.value->stringVal, TYPE_UNKNOWN, SYM_GET, 1);
                Symbol* getter = symTableStackFindSymbol(parser->symStack, getterSym->name);
                if (getter)
                {
                    // Getter existuje → ID reprezentuje přístup ke getteru
                    node->expressionType = getter->expressionType;
                    return;
                }

                // 3) Nic → chyba
                errorExit(SEM_UNDEF, "Undefined variable", node->token.line, &node->token);
            }
            node->expressionType = symbol->expressionType;
            break;
        case GLOBAL_IDENTIFIER:
            Symbol* globalSymbol =
                symTableStackFindSymbol(parser->symStack, node->token.value->stringVal);
            if (!globalSymbol)
            {
                globalSymbol = symbolCreate(node->token.value->stringVal, TYPE_UNKNOWN, SYM_VAR, 0);
                scopeAddSymbol(parser->symStack->scopes[0], globalSymbol);
            }
            node->expressionType = globalSymbol->expressionType;
            break;
        default:
            node->expressionType = TYPE_UNKNOWN;
            break;
    }
}

void assignTypeFromSymtable(AstNode* node, SymTableStack* symTable)
{
    Symbol* symbol = symTableStackFindSymbol(symTable, node->token.value->stringVal);
    if (!symbol) errorExit(SEM_UNDEF, "Undefined variable", node->token.line, &node->token);
    node->expressionType = symbol->expressionType;
}

void astStackInit(AstStack* stack, int capacity)
{
    if (stack == NULL)
    {
        errorExit(INTERNAL_ERROR, "Invalid pointer to AST_STACK", 0, NULL);
    }
    else
    {
        stack->array = malloc(sizeof(char) * capacity);

        if (stack->array != NULL)
        {
            stack->topIndex = -1;
        }
    }
}

/**
 * Vrací nenulovou hodnotu, je-li zásobník prázdný, jinak vrací hodnotu 0.
 * Funkci implementujte jako jediný příkaz.
 * Vyvarujte se zejména konstrukce typu "if ( cond ) b=true else b=false".
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 *
 * @returns true v případě, že je zásobník prázdný, jinak false
 */
bool astStackIsEmpty(AstStack* stack)
{
    return stack->topIndex == -1;
}

/**
 * Vrací nenulovou hodnotu, je-li zásobník plný, jinak vrací hodnotu 0.
 * Dejte si zde pozor na častou programátorskou chybu "o jedničku" a dobře se
 * zamyslete, kdy je zásobník plný, jestliže může obsahovat nejvýše STACK_SIZE
 * prkvů a první prvek je vložen na pozici 0.
 *
 * Funkci implementujte jako jediný příkaz.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 *
 * @returns true v případě, že je zásobník plný, jinak false
 */
bool astStackIsFull(const AstStack* stack)
{
    return stack->topIndex == ASTSTACK_MAX - 1;
}

/**
 * Vrací znak z vrcholu zásobníku prostřednictvím parametru childrenPtr.
 * Tato operace ale prvek z vrcholu zásobníku neodstraňuje.
 * Volání operace Top při prázdném zásobníku je nekorektní a ošetřete ho voláním
 * procedury Stack_Error(SERR_TOP).
 *
 * Pro ověření, zda je zásobník prázdný, použijte dříve definovanou funkci
 * Stack_IsEmpty.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param childrenPtr Ukazatel na cílovou proměnnou
 */
AstNode* astStackTop(AstStack* stack)
{
    if (astStackIsEmpty(stack)) errorExit(INTERNAL_ERROR, "Nothing on top of stack", 0, NULL);

    return stack->array[stack->topIndex];
}

/**
 * Odstraní prvek z vrcholu zásobníku. Pro ověření, zda je zásobník prázdný,
 * použijte dříve definovanou funkci Stack_IsEmpty.
 *
 * Vyvolání operace Pop při prázdném zásobníku je sice podezřelé a může
 * signalizovat chybu v algoritmu, ve kterém je zásobník použit, ale funkci
 * pro ošetření chyby zde nevolejte (můžeme zásobník ponechat prázdný).
 * Spíše než volání chyby by se zde hodilo vypsat varování, což však pro
 * jednoduchost neděláme.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 */
AstNode* astStackPop(AstStack* stack)
{
    if (astStackIsEmpty(stack)) return NULL;
    stack->topIndex--;
    return stack->array[stack->topIndex + 1];
}

/**
 * Vloží znak na vrchol zásobníku. Pokus o vložení prvku do plného zásobníku
 * je nekorektní a ošetřete ho voláním procedury Stack_Error(SERR_PUSH).
 *
 * Pro ověření, zda je zásobník plný, použijte dříve definovanou
 * funkci Stack_IsFull.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param children Znak k vložení
 */

void astStackPush(AstStack* stack, AstNode* node)
{
    if (astStackIsFull(stack))
        errorExit(INTERNAL_ERROR, "Couldnt push to AST_STACK. Stack is FULL", 0, NULL);
    stack->topIndex++;
    stack->array[stack->topIndex] = node;
}

AstNode* astStackNextOp(AstStack* stack)
{
    if (stack->topIndex < 0) return NULL;

    // Start from the top and go down until you find an operator
    for (int i = stack->topIndex; i >= 0; i--)  // <= změna z i > 0 na i >= 0
    {
        AstNode* node = stack->array[i];
        if (node->type == AST_OPERATOR) return node;
    }

    return NULL;  // no operator found
}

/**
 * Zruší a uvolní dynamicky alokované prostředky struktury.
 * Uvede zásobník do prázdného stavu.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 */
void astStackDispose(AstStack* stack)
{
    free(stack->array);
    stack->array = NULL;
    stack->topIndex = -1;
}

/* -------------------------------------------------------------- */
/* Parser Gramatic Functions                                      */
/* -------------------------------------------------------------- */

void parseProgram(Parser* parser)
{
    parser->root = parseClassDec(parser);

    parserLookAhead(parser);
    if (parser->lookAhead.type == EOL) parserAdvance(parser);

    parserAdvance(parser);
    parserValidate(parser, EOF_TOKEN, "Invalid syntax expected EOF");

    Symbol* mainFun = symTableStackFindSymbol(parser->symStack, "main$0");
    if (!mainFun) errorExit(SEM_UNDEF, "Missing main function with no parameters", 0, NULL);
}

AstNode* parseClassDec(Parser* parser)
{
    char* msg[] = {"'class'", "identifier"};
    parserValidateSequence(parser, (TokenType[]){KW_CLASS, IDENTIFIER}, msg, 2);

    AstNode* classNode = astCreateNode(AST_CLASS_DEC);
    tokenDeepCopy(&parser->current, &classNode->token);

    char* msg2[] = {"Invalid syntax, expected '{'", "new line"};
    parserValidateSequence(parser, (TokenType[]){LCURLY, EOL}, msg2, 2);

    parserLookAhead(parser);
    while (parser->lookAhead.type == KW_STATIC)
    {
        AstNode* node = parseFunDec(parser);
        if (!listAppend(((AstN*)classNode->children)->childrenList, node))
            errorExit(INTERNAL_ERROR, "Unable to allocate space for Parser", 0, NULL);
        parserLookAhead(parser);
    }

    parserAdvance(parser);
    parserValidate(parser, RCURLY, "Invalid syntax expected '}'");

    return classNode;
}

AstNode* parseBlock(Parser* parser, Symbol* funSym)
{
    char* msg[] = {"'{'", "new line"};
    parserValidateSequence(parser, (TokenType[]){LCURLY, EOL}, msg, 2);

    AstNode* node = astCreateNode(AST_BLOCK);
    parseStList(parser, ((AstN*)node->children)->childrenList, funSym);

    parserAdvance(parser);
    parserValidate(parser, RCURLY, "Invalid Syntax expected '}' after new line");

    parserLookAhead(parser);
    if (parser->lookAhead.type != KW_ELSE && parser->lookAhead.type != EOF_TOKEN)
    {
        parserAdvance(parser);
        parserValidate(parser, EOL, "Invalid Syntax expected new line after '}'");
    }

    return node;
}

void parseStList(Parser* parser, SLList* list, Symbol* funSym)
{
    AstNode* node;

    parserLookAhead(parser);
    while (parser->lookAhead.type != RCURLY && parser->lookAhead.type != EOF_TOKEN)
    {
        switch (parser->lookAhead.type)
        {
            case KW_IFJ:
                node = parseIfj(parser);
                break;

            case LCURLY:
                symTableStackPush(parser->symStack);
                node = parseBlock(parser, funSym);
                symTableStackPop(parser->symStack);
                break;

            case KW_RETURN:
                node = parseReturn(parser, funSym);
                break;

            case KW_WHILE:
                node = parseWhile(parser, funSym);
                break;
            case KW_IF:
                parseIfChain(parser, list, funSym);
                parserLookAhead(parser);
                continue;
                break;

            case KW_VAR:
                node = parseVarDec(parser);
                break;

            case IDENTIFIER:
            case GLOBAL_IDENTIFIER:
                node = parseVarAssign(parser);
                break;

            default:
                errorExit(SYNTAX_ERROR, "Invalid syntax, expected statement start",
                          parser->current.line, &parser->current);
                break;
        }
        listAppend(list, node);
        parserLookAhead(parser);
    }
}

AstNode* parseFunDec(Parser* parser)
{
    parserAdvance(parser);
    parserValidate(parser, KW_STATIC, "Invalid syntax, expected 'static'");

    parserAdvance(parser);
    parserValidate(parser, IDENTIFIER, "Invalid syntax, expected function name");

    AstNode* funNode = astCreateNode(AST_FUN_DEC);
    tokenDeepCopy(&parser->current, &funNode->token);
    AstBin* children = funNode->children;

    parserLookAhead(parser);

    if (parser->lookAhead.type == LPAR)  // ident(
    {
        children->left = parseParams(parser);
    }
    else if (parser->lookAhead.type == ASSIGN)  // ident=(

    {
        funNode->type = AST_FUN_SET;
        parserAdvance(parser);  // consume '='
        children->left = parseParams(parser);

        int paramCount = ((AstN*)children->left->children)->childrenList->count;
        if (paramCount != 1)
            errorExit(SYNTAX_ERROR, "Setter has invalid count of arguments", parser->current.line,
                      &parser->current);
    }
    else if (parser->lookAhead.type == LCURLY)
    {
        funNode->type = AST_FUN_GET;
    }
    else
    {
        errorExit(SYNTAX_ERROR, "Invalid syntax, expected '(' or '=' or '{' after identifier",
                  parser->current.line, &parser->current);
    }

    SymbolKind symType = SYM_FUNC;
    if (funNode->type == AST_FUN_SET) symType = SYM_SET;
    if (funNode->type == AST_FUN_GET) symType = SYM_GET;

    int paramsCount = 0;
    if ((AstNode*)children->left)
        paramsCount = ((AstN*)children->left->children)->childrenList->count;

    Symbol* funSymbol =
        symbolCreate(funNode->token.value->stringVal, TYPE_NULL, symType, paramsCount);

    Symbol* foundSymbol = symTableStackFindSymbol(parser->symStack, funSymbol->name);
    if (foundSymbol && foundSymbol->numOfParams == paramsCount && foundSymbol->declared)
    {
        errorExit(SEM_REDEF, "Conflicting declaration of function", funNode->token.line,
                  &funNode->token);
    }
    else if (foundSymbol && !foundSymbol->declared)
    {
        foundSymbol->declared = true;
    }
    else
    {
        scopeAddSymbol(parser->symStack->scopes[0], funSymbol);
        funSymbol->declared = true;
    }

    symTableStackPush(parser->symStack);

    ListItem* listItem = NULL;
    AstNode* paramNode = children->left;
    if (paramNode) listItem = ((AstN*)paramNode->children)->childrenList->firstItem;
    int top = parser->symStack->top;

    while (listItem)
    {
        Symbol* symbol =
            symbolCreate(listItem->item->token.value->stringVal, TYPE_UNKNOWN, SYM_PARAM, 0);
        Symbol* foundSymbol =
            scopeFindSymbol(parser->symStack->scopes[parser->symStack->top], symbol->name);
        if (foundSymbol)
            errorExit(SEM_REDEF, "Redefinition of parameter in function",
                      listItem->item->token.line, &listItem->item->token);
        listItem = listItem->next;
        scopeAddSymbol(parser->symStack->scopes[top], symbol);
    }

    children->right = parseBlock(parser, funSymbol);
    funNode->expressionType = funSymbol->expressionType;

    symTableStackPop(parser->symStack);

    return funNode;
}

AstNode* parseParams(Parser* parser)
{
    AstNode* node = astCreateNode(AST_PARAMS);
    AstN* children = node->children;

    parserAdvance(parser);
    parserValidate(parser, LPAR, "Invalid syntax, expected '('");

    parserLookAhead(parser);

    if (parser->lookAhead.type == RPAR)
    {
        parserAdvance(parser);
        parserValidate(parser, RPAR, "Invalid syntax, expected ')'");
        return node;
    }

    while (1)
    {
        parserAdvance(parser);
        parserValidate(parser, IDENTIFIER, "Invalid parameter, expected identifier");

        AstNode* leaf = astCreateNode(AST_IDENTIFIER);
        tokenDeepCopy(&parser->current, &leaf->token);
        listAppend(children->childrenList, leaf);

        parserLookAhead(parser);

        if (parser->lookAhead.type == COMMA)
        {
            parserAdvance(parser);
            parserLookAhead(parser);
            continue;
        }
        if (parser->lookAhead.type == RPAR)
        {
            parserAdvance(parser);
            parserValidate(parser, RPAR, "Invalid syntax, expected ')'");
            break;
        }

        errorExit(SYNTAX_ERROR, "Missing comma or ')' in parameter list", parser->lookAhead.line,
                  &parser->lookAhead);
    }

    return node;
}
AstNode* parseArguments(Parser* parser)
{
    AstNode* node = astCreateNode(AST_PARAMS);
    AstN* children = node->children;
    AstNode* leaf = NULL;
    parserLookAhead(parser);

    if (parser->lookAhead.type == RPAR)
    {
        parserAdvance(parser);
        return node;
    }

    while (1)
    {
        parserAdvance(parser);

        switch (parser->current.type)
        {
            case IDENTIFIER:
            case GLOBAL_IDENTIFIER:
                leaf = astCreateNode(AST_IDENTIFIER);
                break;

            case INT_LITERAL:
            case STRING_LITERAL:
            case MULTILINE_STRING_LITERAL:
            case FLOAT_LITERAL:
            case KW_VAL_NULL:
                leaf = astCreateNode(AST_LITERAL);
                break;

            default:
                errorExit(SYNTAX_ERROR, "Invalid function argument", parser->current.line,
                          &parser->current);
        }

        if (leaf)
        {
            tokenDeepCopy(&parser->current, &leaf->token);
            assignTypeFromToken(leaf, parser);
            listAppend(children->childrenList, leaf);
        }

        parserLookAhead(parser);

        if (parser->lookAhead.type == COMMA)
        {
            parserAdvance(parser);  // consume ','
            parserLookAhead(parser);
            continue;
        }
        else if (parser->lookAhead.type == RPAR)
        {
            parserAdvance(parser);  // consume ')'
            break;
        }
        else
        {
            errorExit(SYNTAX_ERROR, "Missing comma or ')' in argument list", parser->lookAhead.line,
                      &parser->lookAhead);
        }
    }

    return node;
}

AstNode* parseFunCall(Parser* parser)
{
    parserValidate(parser, IDENTIFIER, "Invalid syntax, expected function identifier");

    Token* funcIdentifierToken = malloc(sizeof(Token));

    if (!funcIdentifierToken) errorExit(INTERNAL_ERROR, "Couldnt malloc token", 0, NULL);

    tokenDeepCopy(&parser->current, funcIdentifierToken);

    parserAdvance(parser);
    parserValidate(parser, LPAR, "Invalid syntax, expected '(' after identifier");

    AstNode* node = astCreateNode(AST_FUN_CALL);
    tokenDeepCopy(funcIdentifierToken, &node->token);
    freeToken(funcIdentifierToken);
    free(funcIdentifierToken);
    ((AstBin*)node->children)->right = parseArguments(parser);
    if (((AstBin*)node->children)->right)
    {
    }
    int argCount = 0;

    Symbol* funCall = symbolCreate(node->token.value->stringVal, TYPE_UNKNOWN, SYM_FUNC, argCount);

    Symbol* funDec = NULL;
    if (funCall)
    {
        funDec = symTableStackFindSymbol(parser->symStack, funCall->name);
    }

    if (!funDec)
        listAppend(parser->resolveLater, node);
    else
        node->expressionType = funDec->expressionType;

    parserAdvance(parser);
    parserValidate(parser, EOL, "Invalid syntax, expected 'EOL' after identifier");
    freeSymbol(funCall);

    return node;
}

AstNode* parseIfj(Parser* parser)
{
    parserAdvance(parser);  // consume KW_IFJ

    parserAdvance(parser);
    parserValidate(parser, DOT, "Invalid syntax, expected '.' after 'Ifj'");
    parserLookAhead(parser);

    if (parser->lookAhead.type == EOL) parserAdvance(parser);

    parserAdvance(parser);
    parserValidate(parser, IDENTIFIER, "Invalid syntax, expected function name after '.'");

    AstNode* node = astCreateNode(AST_IFJ);
    AstBin* children = node->children;
    children->right = parseFunCall(parser);
    node->expressionType = children->right->expressionType;
    return node;
}

AstNode* parseVarDec(Parser* parser)
{
    parserAdvance(parser);  // consume KW_VAR

    parserAdvance(parser);
    parserValidate(parser, IDENTIFIER, "Invalid syntax, expected id after 'var'");

    int top = parser->symStack->top;

    AstNode* leaf = astCreateNode(AST_VAR_DEC);
    tokenDeepCopy(&parser->current, &leaf->token);
    Symbol* funSym = scopeFindSymbol(parser->symStack->scopes[top], leaf->token.value->stringVal);
    if (funSym) errorExit(SEM_REDEF, "Redefining variable", leaf->token.line, &leaf->token);

    Symbol* symbol = symbolCreate(leaf->token.value->stringVal, TYPE_NULL, SYM_VAR, 0);
    scopeAddSymbol(parser->symStack->scopes[top], symbol);

    parserAdvance(parser);
    parserValidate(parser, EOL, "Invalid syntax, expected EOL after ID");
    leaf->expressionType = symbol->expressionType;
    return leaf;
}

AstNode* parseVarAssign(Parser* parser)
{
    parserAdvance(parser);

    AstNode* leaf = astCreateNode(AST_IDENTIFIER);
    tokenDeepCopy(&parser->current, &leaf->token);

    parserAdvance(parser);
    parserValidate(parser, ASSIGN, "Invalid syntax, expected '=' after ID");

    AstNode* node = astCreateNode(AST_VAR_ASSIGN);
    AstBin* children = node->children;
    children->left = leaf;

    Symbol* leafSymbol = symTableStackFindSymbol(parser->symStack, leaf->token.value->stringVal);

    if (!leafSymbol && leaf->token.type != GLOBAL_IDENTIFIER)
    {
        Symbol* setterSym = symbolCreate(leaf->token.value->stringVal, TYPE_UNKNOWN, SYM_SET, 1);
        Symbol* setter = symTableStackFindSymbol(parser->symStack, setterSym->name);

        if (!setter && !leafSymbol)
        {
            errorExit(SEM_UNDEF, "Undefined variable", leaf->token.line, &leaf->token);
        }
    }

    else if (!leafSymbol)
    {
        leafSymbol = symbolCreate(leaf->token.value->stringVal, TYPE_UNKNOWN, SYM_VAR, 0);
        scopeAddSymbol(parser->symStack->scopes[0], leafSymbol);
    }

    parserLookAhead(parser);

    switch (parser->lookAhead.type)
    {
        case KW_IFJ:
            children->right = parseIfj(parser);
            break;

        case IDENTIFIER:
            parserAdvance(parser);  // preapare token
            parserLookAhead(parser);
            if (parser->lookAhead.type == LPAR)
            {
                children->right = parseFunCall(parser);
                semanticExpression(children->right);
            }
            else
            {
                children->right = parseExp(parser);
                semanticExpression(children->right);
            }
            break;

        default:
            parserAdvance(parser);
            children->right = parseExp(parser);
            semanticExpression(children->right);
            semanticAssignment(node);
            break;
    }
    if (leafSymbol) leafSymbol->expressionType = leaf->expressionType;
    node->expressionType = children->right->expressionType;

    if ((node->expressionType & TYPE_UNKNOWN) == 1) listAppend(parser->resolveLater, node);

    return node;
}

AstNode* parseReturn(Parser* parser, Symbol* funSym)
{
    parserAdvance(parser);  // consume KW_RETURN

    parserAdvance(parser);  // preapare TOKEN
    AstNode* node = astCreateNode(AST_RETURN);
    AstBin* children = node->children;
    children->right = parseExp(parser);
    if (!children->right)
        errorExit(SYNTAX_ERROR, "return is missing expression", node->token.line, &node->token);
    semanticExpression(children->right);
    if ((funSym->expressionType & TYPE_UNKNOWN) == 1)
        funSym->expressionType = children->right->expressionType;
    else
        funSym->expressionType = funSym->expressionType | children->right->expressionType;

    return node;
}

void parseIfChain(Parser* p, SLList* list, Symbol* funSym)
{
    AstNode* node = parseIf(p, funSym);
    listAppend(list, node);
    parserLookAhead(p);

    while (p->lookAhead.type == KW_ELSE)
    {
        parserAdvance(p);  // consume ELSE
        parserLookAhead(p);

        AstNode* node;

        if (p->lookAhead.type == KW_IF)
            node = parseIfElse(p, funSym);
        else
            node = parseElse(p, funSym);

        listAppend(list, node);

        parserLookAhead(p);
    }
}

AstNode* parseIf(Parser* parser, Symbol* funSym)
{
    parserAdvance(parser);  // consume KW_IF

    parserAdvance(parser);
    parserValidate(parser, LPAR, "Invalid syntax, expected '(' after identifier");

    parserAdvance(parser);
    AstNode* node = astCreateNode(AST_IF_STMT);
    AstBin* children = node->children;
    children->left = parseExp(parser);
    semanticExpression(children->left);

    // token already consumed in exp,just check if valid
    parserValidate(parser, RPAR, "Invalid syntax, expected ')' after '(' or parameters");

    symTableStackPush(parser->symStack);
    children->right = parseBlock(parser, funSym);
    symTableStackPop(parser->symStack);
    return node;
}

AstNode* parseIfElse(Parser* parser, Symbol* funSym)
{
    parserAdvance(parser);
    parserValidate(parser, LPAR, "Invalid syntax, expected '(' after identifier");
    parserAdvance(parser);

    AstNode* node = astCreateNode(AST_IF_ELSE_STMT);
    AstBin* children = node->children;
    children->left = parseExp(parser);
    semanticExpression(children->left);

    parserValidate(parser, RPAR, "Invalid syntax, expected ')' after '(' or parameters");

    symTableStackPush(parser->symStack);
    children->right = parseBlock(parser, funSym);
    symTableStackPop(parser->symStack);
    return node;
}

AstNode* parseElse(Parser* parser, Symbol* funSym)
{
    AstNode* node = astCreateNode(AST_ELSE_STMT);
    AstBin* children = node->children;

    symTableStackPush(parser->symStack);
    children->right = parseBlock(parser, funSym);
    symTableStackPop(parser->symStack);
    return node;
}

AstNode* parseWhile(Parser* parser, Symbol* funSym)
{
    parserAdvance(parser);  // consume KW_IF
    parserAdvance(parser);
    parserValidate(parser, LPAR, "Invalid syntax, expected '(' after identifier");
    parserAdvance(parser);

    AstNode* node = astCreateNode(AST_WHILE);
    AstBin* children = node->children;
    children->left = parseExp(parser);
    if (!children->left)
        errorExit(SYNTAX_ERROR, "while is missing expression", node->token.line, &node->token);
    semanticExpression(children->left);

    parserValidate(parser, RPAR, "Invalid syntax, expected ')' after '(' or parameters");

    symTableStackPush(parser->symStack);
    children->right = parseBlock(parser, funSym);
    symTableStackPop(parser->symStack);
    return node;
}

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//*  -1 = shift, 1 = reduce, 0 = error
//*  rows = stack top, columns = current token
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* cmp - < > <= >= , MD - * / PM - +-  /

const int PRECEDENCE_TABLE[NUM_PRECEDENCE][NUM_PRECEDENCE] = {
    /*        id  is   T   (   )  MD  PM  cmp eq  &&  ||   !   $ */
    /* id */ {+0, -1, +0, +0, -1, -1, -1, -1, -1, -1, -1, +0, +1},
    /* is */ {+0, +0, -1, +0, -1, +0, +0, +0, +1, +1, +1, +0, +1},
    /*TYPE*/ {+0, +0, +0, +0, -1, +0, +0, +0, -1, -1, -1, +0, +1},
    /* (  */ {-1, -1, +0, -1, -1, -1, -1, -1, -1, -1, -1, -1, +1},
    /* )  */ {+0, +1, +0, +0, -1, +1, +1, +1, +1, +1, +1, -1, +1},
    /* MD */ {-1, -1, +0, -1, -1, +1, +1, +1, +1, +1, +1, -1, +1},
    /* PM */ {-1, -1, +0, -1, -1, -1, +1, +1, +1, +1, +1, -1, +1},
    /* cmp*/ {-1, -1, +0, -1, -1, -1, -1, +1, +1, +1, +1, -1, +1},
    /* eq */ {-1, -1, +0, -1, -1, -1, -1, -1, +1, +1, +1, -1, +1},
    /* && */ {-1, -1, +0, -1, -1, -1, -1, -1, -1, +1, +1, -1, +1},
    /* || */ {-1, -1, +0, -1, -1, -1, -1, -1, -1, -1, +1, -1, +1},
    /* NOT*/ {-1, -1, +0, -1, +1, +1, +1, +1, +1, +1, +1, +1, +1},
    /* $  */ {-1, -1, -1, -1, +1, -1, -1, -1, -1, -1, -1, -1, +1}};

PrecedenceSymbol getPrecedenceSymbol(TokenType tokenType)
{
    switch (tokenType)
    {
        case IDENTIFIER:
        case GLOBAL_IDENTIFIER:
        case INT_LITERAL:
        case FLOAT_LITERAL:
        case STRING_LITERAL:
        case MULTILINE_STRING_LITERAL:
        case KW_VAL_NULL:
            return PREC_ID;

        case KW_TYPE_NUM:
        case KW_TYPE_NULL:
        case KW_TYPE_BOOL:
        case KW_TYPE_STRING:
            return PREC_TYPE;

        case IS_EQUAL:
        case IS_NOT_EQUAL:
            return PREC_EQ;  // ==, !=
        case IS_SMALLER:
        case IS_SMALLER_OR_EQUAL:
        case IS_BIGGER:
        case IS_BIGGER_OR_EQUAL:
            return PREC_CMP;

        case PLUS:
        case MINUS:
            return PREC_PLUSMINUS;

        case MULTIPLY:
        case DIVIDE:
            return PREC_MULDIV;

        case LPAR:
            return PREC_LPAREN;
        case LOGICAL_AND:
            return PREC_AND;
        case LOGICAL_OR:
            return PREC_OR;

        case LOGICAL_NOT:
            return PREC_NOT;

        case KW_IS:  // keyword `is`
            return PREC_IS;

        case EOL:  // nebo speciální end symbol
        case RPAR:
            return PREC_END;

        default:
            return NUM_PRECEDENCE;  // invalid / error
    }
}

AstNode* popCheck(AstStack* stack, const char* msg)
{
    if (astStackIsEmpty(stack))
        errorExit(SYNTAX_ERROR, "Invalid expression, stack is empty", 0, NULL);
    AstNode* node = astStackPop(stack);
    if (!node) errorExit(SYNTAX_ERROR, msg, 0, NULL);
    return node;
}

AstNode* reduceExpression(AstStack* stack)
{
    AstNode* right = popCheck(stack, "Reduce: Missing right operand");
    AstNode* op = popCheck(stack, "Reduce: Missing operator");
    AstNode* left = popCheck(stack, "Reduce: Missing left operand");

    if (right->type == AST_OPERATOR || left->type == AST_OPERATOR)
        errorExit(SYNTAX_ERROR, "Invalid expression, can't use operator as operand",
                  right->token.line, NULL);

    op->type = AST_EXPRESSION;
    ((AstBin*)op->children)->left = left;
    ((AstBin*)op->children)->right = right;

    return op;
}

AstNode* reduceNot(AstStack* stack)
{
    AstNode* right = popCheck(stack, "Reduce: Missing right operand");
    AstNode* op = popCheck(stack, "Reduce: Missing operator");

    op->type = AST_EXPRESSION;
    ((AstBin*)op->children)->right = right;

    return op;
}

AstNode* reducePar(AstStack* stack)
{
    AstNode* op = popCheck(stack, "Reduce: Missing operator");
    AstNode* left = popCheck(stack, "Reduce: Missing left operand");

    astDispose(left);
    return op;
}

AstNode* createNodeFromToken(Token* t, Parser* parser)
{
    AstNode* node;
    switch (t->type)
    {
        case IDENTIFIER:
        case GLOBAL_IDENTIFIER:
            node = astCreateNode(AST_IDENTIFIER);
            break;

        case KW_TYPE_BOOL:
        case KW_TYPE_NULL:
        case KW_TYPE_NUM:
        case KW_TYPE_STRING:
            node = astCreateNode(AST_TYPE);
            break;

        case INT_LITERAL:
        case FLOAT_LITERAL:
        case STRING_LITERAL:
        case MULTILINE_STRING_LITERAL:
        case KW_VAL_NULL:
            node = astCreateNode(AST_LITERAL);
            break;

        default:
            node = astCreateNode(AST_OPERATOR);
            break;
    }
    tokenDeepCopy(t, &node->token);
    assignTypeFromToken(node, parser);
    return node;
}

AstNode* parseExp(Parser* parser)
{
    AstStack stack;
    astStackInit(&stack, ASTSTACK_MAX);
    if (!stack.array) return NULL;
    int parenCount = 0;
    while (true)
    {
        if (parser->current.type == LCURLY) break;
        if (parser->current.type == EOL) break;
        if (parser->current.type == RPAR && parenCount == 0) break;

        if (parser->current.type == LPAR) parenCount++;
        if (parser->current.type == RPAR) parenCount--;

        if (parser->current.type == KW_IS)
        {
            parserLookAhead(parser);
            if (parser->lookAhead.type != KW_TYPE_NUM && parser->lookAhead.type != KW_TYPE_STRING &&
                parser->lookAhead.type != KW_TYPE_NULL && parser->lookAhead.type != KW_TYPE_BOOL)
            {
                errorExit(SYNTAX_ERROR, "Invalid syntax expected type after IS",
                          parser->lookAhead.line, &parser->lookAhead);
            }
        }

        AstNode* top = astStackIsEmpty(&stack) ? NULL : astStackNextOp(&stack);
        int currentIdx = getPrecedenceSymbol(parser->current.type);
        int stackIdx = top ? getPrecedenceSymbol(top->token.type) : PREC_END;
        int action = PRECEDENCE_TABLE[stackIdx][currentIdx];

        AstNode* node = createNodeFromToken(&parser->current, parser);

        if (action == -1)
        {
            assignTypeFromToken(node, parser);
            astStackPush(&stack, node);
            parserAdvance(parser);
        }
        else if (action == 1)
        {
            AstNode* reduced;
            if (top->token.type == LPAR)
            {
                astDispose(node);
                reduced = reducePar(&stack);
                parserAdvance(parser);
            }
            else if (top->token.type == LOGICAL_NOT)
            {
                reduced = reduceNot(&stack);
                parserAdvance(parser);
            }
            else
            {
                if (parser->current.type == RPAR) parenCount++;
                reduced = reduceExpression(&stack);
            }

            if (!reduced) errorExit(SEM_TYPE, "Invalid expression", parser->lookAhead.line, NULL);
            astStackPush(&stack, reduced);
        }
        else
        {
            errorExit(SYNTAX_ERROR, "Invalid token in expression", parser->lookAhead.line,
                      &parser->current);
        }
    }
    while (stack.topIndex > 0)
    {
        AstNode* reduced;
        AstNode* top = astStackNextOp(&stack);
        if (top->token.type == LPAR)
            reduced = reducePar(&stack);
        else if (top->token.type == LOGICAL_NOT)
            reduced = reduceNot(&stack);
        else
            reduced = reduceExpression(&stack);

        if (!reduced) errorExit(SEM_TYPE, "Invalid expression", parser->lookAhead.line, NULL);
        astStackPush(&stack, reduced);
    }

    AstNode* expression = NULL;

    if (!astStackIsEmpty(&stack))
    {
        expression = astStackPop(&stack);
    }

    if (parenCount != 0)
        errorExit(SYNTAX_ERROR, "Mismatched parentheses in expression", parser->lookAhead.line,
                  NULL);

    astStackDispose(&stack);

    return expression;
}

void loadIFJBuiltins(SymTableStack* stack)
{
    Scope* global = stack->scopes[0];

    // read_str
    Symbol* s1 = symbolCreate("read_str", (TYPE_STRING | TYPE_NULL), SYM_FUNC, 0);
    s1->declared = true;
    scopeAddSymbol(global, s1);

    // read_num
    Symbol* s2 = symbolCreate("read_num", (TYPE_FLOAT | TYPE_NULL), SYM_FUNC, 0);
    s2->declared = true;
    scopeAddSymbol(global, s2);
    // write
    Symbol* s3 = symbolCreate("write", TYPE_NULL, SYM_FUNC, 1);
    s3->declared = true;
    scopeAddSymbol(global, s3);

    // floor
    Symbol* s5 = symbolCreate("floor", TYPE_FLOAT, SYM_FUNC, 1);
    s5->declared = true;
    scopeAddSymbol(global, s5);

    // str
    Symbol* s6 = symbolCreate("str", TYPE_STRING, SYM_FUNC, 1);
    s6->declared = true;
    scopeAddSymbol(global, s6);

    // lenght
    Symbol* s7 = symbolCreate("length", TYPE_INT, SYM_FUNC, 1);
    s7->declared = true;
    scopeAddSymbol(global, s7);

    // substring
    Symbol* s8 = symbolCreate("substring", (TYPE_STRING | TYPE_NULL), SYM_FUNC, 3);
    s8->declared = true;
    scopeAddSymbol(global, s8);

    // strcmp
    Symbol* s9 = symbolCreate("strcmp", TYPE_INT, SYM_FUNC, 2);
    s9->declared = true;
    scopeAddSymbol(global, s9);

    // ord
    Symbol* s10 = symbolCreate("ord", TYPE_INT, SYM_FUNC, 2);
    s10->declared = true;
    scopeAddSymbol(global, s10);

    // substring
    Symbol* s11 = symbolCreate("chr", TYPE_STRING, SYM_FUNC, 1);
    s11->declared = true;
    scopeAddSymbol(global, s11);

    // write(term)
    s3->paramTypes[0] = TYPE_UNKNOWN;

    // floor(num)
    s5->paramTypes[0] = TYPE_FLOAT | TYPE_INT | TYPE_UNKNOWN;

    // str(term)
    s7->paramTypes[0] = TYPE_UNKNOWN;

    // length(s:string)
    s7->paramTypes[0] = TYPE_STRING | TYPE_UNKNOWN;

    // substring(s:string, i:int, j:int)
    s8->paramTypes[0] = TYPE_STRING | TYPE_UNKNOWN;
    s8->paramTypes[1] = TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN;
    s8->paramTypes[2] = TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN;

    // strcmp(s1:string, s2:string)
    s9->paramTypes[0] = TYPE_STRING | TYPE_UNKNOWN;
    s9->paramTypes[1] = TYPE_STRING | TYPE_UNKNOWN;

    // ord(s: string, i: int)
    s10->paramTypes[0] = TYPE_STRING | TYPE_UNKNOWN;
    s10->paramTypes[1] = TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN;

    // chr(i:int)
    s11->paramTypes[0] = TYPE_INT | TYPE_FLOAT | TYPE_UNKNOWN;
}
