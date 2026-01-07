/**
 * @file codegen.h
 * @author Martin Dolníček (xdolnim00)
 * @brief Code generator for Ifjcode25 from Abstract Syntax Tree
 * @version 0.1
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "symtable.h"

typedef struct
{
    SymTableStack *symStack;
    int labelCounter;    // Globální čítač pro unikátní LABELy
    int tempVarCounter;  // Čítač pro dočasné proměnné
    bool inFunction;     // Jestli právě generujeme funkci
    int frameDepth;      // 0 = global, 1 = first PUSHFRAME, …
} CodeGenerator;

/**
 * @brief Initialize CodeGenerator.
 * @param codeGen Pointer to uninitialised CodeGenerator.
 */
void codeGeneratorInit(CodeGenerator *codeGen, SymTableStack *symStack);

/**
 * @brief Start generating output code.
 * @param astRoot Pointer to initialised AST node representing root node of AST.
 */
void generate(AstNode *astRoot, SymTableStack *symStack);

/**
 * @brief Dispatches code generation for a given AST node.
 * @param node Pointer to the AST node to be processed.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genNode(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a class declaration node (AST_CLASS_DEC).
 * @param node Pointer to the AST node representing a class declaration.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genClassDec(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a variable declaration node (AST_VAR_DEC).
 * @param node Pointer to the AST node representing a variable declaration.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genVarDec(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a variable assignment node (AST_VAR_ASSIGN).
 * @param node Pointer to the AST node representing a variable assignment.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genVarAssign(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a return statement node (AST_RETURN).
 * @param node Pointer to the AST node representing a return statement.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genReturn(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a literal node (AST_LITERAL).
 * @param node Pointer to the AST node representing a literal value (int, float, string, bool,
 * etc.).
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genLiteral(AstNode *node);

/**
 * @brief Generates code for a type node (AST_TYPE).
 * @param node Pointer to the AST node representing a data type.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genType(AstNode *node);

/**
 * @brief Generates code for an identifier node (AST_IDENTIFIER).
 * @param node Pointer to the AST node representing an identifier (variable, function, etc.).
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genIdentifier(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a function declaration node (AST_FUN_DEC).
 * @param node Pointer to the AST node representing a function declaration.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genFunDec(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a function setter node (AST_FUN_SET).
 * @param node Pointer to the AST node representing a setter function.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genFunSet(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a function getter node (AST_FUN_GET).
 * @param node Pointer to the AST node representing a getter function.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genFunGet(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a function call node (AST_FUN_CALL).
 * @param node Pointer to the AST node representing a function call.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genFunCall(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for an if statement node (AST_IF_STMT).
 * @param node Pointer to the AST node representing an if statement.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genIfStmt(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for an if-else statement node (AST_IF_ELSE_STMT).
 * @param node Pointer to the AST node representing an if-else statement.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genIfElseStmt(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for an else statement node (AST_ELSE_STMT).
 * @param node Pointer to the AST node representing an else statement.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genElseStmt(AstNode *node, CodeGenerator *codeGen);

void genWhileStmt(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for an IFJ built-in function node (AST_IFJ).
 * @param node Pointer to the AST node representing an IFJ built-in function (e.g., Ifj.write).
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genIfj(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a block node (AST_BLOCK).
 * @param node Pointer to the AST node representing a block of statements.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genBlock(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for a parameters node (AST_PARAMS).
 * @param node Pointer to the AST node representing a list of parameters in a function or call.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genParams(AstNode *node, CodeGenerator *codeGen);

/**
 * @brief Generates code for an operator node (AST_OPERATOR).
 * @param node Pointer to the AST node representing an operator (+, -, *, /, <, >, etc.).
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genOperator(AstNode *node, CodeGenerator *codeGen);

/**
 * Chooses the right structure (BinTree or SLList) for the AstNodeType.
 * @brief Access and call code_gen on all children of AstNode.
 * @param astRoot Pointer to initialised AST node.
 * @param codeGen Pointer to initialised CodeGenerator struct.
 */
void genNodeChildren(AstNode *node, CodeGenerator *codeGen);

#endif  // CODEGEN_H