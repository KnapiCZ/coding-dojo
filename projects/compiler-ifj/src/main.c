/**
 * @file main.c
 * @author Filip Knapo
 * @brief This is main file of compiler.
 * @version 0.1
 * @date 2025-11-05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include "semantic.h"
#include "utils.h"

int main(int argc, char const* argv[])
{
    /* =============================================================
       LEX TEST MODE (for run_lex_tests.sh)
       ./compiler --test-scanner < input
       ============================================================= */
    if (argc == 2 && strcmp(argv[1], "--test-scanner") == 0)
    {
        Scanner scanner;
        initScanner(&scanner, stdin);

        Token token;
        int res;

        while (1)
        {
            res = getNextToken(&scanner, &token);
            if (res != 0)
            {
                // lexical error
                freeToken(&token);
                disposeScanner(&scanner);
                return LEXICAL_ERROR;  // = 1
            }

            if (token.type == EOF_TOKEN)
            {
                freeToken(&token);
                disposeScanner(&scanner);
                return 0;  // OK
            }

            // Pre lex testy netlačíme nič (výstup ide do /dev/null)
            freeToken(&token);
        }
    }

    /* =============================================================
       NORMAL USER MODE: ./compiler  (stdin)
       ============================================================= */
    if (argc == 1)
    {
        Scanner* scanner = malloc(sizeof(Scanner));
        if (!scanner)
        {
            errorExit(INTERNAL_ERROR, "Failed to allocate scanner", 0, NULL);
        }

        FILE* source = stdin;
        SLList* resolveLater = listInit();
        if (!resolveLater)
        {
            free(scanner);
            errorExit(INTERNAL_ERROR, "Failed to create symbol table", 0, NULL);
        }

        SymTableStack* symStack = symTableStackCreate(INITIAL_CAPACITY_STACK);
        if (!symStack)
        {
            free(scanner);
            listDispose(resolveLater);
            errorExit(INTERNAL_ERROR, "Failed to create symbol table", 0, NULL);
        }
        symTableStackPush(symStack);

        initScanner(scanner, source);

        // Prológ import "ifj25" for Ifj
        if (readPrologue(scanner) != 0)
        {
            disposeScanner(scanner);
            free(scanner);
            freeSymTableStack(symStack);
            errorExit(SYNTAX_ERROR, "Invalid prologue", 0, NULL);
        }

        Parser* parser = parserInit(scanner, symStack, resolveLater);
        if (!parser)
        {
            disposeScanner(scanner);
            free(scanner);
            freeSymTableStack(symStack);
            errorExit(INTERNAL_ERROR, "Failed to init parser", 0, NULL);
        }
        loadIFJBuiltins(symStack);
        parseProgram(parser);
        AstNode* temp = astCreateNode(AST_BLOCK);
        ((AstN*)temp->children)->childrenList = resolveLater;
        symTableStackPush(symStack);
        semanticResolveCheckLater(resolveLater, parser->symStack);

        checkFunDec(symStack->scopes[0]);

        if (parser->root)
        {
            // Kódovanie do IFJcode25
            generate(parser->root, symStack);
            astDispose(parser->root);
            parser->root = NULL;  // Zabrániť dvojitému uvoľneniu
        }

        if (resolveLater)
        {
            ListItem* current = resolveLater->firstItem;
            while (current)
            {
                ListItem* next = current->next;
                free(current);
                current = next;
            }
            free(resolveLater);
        }
        symTableStackPop(symStack);
        freeSymTableStack(symStack);
        free(parser);
        disposeScanner(scanner);
        free(scanner);

        return 0;
    }

    /* =============================================================
       TEST MODES WITH FILE: ./compiler --flag file
       ============================================================= */
    if (argc == 3)
    {
        Scanner* scanner = malloc(sizeof(Scanner));
        if (!scanner)
        {
            errorExit(INTERNAL_ERROR, "Failed to allocate scanner", 0, NULL);
        }

        FILE* source = fopen(argv[argc - 1], "r");
        if (!source)
        {
            free(scanner);
            errorExit(INTERNAL_ERROR, "Couldnt open source file", 0, NULL);
        }

        initScanner(scanner, source);

        /* ---------------------- Scanner test ---------------------- */
        if (strcmp(argv[argc - 2], "--test-scanner") == 0)
        {
            // Tu chceme otestovať scanner na celom programe s korektným prologom
            if (readPrologue(scanner) != 0)
            {
                disposeScanner(scanner);
                free(scanner);
                fclose(source);
                errorExit(SYNTAX_ERROR, "Invalid prologue", 0, NULL);
            }

            Token* token = malloc(sizeof(Token));
            if (!token)
            {
                disposeScanner(scanner);
                free(scanner);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to allocate token", 0, NULL);
            }

            while (1)
            {
                if (getNextToken(scanner, token) != 0)
                {
                    int errorLine = scanner->line;
                    freeToken(token);
                    free(token);
                    disposeScanner(scanner);
                    free(scanner);
                    fclose(source);
                    errorExit(LEXICAL_ERROR, "Lexical error", errorLine, NULL);
                }

                // printToken(token);

                if (token->type == EOF_TOKEN) break;

                freeToken(token);
            }

            freeToken(token);
            free(token);
            disposeScanner(scanner);
            free(scanner);
            fclose(source);
            return 0;
        }

        /* ---------------------- Parser test ---------------------- */
        else if (strcmp(argv[argc - 2], "--test-parser") == 0)
        {
            SymTableStack* symStack = symTableStackCreate(INITIAL_CAPACITY_STACK);

            symTableStackPush(symStack);

            if (!symStack)
            {
                disposeScanner(scanner);
                free(scanner);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to create symtable", 0, NULL);
            }

            if (readPrologue(scanner) != 0)
            {
                disposeScanner(scanner);
                free(scanner);
                freeSymTableStack(symStack);
                fclose(source);
                errorExit(SYNTAX_ERROR, "Invalid prologue", 0, NULL);
            }
            SLList* resolveLater = NULL;
            Parser* parser = parserInit(scanner, symStack, resolveLater);
            if (!parser)
            {
                disposeScanner(scanner);
                free(scanner);
                freeSymTableStack(symStack);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to init parser", 0, NULL);
            }

            // int prefix[128] = {0};
            parseProgram(parser);

            if (parser->root)
            {
                // printASTTree(parser->root, 0, 0, prefix);
                astDispose(parser->root);
                parser->root = NULL;  // Zabrániť dvojitému uvoľneniu
            }

            freeSymTableStack(symStack);
            free(parser);
            disposeScanner(scanner);
            free(scanner);
            fclose(source);
            return 0;
        }

        /* ---------------------- Codegen test ---------------------- */
        else if (strcmp(argv[argc - 2], "--test-codegen") == 0)
        {
            SymTableStack* symStack = symTableStackCreate(INITIAL_CAPACITY_STACK);

            symTableStackPush(symStack);

            if (!symStack)
            {
                disposeScanner(scanner);
                free(scanner);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to create symtable", 0, NULL);
            }

            if (readPrologue(scanner) != 0)
            {
                disposeScanner(scanner);
                free(scanner);
                freeSymTableStack(symStack);
                fclose(source);
                errorExit(SYNTAX_ERROR, "Invalid prologue", 0, NULL);
            }

            SLList* resolveLater = listInit();

            if (!resolveLater)
            {
                disposeScanner(scanner);
                free(scanner);
                freeSymTableStack(symStack);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to init list", 0, NULL);
            }

            Parser* parser = parserInit(scanner, symStack, resolveLater);
            if (!parser)
            {
                listDispose(resolveLater);
                disposeScanner(scanner);
                free(scanner);
                freeSymTableStack(symStack);
                fclose(source);
                errorExit(INTERNAL_ERROR, "Failed to init parser", 0, NULL);
            }

            // Načítanie vstavaných funkcií
            loadIFJBuiltins(symStack);

            // int prefix[128] = {0};
            parseProgram(parser);

            semanticResolveCheckLater(resolveLater, parser->symStack);
            checkFunDec(symStack->scopes[0]);

            if (parser->root)
            {
                // printASTTree(parser->root, 0, 1, prefix);
                generate(parser->root, symStack);
                astDispose(parser->root);
                parser->root = NULL;  // Zabrániť dvojitému uvoľneniu
            }

            if (resolveLater)
            {
                ListItem* current = resolveLater->firstItem;
                while (current)
                {
                    ListItem* next = current->next;
                    free(current);  // Uvoľníme len ListItem, nie current->item (ten je už free)
                    current = next;
                }
                free(resolveLater);
            }

            freeSymTableStack(symStack);
            free(parser);
            disposeScanner(scanner);
            free(scanner);
            fclose(source);
            return 0;
        }

        /* ---------------------- Unknown flag ---------------------- */
        else
        {
            disposeScanner(scanner);
            free(scanner);
            fclose(source);
            errorExit(INTERNAL_ERROR, "Unknown option", 0, NULL);
        }
    }

    /* =============================================================
       INVALID ARGUMENT COUNT
       ============================================================= */
    errorExit(INTERNAL_ERROR, "Invalid number of arguments", 0, NULL);
    return 0;
}
