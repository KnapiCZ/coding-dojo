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

#ifndef UTILS_H
#define UTILS_H

#define ASTSTACK_MAX 100

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "parser.h"
#include "scanner.h"

const char* tokenTypeStr(TokenType type);
void printTokenValue(Token* token);
void printToken(Token* token);
void printASTTree(AstNode* node, int level, int isLast, int* prefix);
const char* astNodeTypeName(AstNodeType type);

void emitIndent(int indent);
void emit(const char* fmt, ...);
void emitLine(const char* fmt, ...);

void printSymTableStack(SymTableStack* stack);
void printSymTableGrid(SymTableStack* stack);

#endif  // UTILS_H