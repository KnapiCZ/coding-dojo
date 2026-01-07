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

#ifndef PARSER_H
#define PARSER_H

#define ASTSTACK_MAX 100

#include <stdbool.h>
#include <stdio.h>

#include "scanner.h"
#include "symtable.h"

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Forward declaration                                   */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

typedef struct ListItem ListItem;
typedef struct AstNode AstNode;

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Single Linked List Structs                            */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

/**
 * @brief Item of list with pointer to AST node and next item of list
 *
 */
typedef struct ListItem
{
    AstNode* item;
    ListItem* next;
} ListItem;

/**
 * @brief List for saving nodes as AST children in certain node types
 *
 */
typedef struct
{
    long count;
    ListItem* firstItem;
    ListItem* lastItem;
} SLList;

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Abstract Syntactic Tree (AST) Structs                 */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

/**
 * @brief Types of nodes in AST
 *
 */
typedef enum
{
    AST_CLASS_DEC,
    AST_VAR_DEC,
    AST_VAR_ASSIGN,
    AST_RETURN,
    AST_LITERAL,
    AST_TYPE,
    AST_IDENTIFIER,
    AST_EXPRESSION,
    AST_FUN_DEC,
    AST_FUN_SET,
    AST_FUN_GET,
    AST_FUN_CALL,
    AST_IF_STMT,
    AST_IF_ELSE_STMT,
    AST_ELSE_STMT,
    AST_WHILE,
    AST_IFJ,
    AST_BLOCK,
    AST_PARAMS,
    AST_OPERATOR
} AstNodeType;

/**
 * @brief Default type of node
 *
 */
typedef struct AstNode
{
    AstNodeType type;  // Type of Node in AST
    Token token;       // Token
    void* children;    // Pointer to Children of AST Binary or with 3 or more
    ExprType expressionType;
} AstNode;

/**
 * @brief Node data used for Bin Trees
 *
 */
typedef struct
{
    AstNode* right;  // Subtree of condition
    AstNode* left;   // Body of if statment
} AstBin;

/**
 * @brief Node data used for Trees with x children
 *
 */
typedef struct
{
    SLList* childrenList;  // Body of Block
} AstN;

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Parser Struct                                         */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

typedef struct
{
    Scanner* scanner;
    SymTableStack* symStack;
    Token current;
    Token lookAhead;
    AstNode* root;
    SLList* resolveLater;
} Parser;

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Stack && PRECEDENCE                                         */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

typedef struct
{
    AstNode** array;  // pole ukazatelů na AST uzly
    int topIndex;     // index vrcholu zásobníku
    int capacity;     // aktuální kapacita
} AstStack;

typedef enum
{
    PREC_ID,         // identifier / literal
    PREC_IS,         // is (typ checking / comparison)
    PREC_TYPE,       // TYPE token
    PREC_LPAREN,     // (
    PREC_RPAREN,     // )
    PREC_MULDIV,     // *
    PREC_PLUSMINUS,  // +
    PREC_CMP,        // >= < > <=
    PREC_EQ,         // == !=
    PREC_AND,        // &&
    PREC_OR,         // ||
    PREC_NOT,        // !
    PREC_END,        // special symbol for $ / end of input
    NUM_PRECEDENCE
} PrecedenceSymbol;

// Table actions: -1 = shift, 0 = error, 1 = reduce
extern const int PRECEDENCE_TABLE[NUM_PRECEDENCE][NUM_PRECEDENCE];

PrecedenceSymbol getPrecedenceSymbol(TokenType tokenType);

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Parser functions                                      */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

/**
 * @brief Initialization of parser struct
 *
 * @param parser is pointer to uninitialized struct Parser
 * @param scanner pointer to initialized struct Scanner
 */
Parser* parserInit(Scanner* scanner, SymTableStack* symStack, SLList* resolveLater);
/**
 * @brief load of next token to parser
 *
 * @param parser - pointer to initialized struct Parser
 */
void parserAdvance(Parser* parser);

/**
 * @brief Validates if curent token has correct type
 *
 * @param type is TokenType that we want to validate
 */
void parserValidate(Parser* parser, TokenType type, char* errorMessage);

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* SLL Functions                                         */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

SLList* listInit();
void listDispose(SLList* list);

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* AST Functions                                         */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

AstNode* astInit(void);
void astDispose(AstNode* tree);
AstNode* astCreateNode(AstNodeType type);

//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* AST STACK FOR EXPRESSIONS                             */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

void astStackInit(AstStack* stack, int capacity);
void astStackDispose(AstStack* stack);
bool astStackIsEmpty(AstStack* stack);
bool astStackIsFull(const AstStack* stack);
AstNode* astStackTop(AstStack* stack);
AstNode* astStackPop(AstStack* stack);
AstNode* astStackNextOp(AstStack* stack);
void astStackPush(AstStack* stack, AstNode* node);
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */
//* Parser Gramatic Functions                             */
//* ----------------------------------------------------- */
//* ----------------------------------------------------- */

void parseProgram(Parser* p);
void parseStList(Parser* parser, SLList* list, Symbol* funSym);
AstNode* parseBlock(Parser* parser, Symbol* funSym);
AstNode* parseClassDec(Parser* p);
AstNode* parseStatement(Parser* p);
AstNode* parseFunDec(Parser* p);
AstNode* parseFunCall(Parser* parser);
AstNode* parseParams(Parser* p);
AstNode* parseIfj(Parser* p);
AstNode* parseIfjTail(Parser* p);
AstNode* parseVarDec(Parser* p);
AstNode* parseVarAssign(Parser* p);
AstNode* parseReturn(Parser* parser, Symbol* funSym);
AstNode* parseReturnTail(Parser* p);
AstNode* parseIf(Parser* parser, Symbol* funSym);
AstNode* parseIfElse(Parser* parser, Symbol* funSym);
AstNode* parseElse(Parser* p, Symbol* funSym);
AstNode* parseExp(Parser* p);
AstNode* parseWhile(Parser* p, Symbol* funSym);

void parseIfChain(Parser* p, SLList* list, Symbol* funSym);
/* ----------------------------------------------------- */
/* Precedence                                            */
/* ----------------------------------------------------- */

AstNode* reduceExpression(AstStack* stack);

/* ------------------------------------------------------ */
/* RESOLVE LATER EXPRESSIONS AND STUFF                    */
/* ------------------------------------------------------ */

void parserResolveLater(SLList list);
void loadIFJBuiltins(SymTableStack* stack);

#endif  // PARSER_H