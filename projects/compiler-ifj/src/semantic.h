/**
 * @file semantic.h
 * @author Filip Knapo (you@domain.com)
 * @brief this file is used for checking types during expressions and other values
 * @version 0.1
 * @date 2025-11-14
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdbool.h>
#include <stdio.h>

#include "parser.h"
#include "scanner.h"

typedef enum
{
    MULTIPLICATION,
    DIVISION,
    ADDITION,
    COMPARISON

} operationType;
void semanticExpression(AstNode* node);
void semanticAssignment(AstNode* node);
void semanticResolveCheckLater(SLList* checkLaterList, SymTableStack* stack);
bool checkFunDec(Scope* globalScope);
#endif  // SEMANTIC_H