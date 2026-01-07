/**
 * @file codegen.h
 * @author Martin Dolníček (xdolnim00)
 * @author Samuel Vajda (xvajdas00)
 * @brief Code generator for Ifjcode25 from Abstract Syntax Tree
 * @version 0.1
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "codegen.h"

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "utils.h"

/**
 * @brief Initialize code generator
 */
void codeGeneratorInit(CodeGenerator *codeGen, SymTableStack *symStack)
{
    codeGen->symStack = symStack;
    codeGen->frameDepth = 0;
    codeGen->inFunction = false;
    codeGen->labelCounter = 0;
    codeGen->tempVarCounter = 0;
}

/**
 * @brief Generate unique label
 * E.g. Base = "if" -> "if_0", "if_1"...
 * @param gen Pointer to code generator state
 * @param base Base for label name
 * @return Newly allocated string with unique label (caller must free)
 */
char *genUniqueLabel(CodeGenerator *gen, const char *base)
{
    char *label = malloc(64);
    if (!label)
    {
        errorExit(INTERNAL_ERROR, "Failed to allocate label", 0, NULL);
    }

    char num[16];
    sprintf(num, "%d", gen->labelCounter++);  // conversion int → string

    strcpy(label, "$");   // IFJcode25 label must start with $
    strcat(label, base);  // add base
    strcat(label, "_");   // add "_"
    strcat(label, num);   // add number

    return label;
}

/**
 * @brief Generate temporary variable
 * E.g. $$tmp0, $$tmp1 ...
 * @param gen Pointer to code generator state
 */
char *genTempVar(CodeGenerator *gen)
{
    char *var = malloc(32);
    if (!var)
    {
        errorExit(INTERNAL_ERROR, "Failed to allocate temporary variable", 0, NULL);
    }
    // Same algorithm as for label
    char num[16];
    sprintf(num, "%d", gen->tempVarCounter++);

    strcpy(var, "$$tmp_");
    strcat(var, num);

    return var;
}

/**
 * @brief Start generating output code.
 * @param astRoot Pointer to initialised AST node representing root node of AST.
 */
void generate(AstNode *astRoot, SymTableStack *symStack)
{
    CodeGenerator codeGen;
    codeGeneratorInit(&codeGen, symStack);

    // AstRoot odkazuje na class Program (jediná class celého souboru?)
    genNode(astRoot, &codeGen);
}

/**
 * @brief Dispatches code generation for a given AST node.
 * @param node Pointer to the AST node to be processed.
 */
void genNode(AstNode *node, CodeGenerator *codeGen)
{
    switch (node->type)
    {
        case AST_CLASS_DEC:
            genClassDec(node, codeGen);
            break;

        case AST_BLOCK:
            genBlock(node, codeGen);
            break;

        case AST_VAR_DEC:
            genVarDec(node, codeGen);
            break;

        case AST_FUN_DEC:
            genFunDec(node, codeGen);
            break;

        case AST_FUN_SET:
            genFunSet(node, codeGen);
            break;

        case AST_FUN_GET:
            genFunGet(node, codeGen);
            break;

        case AST_RETURN:
            genReturn(node, codeGen);
            break;

        case AST_PARAMS:
            genParams(node, codeGen);
            break;

        case AST_VAR_ASSIGN:
            genVarAssign(node, codeGen);
            break;

        case AST_IDENTIFIER:
            genIdentifier(node, codeGen);
            break;

        case AST_LITERAL:
            genLiteral(node);
            break;

        case AST_OPERATOR:
            genOperator(node, codeGen);
            break;

        case AST_FUN_CALL:
            genFunCall(node, codeGen);
            break;

        case AST_IF_STMT:
            genIfStmt(node, codeGen);
            break;

        case AST_IF_ELSE_STMT:
            genIfElseStmt(node, codeGen);
            break;

        case AST_IFJ:
            genIfj(node, codeGen);
            break;

        case AST_ELSE_STMT:
            genElseStmt(node, codeGen);
            break;

        case AST_WHILE:
            genWhileStmt(node, codeGen);
            break;

        case AST_TYPE:
            genType(node);
            break;

        case AST_EXPRESSION:
            genOperator(node, codeGen);

        default:
            break;
    }
}

/**
 * @brief Generates code for a class declaration node (AST_CLASS_DEC).
 * @param node Pointer to the AST node representing a class declaration.
 */
void genClassDec(AstNode *node, CodeGenerator *codeGen)
{
    emitLine(".IFJcode25");
    emitLine("JUMP $$main_entry");  // Skok přes funkce

    // 1. Generuj VŠECHNY funkce (včetně main)
    if (node->children)
    {
        AstN *children = (AstN *)node->children;
        if (children->childrenList)
        {
            ListItem *item = children->childrenList->firstItem;
            while (item)
            {
                if (item->item->type == AST_FUN_DEC || item->item->type == AST_FUN_GET ||
                    item->item->type == AST_FUN_SET)
                {
                    genNode(item->item, codeGen);
                }
                item = item->next;
            }
        }
    }

    // 2. Vstupní bod programu
    emitLine("\nLABEL $$main_entry");

    // 3. Definuj globální proměnné
    if (codeGen->symStack && codeGen->symStack->top >= 0)
    {
        Scope *globalScope = codeGen->symStack->scopes[0];
        for (int i = 0; i < globalScope->size; i++)
        {
            Symbol *sym = globalScope->symbols[i];
            if (sym && sym != TOMBSTONE && sym->kind == SYM_VAR)
            {
                emitLine("DEFVAR GF@%s", sym->name);
                emitLine("MOVE GF@%s nil@nil", sym->name);
            }
        }
    }

    // 4. Globální přiřazení
    if (node->children)
    {
        AstN *children = (AstN *)node->children;
        if (children->childrenList)
        {
            ListItem *item = children->childrenList->firstItem;
            while (item)
            {
                if (item->item->type != AST_FUN_DEC && item->item->type != AST_FUN_GET &&
                    item->item->type != AST_FUN_SET && item->item->type != AST_VAR_DEC)
                {
                    genNode(item->item, codeGen);
                }
                item = item->next;
            }
        }
    }

    // 5. Zavolej main a ukonči
    emitLine("CALL main$0");
    emitLine("EXIT int@0");
}

/**
 * @brief Generates code for a block node (AST_BLOCK).
 * @param node Pointer to the AST node representing a block of statements.
 */
void genBlock(AstNode *node, CodeGenerator *codeGen)
{
    genNodeChildren(node, codeGen);
}

/**
 * @brief Generates code for a variable declaration node (AST_VAR_DEC).
 * @param node Pointer to the AST node representing a variable declaration.
 */
void genVarDec(AstNode *node, CodeGenerator *codeGen)
{
    // AST_VAR_DEC nikdy nebude mít děti, je to list

    if (node == NULL) return;

    if (node->token.type == GLOBAL_IDENTIFIER)
    {
        emitLine("MOVE GF@%s nil@nil", node->token.value->stringVal);
    }

    else if (node->token.type == IDENTIFIER)
    {
        emitLine("DEFVAR LF@%s$%d", node->token.value->stringVal, codeGen->frameDepth);
        emitLine("MOVE LF@%s$%d nil@nil", node->token.value->stringVal, codeGen->frameDepth);
    }
}

/**
 * @brief Generates code for a variable assignment node (AST_VAR_ASSIGN).
 * @param node Pointer to the AST node representing a variable assignment.
 */
void genVarAssign(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;

    AstBin *data = (AstBin *)node->children;
    AstNode *left = data->left;    // Identifier
    AstNode *right = data->right;  // Expression

    // Najprv vyhodnoť výraz (výsledok bude na zásobníku)
    genNode(right, codeGen);

    char *name = left->token.value->stringVal;
    Symbol *sym = symTableStackFindSymbol(codeGen->symStack, name);

    if (sym && sym->kind == SYM_SET)
    {
        emitLine("CALL %s_set", name);
    }
    else if (left->token.type == GLOBAL_IDENTIFIER)
    {
        emitLine("POPS GF@%s\n", name);
    }
    else
    {
        emitLine("POPS LF@%s$%d\n", name, codeGen->frameDepth);
    }
}

/**
 * @brief Generates code for a function declaration node (AST_FUN_DEC).
 * @param node Pointer to the AST node representing a function declaration.
 */
void genFunDec(AstNode *node, CodeGenerator *codeGen)
{
    // Verze s předáváním parametrů na zásobníku
    // Konvence volání
    //      Na zásobník předává zleva doprava
    //      Návratová hodnota na zásobníku
    codeGen->frameDepth += 1;
    codeGen->inFunction = true;

    int paramCount = 0;
    AstBin *nodeChildren = (AstBin *)node->children;

    if (nodeChildren->left)
    {
        // PARAMS
        AstN *paramsChildren = (AstN *)nodeChildren->left->children;
        SLList *paramList = paramsChildren->childrenList;
        ListItem *currentItem = paramList->firstItem;

        while (currentItem != NULL)
        {
            paramCount += 1;
            currentItem = currentItem->next;
        }
    }

    // Generujeme náveštie s počtom parametrov
    emitLine("\nLABEL %s$%d", node->token.value->stringVal, paramCount);

    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME\n");

    genNodeChildren(node, codeGen);

    //  Pokud funkce nemá return, tak vygeneruj jen RETURN
    //  Funkce má return, vygeneruje jej až se dostane k jeji node
    if (codeGen->inFunction)
    {
        emitLine("PUSHS nil@nil");
        emitLine("POPFRAME");
        emitLine("RETURN\n");
        codeGen->inFunction = false;
        codeGen->frameDepth -= 1;
    }

    // Generování funkce main končí ukončením s návratovou hodnotou 0 (neověřená informace)
    if (strcmp(node->token.value->stringVal, "main") == 0 && paramCount == 0)
    {
        emitLine("EXIT int@0");
    }
}

/**
 * @brief Generates code for a return statement node (AST_RETURN).
 * @param node Pointer to the AST node representing a return statement.
 */
void genReturn(AstNode *node, CodeGenerator *codeGen)
{
    AstBin *bin = (AstBin *)node->children;
    if (bin && bin->right)
    {
        // Vygeneruje kód pre výraz -> výsledok ostane na vrchole zásobníka
        genNode(bin->right, codeGen);
    }
    else
    {
        // Void return -> vráť nil
        emitLine("PUSHS nil@nil");
    }

    emitLine("POPFRAME");
    emitLine("RETURN\n");
}

/**
 * @brief Generates code for a literal node (AST_LITERAL).
 * @param node Pointer to the AST node representing a literal value (int, float, string, bool,
 * etc.).
 */
void genLiteral(AstNode *node)  // vratit code gen
{
    if (node == NULL) return;

    switch (node->token.type)
    {
        case INT_LITERAL:
            emitLine("PUSHS int@%lld", node->token.value->intVal);
            break;

        case FLOAT_LITERAL:
            emitLine("PUSHS float@%a", node->token.value->floatVal);
            break;

        case STRING_LITERAL:
        case MULTILINE_STRING_LITERAL:
        {
            char *str = node->token.value->stringVal;
            unsigned int len = strlen(str);

            // (worst case: každý znak = 4 znaky escaped)
            char *buffer = malloc(len * 4 + 1);
            if (!buffer)
            {
                errorExit(INTERNAL_ERROR, "Failed to allocate buffer for string", 0, NULL);
            }

            unsigned int pos = 0;
            for (unsigned int i = 0; str[i] != '\0'; i++)
            {
                unsigned char c = (unsigned char)str[i];

                // Znaky <= 32, 35 (#), 92 (\) musia byť escaped
                if (c <= 32 || c == 35 || c == 92)
                {
                    pos += sprintf(&buffer[pos], "\\%03d", c);
                }
                else
                {
                    buffer[pos++] = c;
                }
            }
            buffer[pos] = '\0';

            emitLine("PUSHS string@%s", buffer);

            free(buffer);
            break;
        }

        case KW_VAL_NULL:
            emitLine("PUSHS nil@nil");
            break;

        case KW_VAL_TRUE:
            emitLine("PUSHS bool@true");
            break;

        case KW_VAL_FALSE:
            emitLine("PUSHS bool@false");
            break;

        default:
            break;
    }
}

/**
 * @brief Generates code for a type node (AST_TYPE).
 * @param node Pointer to the AST node representing a data type.
 */
void genType(AstNode *node)
{
    if (node == NULL) return;

    switch (node->token.type)
    {
        case KW_TYPE_NUM:
            emitLine("PUSHS string@float");
            break;
        case KW_TYPE_STRING:
            emitLine("PUSHS string@string");
            break;
        case KW_TYPE_BOOL:
            emitLine("PUSHS string@bool");
            break;
        case KW_TYPE_NULL:
        case KW_VAL_NULL:
            emitLine("PUSHS string@nil");
            break;
        default:
            break;
    }
}

/**
 * @brief Generates code for an identifier node (AST_IDENTIFIER).
 * @param node Pointer to the AST node representing an identifier (variable, function, etc.).
 */
void genIdentifier(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;

    char *name = node->token.value->stringVal;

    // Zistíme, či je to Getter pomocou SymTable
    Symbol *sym = symTableStackFindSymbol(codeGen->symStack, name);

    if (sym && sym->kind == SYM_GET)
    {
        // JE TO GETTER -> Voláme funkciu
        // Predpokladám, že getter sa v IFJcode volá "meno_get" (podľa genFunGet)
        emitLine("CALL %s_get", name);
        // Výsledok je na zásobníku (z returnu funkcie), takže nemusíme robiť PUSHS
    }
    else if (node->token.type == GLOBAL_IDENTIFIER)
    {
        emitLine("PUSHS GF@%s", name);
    }
    else
    {
        emitLine("PUSHS LF@%s$%d", name, codeGen->frameDepth);
    }
}

/**
 * @brief Generates code for a function setter node (AST_FUN_SET).
 * @param node Pointer to the AST node representing a setter function.
 */
void genFunSet(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;

    codeGen->frameDepth += 1;
    codeGen->inFunction = true;

    // Setter má formát: static name =(param) { ... }
    // Generujeme label s príponou "_set"
    emitLine("\nLABEL %s_set", node->token.value->stringVal);
    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME\n");

    genNodeChildren(node, codeGen);

    // Ak nie je explicitný return, vráť nil
    emitLine("PUSHS nil@nil");
    emitLine("POPFRAME");
    emitLine("RETURN\n");

    codeGen->inFunction = false;
    codeGen->frameDepth -= 1;
}
/**
 * @brief Generates code for a function getter node (AST_FUN_GET).
 * @param node Pointer to the AST node representing a getter function.
 */
void genFunGet(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;

    codeGen->frameDepth += 1;
    codeGen->inFunction = true;

    // Getter má formát: static name { ... }
    // Generujeme label s príponou "_get"
    emitLine("\nLABEL %s_get", node->token.value->stringVal);
    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME\n");

    genNodeChildren(node, codeGen);

    // Ak nie je explicitný return, vráť nil
    emitLine("PUSHS nil@nil");
    emitLine("POPFRAME");
    emitLine("RETURN\n");

    codeGen->inFunction = false;
    codeGen->frameDepth -= 1;
}

/**
 * @brief Generates code for a function call node (AST_FUN_CALL).
 * @param node Pointer to the AST node representing a function call.
 */
void genFunCall(AstNode *node, CodeGenerator *codeGen)
{
    // 1. Získame zoznam argumentov
    AstBin *bin = (AstBin *)node->children;
    AstNode *argsNode = bin->right;  // AST_PARAMS s argumentmi
    int argCount = 0;

    // 2. Vygenerujeme kód pre argumenty (PUSHS)
    if (argsNode)
    {
        AstN *listNode = (AstN *)argsNode->children;
        if (listNode && listNode->childrenList)
        {
            argCount = listNode->childrenList->count;

            ListItem *item = listNode->childrenList->firstItem;
            while (item)
            {
                genNode(item->item, codeGen);
                item = item->next;
            }
        }
    }

    // 3. Zavoláme funkciu so správnym počtom parametrov v náveští
    emitLine("CALL %s$%d", node->token.value->stringVal, argCount);
}

/**
 * @brief Generates code for an if statement node (AST_IF_STMT).
 * @param node Pointer to the AST node representing an if statement.
 */
void genIfStmt(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;
    if (node->children == NULL) return;

    AstBin *data = (AstBin *)node->children;
    if (data == NULL || data->left == NULL || data->right == NULL) return;

    char *labelEnd = genUniqueLabel(codeGen, "if_end");

    emitLine("# If statement");

    // Vyhodnoť podmienku
    genNode(data->left, codeGen);

    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME");

    // Ulož výsledok do LOKÁLNEJ dočasnej premennej (Oprava chyby interpreteru)
    char *condRes = genTempVar(codeGen);
    emit("DEFVAR LF@%s$%d\n", condRes, codeGen->frameDepth);
    emit("POPS LF@%s$%d\n", condRes, codeGen->frameDepth);

    char *labelSkip = genUniqueLabel(codeGen, "if_skip");

    // Skontroluj podmienku (false alebo nil znamená nepravda)
    emit("JUMPIFEQ %s LF@%s$%d bool@false\n", labelSkip, condRes, codeGen->frameDepth);
    emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", labelSkip, condRes, codeGen->frameDepth);

    emitLine("POPFRAME");

    // Telo if
    genNode(data->right, codeGen);

    emit("JUMP %s\n", labelEnd);

    // --- VETVA FALSE/SKIP ---
    emit("LABEL %s\n", labelSkip);
    emitLine("POPFRAME");  // Uvoľnenie rámca aj v prípade skoku

    emitLine("LABEL %s", labelEnd);

    free(condRes);
    free(labelEnd);
    free(labelSkip);
}

/**
 * @brief Generates code for an if-else statement node (AST_IF_ELSE_STMT).
 * @param node Pointer to the AST node representing an if-else statement.
 */
void genIfElseStmt(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;
    if (node->children == NULL) return;

    AstBin *data = (AstBin *)node->children;
    if (data == NULL || data->left == NULL || data->right == NULL) return;

    AstNode *cond = data->left;

    AstNode *blocksContainer = data->right;
    if (blocksContainer == NULL || blocksContainer->children == NULL) return;

    AstBin *blocks = (AstBin *)blocksContainer->children;
    if (blocks == NULL) return;

    AstNode *trueBlock = blocks->left;
    AstNode *falseBlock = blocks->right;

    char *labelElse = genUniqueLabel(codeGen, "if_else");
    char *labelEnd = genUniqueLabel(codeGen, "if_end");

    genNode(cond, codeGen);

    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME");

    // Kontrola (nil alebo false -> skok na else)
    char *condRes = genTempVar(codeGen);
    emit("DEFVAR LF@%s$%d\n", condRes, codeGen->frameDepth);
    emit("POPS LF@%s$%d\n", condRes, codeGen->frameDepth);

    char *labelFalseJump = genUniqueLabel(codeGen, "if_fail_jump");

    emit("JUMPIFEQ %s LF@%s$%d bool@false\n", labelFalseJump, condRes, codeGen->frameDepth);
    emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", labelFalseJump, condRes, codeGen->frameDepth);

    // --- TRUE VETVA ---
    emitLine("POPFRAME");
    genNode(trueBlock, codeGen);
    emit("JUMP %s\n", labelEnd);

    // --- FALSE VETVA (príprava) ---
    emit("LABEL %s\n", labelFalseJump);
    emitLine("POPFRAME");

    // --- ELSE VETVA ---
    emit("LABEL %s\n", labelElse);  // Toto návestie tu ostalo z pôvodnej logiky, v podstate sa
                                    // zlúči s labelFalseJump
    genNode(falseBlock, codeGen);

    emit("LABEL %s\n", labelEnd);

    free(condRes);
    free(labelElse);
    free(labelEnd);
    free(labelFalseJump);
}

/**
 * @brief Generates code for an else statement node (AST_ELSE_STMT).
 * @param node Pointer to the AST node representing an else statement.
 */
void genElseStmt(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;
    genNodeChildren(node, codeGen);
}

/**
 * @brief Generates code for a while statement node (AST_WHILE).
 */
void genWhileStmt(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;
    if (node->children == NULL) return;

    AstBin *data = (AstBin *)node->children;
    if (data == NULL || data->left == NULL || data->right == NULL) return;

    AstNode *cond = data->left;
    AstNode *body = data->right;

    char *labelStart = genUniqueLabel(codeGen, "while_start");
    char *labelEnd = genUniqueLabel(codeGen, "while_end");

    // Potřeba definovat tempVar před vstupem do funkce, aby nedošlo ke znovudeklaraci
    char *condRes = genTempVar(codeGen);
    emit("DEFVAR LF@%s$%d\n", condRes, codeGen->frameDepth);
    emitLine("# While loop");

    // 1. Label začiatku cyklu
    emit("LABEL %s\n", labelStart);

    // 2. Vyhodnotenie podmienky
    genNode(cond, codeGen);

    // 3. Kontrola podmienky (výsledok je na zásobníku)
    emit("POPS LF@%s$%d\n", condRes, codeGen->frameDepth);

    // Ak false alebo nil, skok na koniec
    emit("JUMPIFEQ %s LF@%s$%d bool@false\n", labelEnd, condRes, codeGen->frameDepth);
    emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", labelEnd, condRes, codeGen->frameDepth);

    free(condRes);

    // 4. Telo cyklu
    genNode(body, codeGen);

    // 5. Skok na začiatok
    emit("JUMP %s\n", labelStart);

    // 6. Label konca
    emit("LABEL %s\n", labelEnd);

    free(labelStart);
    free(labelEnd);
}

void emitTypeCheck(const char *tmp, const char *wanted, CodeGenerator *codeGen, char *endLabel)
{
    // create a temp type var
    char *typeTmp = genTempVar(codeGen);
    emit("DEFVAR LF@%s$%d\n", typeTmp, codeGen->frameDepth);
    emit("TYPE LF@%s$%d LF@%s$%d\n", typeTmp, codeGen->frameDepth, tmp, codeGen->frameDepth);
    // Compare with wanted string
    emit("JUMPIFEQ %s LF@%s$%d string@%s\n", endLabel, typeTmp, codeGen->frameDepth, wanted);
    // not equal => error
    emit("EXIT int@25\n");
    // label ok
    emit("LABEL %s\n", endLabel);
    free(typeTmp);
}

char *escapeString(const char *src)
{
    size_t len = strlen(src);
    char *out = malloc(len * 4 + 1);  // worst case — všechny znaky escapovány
    char *p = out;

    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = src[i];

        switch (c)
        {
            case '\\':
                *p++ = '\\';
                *p++ = '\\';
                break;

            case '\"':
                *p++ = '\\';
                *p++ = '"';
                break;

            case '\n':
                strcpy(p, "\\010");
                p += 4;
                break;

            case '\t':
                strcpy(p, "\\009");
                p += 4;
                break;

            default:
                if (c <= 32)
                {
                    // control characters
                    sprintf(p, "\\%03d", c);
                    p += 4;
                }
                else
                {
                    *p++ = c;
                }
                break;
        }
    }

    *p = '\0';
    return out;
}

/**
 * @brief Generate built-in Ifj.* functions with runtime checks (exit 25 on bad type)
 * @param node AST_IFJ node, node->token.value->stringVal contains function name (e.g. "write").
 * @param codeGen code generator state (used for frameDepth, temp counters)
 *
 * node->children->right should be FUN_CALL whose right child is AST_PARAMS (list)
 */
void genIfj(AstNode *node, CodeGenerator *codeGen)
{
    if (!node) return;

    // get FUN_CALL child (params)
    AstBin *bin = (AstBin *)node->children;
    AstNode *funCall = bin->right;  // should be AST_FUN_CALL
    AstNode *paramsNode = NULL;
    const char *fname = funCall->token.value->stringVal;
    if (funCall && funCall->children)
    {
        AstBin *fcbin = (AstBin *)funCall->children;
        paramsNode = fcbin->right;  // AST_PARAMS
    }

    // Collect params into array for easy indexing
    AstN *paramsN = paramsNode ? (AstN *)paramsNode->children : NULL;
    SLList *plist = paramsN ? paramsN->childrenList : NULL;

    // Count params
    long paramCount = 0;
    if (plist) paramCount = plist->count;

    // Iterate params into LF temporaries (MOVE into LF@tmp), store names
    char **tmpNames = NULL;
    if (paramCount > 0)
    {
        tmpNames = malloc(sizeof(char *) * paramCount);
        ListItem *it = plist->firstItem;
        long idx = 0;
        while (it != NULL && idx < paramCount)
        {
            AstNode *p = it->item;
            char *tmp = genTempVar(codeGen);
            // DEFVAR LF@tmp$depth
            emitLine("DEFVAR LF@%s$%d", tmp, codeGen->frameDepth);

            // Move value into temporary:
            if (p->type == AST_IDENTIFIER)
            {
                // identifier might be GF or LF
                if (p->token.type == GLOBAL_IDENTIFIER)
                    emitLine("MOVE LF@%s$%d GF@%s", tmp, codeGen->frameDepth,
                             p->token.value->stringVal);
                else
                    emitLine("MOVE LF@%s$%d LF@%s$%d", tmp, codeGen->frameDepth,
                             p->token.value->stringVal, codeGen->frameDepth);
            }
            else if (p->type == AST_LITERAL)
            {
                // Move literal directly
                switch (p->token.type)
                {
                    case INT_LITERAL:
                        emitLine("MOVE LF@%s$%d int@%lld", tmp, codeGen->frameDepth,
                                 p->token.value->intVal);
                        break;
                    case FLOAT_LITERAL:
                        emitLine("MOVE LF@%s$%d float@%a", tmp, codeGen->frameDepth,
                                 p->token.value->floatVal);
                        break;
                    case STRING_LITERAL:
                    case MULTILINE_STRING_LITERAL:
                    {
                        char *esc = escapeString(p->token.value->stringVal);
                        emitLine("MOVE LF@%s$%d string@%s", tmp, codeGen->frameDepth, esc);
                        free(esc);
                        break;
                    }
                    case KW_VAL_TRUE:
                        emitLine("MOVE LF@%s$%d bool@true", tmp, codeGen->frameDepth);
                        break;
                    case KW_VAL_FALSE:
                        emitLine("MOVE LF@%s$%d bool@false", tmp, codeGen->frameDepth);
                        break;
                    case KW_VAL_NULL:
                        emitLine("MOVE LF@%s$%d nil@nil", tmp, codeGen->frameDepth);
                        break;
                    default:
                        errorExit(INTERNAL_ERROR, "Unknown literal in IFJ call", 0, NULL);
                }
            }
            else
            {
                // If param is expression (operator, fun_call, ...), generate it and then POP into
                // tmp Generate expression onto stack
                genNode(p, codeGen);
                // After genNode we assume value on data stack -> POPS into tmp
                emitLine("POPS LF@%s$%d", tmp, codeGen->frameDepth);
            }

            tmpNames[idx++] = tmp;
            it = it->next;
        }
    }

    // Helper labels for error handling
    char *errLabel = genUniqueLabel(codeGen, "ifj_err");
    char *endLabel = genUniqueLabel(codeGen, "ifj_ok");

    // Helper to check type of LF@tmp and jump to err if mismatch
    // wantedType -> literal string ("int", "float", "string", "bool", "nil")

    // Implement builtins
    if (strcmp(fname, "write") == 0)
    {
        if (paramCount != 1) errorExit(SYNTAX_ERROR, "Ifj.write expects 1 parameter", 0, NULL);

        // simply WRITE tmp0
        emitLine("WRITE LF@%s$%d", tmpNames[0], codeGen->frameDepth);
        // write returns null
        emitLine("PUSHS nil@nil");
    }
    else if (strcmp(fname, "read_str") == 0)
    {
        if (paramCount != 0) errorExit(SYNTAX_ERROR, "Ifj.read_str expects 0 parameters", 0, NULL);
        // read_str -> READ <var> string
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);
        emitLine("READ LF@%s$%d string", res, codeGen->frameDepth);
        // push result to stack (return value)
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);
        free(res);
    }
    else if (strcmp(fname, "read_num") == 0)
    {
        if (paramCount != 0) errorExit(SYNTAX_ERROR, "Ifj.read_num expects 0 parameters", 0, NULL);
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);
        emitLine("READ LF@%s$%d float", res,
                 codeGen->frameDepth);  // read_num returns float or null
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);
        free(res);
    }
    else if (strcmp(fname, "floor") == 0)
    {
        if (paramCount != 1) errorExit(SYNTAX_ERROR, "Ifj.floor expects 1 parameter", 0, NULL);
        // Accept int or float. If int -> return int (same), if float -> FLOAT2INT
        char *in = tmpNames[0];
        char *res = genTempVar(codeGen);
        ememitLineit("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // TYPE check: if int -> MOVE result; if float -> FLOAT2INT; else EXIT 25
        char *typeTmp = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", typeTmp, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", typeTmp, codeGen->frameDepth, in, codeGen->frameDepth);
        // compare to "int"
        char *labelIsInt = genUniqueLabel(codeGen, "is_int");
        char *labelIsFloat = genUniqueLabel(codeGen, "is_float");
        char *labelDone = genUniqueLabel(codeGen, "floor_done");

        emitLine("JUMPIFEQ %s LF@%s$%d string@int", labelIsInt, typeTmp, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@float", labelIsFloat, typeTmp, codeGen->frameDepth);
        // not int or float -> exit 25
        emitLine("EXIT int@25");

        emitLine("LABEL %s", labelIsInt);
        emitLine("MOVE LF@%s$%d LF@%s$%d\n", res, codeGen->frameDepth, in, codeGen->frameDepth);
        emitLine("JUMP %s", labelDone);

        emitLine("LABEL %s", labelIsFloat);
        emitLine("FLOAT2INT LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, in, codeGen->frameDepth);

        emitLine("LABEL %s", labelDone);
        // push result
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(typeTmp);
        free(res);
    }
    else if (strcmp(fname, "str") == 0)
    {
        if (paramCount != 1) errorExit(SYNTAX_ERROR, "Ifj.str expects 1 parameter", 0, NULL);
        char *in = tmpNames[0];
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // detect type and convert accordingly: string -> MOVE, int->INT2STR, float->FLOAT2STR,
        // bool->MOVE true/false to string, nil->string@null
        char *typeTmp = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", typeTmp, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", typeTmp, codeGen->frameDepth, in, codeGen->frameDepth);

        char *labelStrDone = genUniqueLabel(codeGen, "str_done");
        char *labelIsStr = genUniqueLabel(codeGen, "is_str");
        char *labelIsInt = genUniqueLabel(codeGen, "is_int");
        char *labelIsFloat = genUniqueLabel(codeGen, "is_float");
        char *labelIsBool = genUniqueLabel(codeGen, "is_bool");
        char *labelIsNil = genUniqueLabel(codeGen, "is_nil");

        emitLine("JUMPIFEQ %s LF@%s$%d string@string", labelIsStr, typeTmp, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@int", labelIsInt, typeTmp, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@float", labelIsFloat, typeTmp, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@bool", labelIsBool, typeTmp, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@nil", labelIsNil, typeTmp, codeGen->frameDepth);
        // default
        emitLine("MOVE LF@%s$%d string@UNKNOWN", res, codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);

        emitLine("LABEL %s", labelIsStr);
        emitLine("MOVE LF@%s$%d LF@%s$%d\n", res, codeGen->frameDepth, in, codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);

        emitLine("LABEL %s", labelIsInt);
        emitLine("INT2STR LF@%s$%d LF@%s$%d\n", res, codeGen->frameDepth, in, codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);

        emitLine("LABEL %s", labelIsFloat);
        emitLine("FLOAT2STR LF@%s$%d LF@%s$%d\n", res, codeGen->frameDepth, in,
                 codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);

        emitLine("LABEL %s", labelIsBool);
        // bool -> "true"/"false"
        // create tmp bool check
        char *boolLabel = genUniqueLabel(codeGen, "bool_true");
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", boolLabel, in, codeGen->frameDepth);
        // fallback false
        emitLine("MOVE LF@%s$%d string@false", res, codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);
        emitLine("LABEL %s", boolLabel);
        emitLine("MOVE LF@%s$%d string@true", res, codeGen->frameDepth);
        emitLine("JUMP %s", labelStrDone);

        emitLine("LABEL %s", labelIsNil);
        emitLine("MOVE LF@%s$%d string@null", res, codeGen->frameDepth);

        emitLine("LABEL %s", labelStrDone);

        // push result
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(typeTmp);
        free(res);
    }
    else if (strcmp(fname, "length") == 0)
    {
        if (paramCount != 1) errorExit(SYNTAX_ERROR, "Ifj.length expects 1 parameter", 0, NULL);
        char *in = tmpNames[0];
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // require string type
        char *typeTmp = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", typeTmp, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", typeTmp, codeGen->frameDepth, in, codeGen->frameDepth);
        // if not string -> EXIT 25
        emitLine("JUMPIFEQ %s LF@%s$%d string@string", endLabel, typeTmp, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        emitLine("STRLEN LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, in, codeGen->frameDepth);
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(typeTmp);
        free(res);
    }
    else if (strcmp(fname, "strcmp") == 0)
    {
        if (paramCount != 2) errorExit(SYNTAX_ERROR, "Ifj.strcmp expects 2 parameters", 0, NULL);
        char *a = tmpNames[0];
        char *b = tmpNames[1];
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // require strings
        char *t1 = genTempVar(codeGen);
        char *t2 = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", t1, codeGen->frameDepth);
        emitLine("DEFVAR LF@%s$%d", t2, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", t1, codeGen->frameDepth, a, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", t2, codeGen->frameDepth, b, codeGen->frameDepth);
        // if not strings exit 25
        emitLine("JUMPIFEQ %s LF@%s$%d string@string", endLabel, t1, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);
        emitLine("JUMPIFEQ %s LF@%s$%d string@string", endLabel, t2, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        // Use built-in comparison: strcmp-like can be emulated with LT/GT/EQ
        // We'll compute: if a < b -> -1, if a == b -> 0, else 1
        char *labelLt = genUniqueLabel(codeGen, "strcmp_lt");
        char *labelEq = genUniqueLabel(codeGen, "strcmp_eq");
        char *labelDone = genUniqueLabel(codeGen, "strcmp_done");

        emitLine("LT LF@%s$%d LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, a, codeGen->frameDepth,
                 b, codeGen->frameDepth);
        // res now boolean; we need to branch
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", labelLt, res, codeGen->frameDepth);
        // check equality
        emitLine("EQ LF@%s$%d LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, a, codeGen->frameDepth,
                 b, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", labelEq, res, codeGen->frameDepth);
        // else greater
        emitLine("MOVE LF@%s$%d int@1", res, codeGen->frameDepth);
        emitLine("JUMP %s", labelDone);
        emitLine("LABEL %s", labelLt);
        emitLine("MOVE LF@%s$%d int@-1", res, codeGen->frameDepth);
        emitLine("JUMP %s", labelDone);
        emitLine("LABEL %s", labelEq);
        emitLine("MOVE LF@%s$%d int@0", res, codeGen->frameDepth);
        emitLine("LABEL %s", labelDone);

        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(t1);
        free(t2);
        free(res);
    }
    else if (strcmp(fname, "ord") == 0)
    {
        if (paramCount != 2) errorExit(SYNTAX_ERROR, "Ifj.ord expects 2 parameters", 0, NULL);
        char *s = tmpNames[0];
        char *i = tmpNames[1];
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // Type checks: s string, i int
        char *tt = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tt, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, s, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@string", endLabel, tt, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        char *tt2 = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tt2, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", tt2, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@int", endLabel, tt2, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        char *len = genTempVar(codeGen);
        char *cmpRes = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", len, codeGen->frameDepth);
        emitLine("DEFVAR LF@%s$%d", cmpRes, codeGen->frameDepth);

        emitLine("STRLEN LF@%s$%d LF@%s$%d", len, codeGen->frameDepth, s, codeGen->frameDepth);

        char *retZero = genUniqueLabel(codeGen, "ord_zero");
        char *doOrd = genUniqueLabel(codeGen, "ord_do");

        // i < 0
        emitLine("LT LF@%s$%d LF@%s$%d int@0", cmpRes, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", retZero, cmpRes, codeGen->frameDepth);

        // i >= len (t.j. ak NOT i < len)
        emitLine("LT LF@%s$%d LF@%s$%d LF@%s$%d", cmpRes, codeGen->frameDepth, i,
                 codeGen->frameDepth, len, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@false", retZero, cmpRes, codeGen->frameDepth);

        // OK
        emitLine("STRI2INT LF@%s$%d LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, s,
                 codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMP %s", doOrd);

        // Out of bounds
        emitLine("LABEL %s", retZero);
        emitLine("MOVE LF@%s$%d int@0", res, codeGen->frameDepth);

        emitLine("LABEL %s", doOrd);
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(res);
        free(tt);
        free(tt2);
        free(len);
        free(cmpRes);
        free(retZero);
        free(doOrd);
    }
    else if (strcmp(fname, "chr") == 0)
    {
        if (paramCount != 1) errorExit(SYNTAX_ERROR, "Ifj.chr expects 1 parameter", 0, NULL);
        char *i = tmpNames[0];
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);

        // check int
        char *tt = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tt, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@int", endLabel, tt, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        // INT2CHAR var symb
        emitLine("INT2CHAR LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        free(res);
        free(tt);
    }
    else if (strcmp(fname, "substring") == 0)
    {
        // substring(s,i,j) -> String | Null
        // Need types: s string, i int, j int
        if (paramCount != 3) errorExit(SYNTAX_ERROR, "Ifj.substring expects 3 parameters", 0, NULL);

        // Basic type checking: s string, i int, j int. If not types -> EXIT 25
        // For out-of-range we will push nil
        char *s = tmpNames[0];
        char *i = tmpNames[1];
        char *j = tmpNames[2];
        char *tt = genTempVar(codeGen);

        emitLine("DEFVAR LF@%s$%d", tt, codeGen->frameDepth);

        emitLine("TYPE LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, s, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@string", endLabel, tt, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        char *tt2 = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tt2, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", tt2, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@int", endLabel, tt2, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        char *tt3 = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tt3, codeGen->frameDepth);
        emitLine("TYPE LF@%s$%d LF@%s$%d", tt3, codeGen->frameDepth, j, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d string@int", endLabel, tt3, codeGen->frameDepth);
        emitLine("EXIT int@25");
        emitLine("LABEL %s", endLabel);

        // Range checks: i<0 or j<0 or i>j or i >= length(s) or j > length(s) => return nil
        // Compute len
        char *len = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", len, codeGen->frameDepth);
        emitLine("STRLEN LF@%s$%d LF@%s$%d", len, codeGen->frameDepth, s, codeGen->frameDepth);

        // Check conditions using simple comparisons, if any true -> push nil
        char *okLabel = genUniqueLabel(codeGen, "substr_ok");
        char *returnNil = genUniqueLabel(codeGen, "substr_nil");

        // if i < 0 -> nil
        emitLine("LT LF@%s$%d LF@%s$%d int@0", tt, codeGen->frameDepth, i, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", returnNil, tt, codeGen->frameDepth);
        // if j < 0 -> nil
        emitLine("LT LF@%s$%d LF@%s$%d int@0", tt, codeGen->frameDepth, j, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", returnNil, tt, codeGen->frameDepth);
        // if i > j -> nil
        emitLine("GT LF@%s$%d LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, i, codeGen->frameDepth,
                 j, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", returnNil, tt, codeGen->frameDepth);
        // if i >= len -> nil
        emitLine("LT LF@%s$%d LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, i, codeGen->frameDepth,
                 len, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@false", returnNil, tt, codeGen->frameDepth);
        // if j > len -> nil
        emitLine("GT LF@%s$%d LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, j, codeGen->frameDepth,
                 len, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@true", returnNil, tt, codeGen->frameDepth);

        // If all good -> build substring by iterating from i to j-1 and
        // concatenating GETCHAR results.
        char *res = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", res, codeGen->frameDepth);
        emitLine("MOVE LF@%s$%d string@", res, codeGen->frameDepth);

        char *idx = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", idx, codeGen->frameDepth);
        emitLine("MOVE LF@%s$%d LF@%s$%d", idx, codeGen->frameDepth, i, codeGen->frameDepth);

        char *loopLabel = genUniqueLabel(codeGen, "substr_loop");
        char *endLabel = genUniqueLabel(codeGen, "substr_end");

        emitLine("LABEL %s", loopLabel);
        // if idx >= j -> jump end
        emitLine("LT LF@%s$%d LF@%s$%d LF@%s$%d", tt, codeGen->frameDepth, idx, codeGen->frameDepth,
                 j, codeGen->frameDepth);
        emitLine("JUMPIFEQ %s LF@%s$%d bool@false", endLabel, tt, codeGen->frameDepth);

        // GETCHAR tmpchar = GETCHAR(s, idx)
        char *tmpchar = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", tmpchar, codeGen->frameDepth);
        emitLine("GETCHAR LF@%s$%d LF@%s$%d LF@%s$%d", tmpchar, codeGen->frameDepth, s,
                 codeGen->frameDepth, idx, codeGen->frameDepth);
        // CONCAT res = res + tmpchar
        emitLine("CONCAT LF@%s$%d LF@%s$%d LF@%s$%d", res, codeGen->frameDepth, res,
                 codeGen->frameDepth, tmpchar, codeGen->frameDepth);

        // idx = idx + 1
        char *one = genTempVar(codeGen);
        emitLine("DEFVAR LF@%s$%d", one, codeGen->frameDepth);
        emitLine("MOVE LF@%s$%d int@1", one, codeGen->frameDepth);
        emitLine("ADD LF@%s$%d LF@%s$%d LF@%s$%d", idx, codeGen->frameDepth, idx,
                 codeGen->frameDepth, one, codeGen->frameDepth);

        // jump back
        emitLine("JUMP %s", loopLabel);

        emitLine("LABEL %s", endLabel);
        // push result
        emitLine("PUSHS LF@%s$%d", res, codeGen->frameDepth);

        // cleanup tmp names (free)
        free(len);
        free(okLabel);
        free(returnNil);
        free(res);
        free(idx);
        free(loopLabel);
        free(endLabel);
        free(tmpchar);
        free(one);
        free(tt);
        free(tt2);
        free(tt3);
    }
    else
    {
        errorExit(INTERNAL_ERROR, "Unknown IFJ builtin called", 0, NULL);
    }

    // free tmpNames and generated tmp strings
    if (tmpNames)
    {
        for (long k = 0; k < paramCount; ++k)
        {
            free(tmpNames[k]);
        }
        free(tmpNames);
    }

    free(errLabel);
    free(endLabel);
}

void genParamsBackwardsRecursive(ListItem *item, CodeGenerator *codeGen)
{
    if (item == NULL) return;

    genParamsBackwardsRecursive(item->next, codeGen);

    emitLine("DEFVAR LF@%s$%d", item->item->token.value->stringVal, codeGen->frameDepth);
    emitLine("POPS LF@%s$%d", item->item->token.value->stringVal, codeGen->frameDepth);
}

void genParamsBackwards(SLList *list, CodeGenerator *codeGen)
{
    if (!list) return;
    genParamsBackwardsRecursive(list->firstItem, codeGen);
}

/**
 * @brief Generates code for a parameters node (AST_PARAMS).
 * @param node Pointer to the AST node representing a list of parameters in a function or call.
 */
void genParams(AstNode *node, CodeGenerator *codeGen)
{
    // Děti PARAMS jsou vždy IDENTIFIER nebo LITERAL -> listy (bez dat, rovnou token)

    AstN *nodeChildren = (AstN *)node->children;
    SLList *nodeList = nodeChildren->childrenList;

    ListItem *currentItem = nodeList->firstItem;

    // Definuji novou proměnnou (parametr funkce)
    // Popnu do ní proměnnou ze zásobníku
    if (codeGen->inFunction)
    {
        genParamsBackwards(nodeList, codeGen);
        emitLine("");
    }
    else
    {
        while (currentItem != NULL)
        {
            switch (currentItem->item->token.type)
            {
                case IDENTIFIER:
                    genIdentifier(currentItem->item, codeGen);
                    break;

                case INT_LITERAL:
                case FLOAT_LITERAL:
                case STRING_LITERAL:
                case MULTILINE_STRING_LITERAL:
                case KW_VAL_TRUE:
                case KW_VAL_FALSE:
                    genLiteral(currentItem->item);
                    break;

                default:
                    break;
            }

            currentItem = currentItem->next;
        }
    }
}

void genDynamicArithmetic(TokenType op, CodeGenerator *codeGen)
{
    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME");

    char *b = genTempVar(codeGen);
    char *a = genTempVar(codeGen);
    char *tb = genTempVar(codeGen);
    char *ta = genTempVar(codeGen);

    emitLine("DEFVAR LF@%s$%d", b, codeGen->frameDepth);
    emitLine("DEFVAR LF@%s$%d", a, codeGen->frameDepth);
    emitLine("DEFVAR LF@%s$%d", tb, codeGen->frameDepth);
    emitLine("DEFVAR LF@%s$%d", ta, codeGen->frameDepth);

    emitLine("POPS LF@%s$%d", b, codeGen->frameDepth);
    emitLine("POPS LF@%s$%d", a, codeGen->frameDepth);
    emitLine("TYPE LF@%s$%d LF@%s$%d", tb, codeGen->frameDepth, b, codeGen->frameDepth);
    emitLine("TYPE LF@%s$%d LF@%s$%d", ta, codeGen->frameDepth, a, codeGen->frameDepth);

    char *lEnd = genUniqueLabel(codeGen, "op_end");

    if (op == PLUS)
    {
        char *lNotStr = genUniqueLabel(codeGen, "not_str");
        emitLine("JUMPIFNEQ %s LF@%s$%d string@string", lNotStr, ta, codeGen->frameDepth);
        emitLine("JUMPIFNEQ %s LF@%s$%d string@string", lNotStr, tb, codeGen->frameDepth);

        emitLine("CONCAT LF@%s$%d LF@%s$%d LF@%s$%d", a, codeGen->frameDepth, a,
                 codeGen->frameDepth, b, codeGen->frameDepth);
        emitLine("PUSHS LF@%s$%d", a, codeGen->frameDepth);
        emitLine("JUMP %s", lEnd);

        emitLine("LABEL %s", lNotStr);
        free(lNotStr);
    }

    char *lFloat = genUniqueLabel(codeGen, "op_flt");
    char *lInt = genUniqueLabel(codeGen, "op_int");

    if (op == DIVIDE)
    {
        emitLine("JUMP %s", lFloat);
    }

    emitLine("JUMPIFEQ %s LF@%s$%d string@float", lFloat, ta, codeGen->frameDepth);
    emitLine("JUMPIFEQ %s LF@%s$%d string@float", lFloat, tb, codeGen->frameDepth);

    emitLine("LABEL %s", lInt);
    emitLine("PUSHS LF@%s$%d", a, codeGen->frameDepth);
    emitLine("PUSHS LF@%s$%d", b, codeGen->frameDepth);
    if (op == PLUS)
        emitLine("ADDS");
    else if (op == MINUS)
        emitLine("SUBS");
    else if (op == MULTIPLY)
        emitLine("MULS");
    emitLine("JUMP %s", lEnd);

    emitLine("LABEL %s", lFloat);
    char *lAOk = genUniqueLabel(codeGen, "a_ok");
    char *lBOk = genUniqueLabel(codeGen, "b_ok");

    emitLine("JUMPIFEQ %s LF@%s$%d string@float", lAOk, ta, codeGen->frameDepth);
    emitLine("INT2FLOAT LF@%s$%d LF@%s$%d", a, codeGen->frameDepth, a, codeGen->frameDepth);
    emitLine("LABEL %s", lAOk);

    emitLine("JUMPIFEQ %s LF@%s$%d string@float", lBOk, tb, codeGen->frameDepth);
    emitLine("INT2FLOAT LF@%s$%d LF@%s$%d", b, codeGen->frameDepth, b, codeGen->frameDepth);
    emitLine("LABEL %s", lBOk);

    emitLine("PUSHS LF@%s$%d", a, codeGen->frameDepth);
    emitLine("PUSHS LF@%s$%d", b, codeGen->frameDepth);
    if (op == PLUS)
        emitLine("ADDS");
    else if (op == MINUS)
        emitLine("SUBS");
    else if (op == MULTIPLY)
        emitLine("MULS");
    else if (op == DIVIDE)
        emitLine("DIVS");

    emitLine("LABEL %s", lEnd);

    emitLine("POPFRAME");

    free(b);
    free(a);
    free(tb);
    free(ta);
    free(lFloat);
    free(lInt);
    free(lEnd);
    free(lAOk);
    free(lBOk);
}

void genDynamicComparison(TokenType op, CodeGenerator *codeGen)
{
    emitLine("CREATEFRAME");
    emitLine("PUSHFRAME");

    // Vygenerujeme dočasné premenné
    char *b = genTempVar(codeGen);
    char *a = genTempVar(codeGen);
    char *tb = genTempVar(codeGen);
    char *ta = genTempVar(codeGen);

    // Definícia a načítanie hodnôt zo zásobníka
    emit("DEFVAR LF@%s$%d\n", b, codeGen->frameDepth);
    emit("DEFVAR LF@%s$%d\n", a, codeGen->frameDepth);
    emit("DEFVAR LF@%s$%d\n", tb, codeGen->frameDepth);
    emit("DEFVAR LF@%s$%d\n", ta, codeGen->frameDepth);

    emit("POPS LF@%s$%d\n", b, codeGen->frameDepth);
    emit("POPS LF@%s$%d\n", a, codeGen->frameDepth);

    // Zistíme typy
    emit("TYPE LF@%s$%d LF@%s$%d\n", tb, codeGen->frameDepth, b, codeGen->frameDepth);
    emit("TYPE LF@%s$%d LF@%s$%d\n", ta, codeGen->frameDepth, a, codeGen->frameDepth);

    char *lFloat = genUniqueLabel(codeGen, "cmp_flt");
    char *lEnd = genUniqueLabel(codeGen, "cmp_end");

    // Ak je aspoň jeden operand float, musíme konvertovať na float
    emit("JUMPIFEQ %s LF@%s$%d string@float\n", lFloat, ta, codeGen->frameDepth);
    emit("JUMPIFEQ %s LF@%s$%d string@float\n", lFloat, tb, codeGen->frameDepth);

    // --- VETVA INT / STRING / BOOL / NIL (bez konverzie) ---
    emit("PUSHS LF@%s$%d\n", a, codeGen->frameDepth);
    emit("PUSHS LF@%s$%d\n", b, codeGen->frameDepth);
    switch (op)
    {
        case IS_SMALLER:
            emitLine("LTS");
            break;
        case IS_BIGGER:
            emitLine("GTS");
            break;
        case IS_EQUAL:
            emitLine("EQS");
            break;
        default:
            break;
    }
    emit("JUMP %s\n", lEnd);

    // --- VETVA FLOAT (konverzia int -> float) ---
    emit("LABEL %s\n", lFloat);
    char *lAOk = genUniqueLabel(codeGen, "cmp_a_ok");
    char *lBOk = genUniqueLabel(codeGen, "cmp_b_ok");

    // Konverzia A ak nie je float
    emit("JUMPIFEQ %s LF@%s$%d string@float\n", lAOk, ta, codeGen->frameDepth);
    emit("INT2FLOAT LF@%s$%d LF@%s$%d\n", a, codeGen->frameDepth, a, codeGen->frameDepth);
    emit("LABEL %s\n", lAOk);

    // Konverzia B ak nie je float
    emit("JUMPIFEQ %s LF@%s$%d string@float\n", lBOk, tb, codeGen->frameDepth);
    emit("INT2FLOAT LF@%s$%d LF@%s$%d\n", b, codeGen->frameDepth, b, codeGen->frameDepth);
    emit("LABEL %s\n", lBOk);

    emit("PUSHS LF@%s$%d\n", a, codeGen->frameDepth);
    emit("PUSHS LF@%s$%d\n", b, codeGen->frameDepth);
    switch (op)
    {
        case IS_SMALLER:
            emitLine("LTS");
            break;
        case IS_BIGGER:
            emitLine("GTS");
            break;
        case IS_EQUAL:
            emitLine("EQS");
            break;
        default:
            break;
    }

    emit("LABEL %s\n", lEnd);

    emitLine("POPFRAME");

    // Uvoľnenie pamäte pre názvy
    free(b);
    free(a);
    free(tb);
    free(ta);
    free(lFloat);
    free(lEnd);
    free(lAOk);
    free(lBOk);
}

/**
 * @brief Generates code for an operator node (AST_OPERATOR).
 * @param node Pointer to the AST node representing an operator (+, -, *, /, <, >, etc.).
 */
void genOperator(AstNode *node, CodeGenerator *codeGen)
{
    if (node == NULL) return;

    AstBin *data = (AstBin *)node->children;

    if (node->token.type == LOGICAL_AND)
    {
        char *labEnd = genUniqueLabel(codeGen, "and_end");
        char *labFalse = genUniqueLabel(codeGen, "and_false");

        genNode(data->left, codeGen);

        char *tmp = genTempVar(codeGen);
        emit("DEFVAR LF@%s$%d\n", tmp, codeGen->frameDepth);
        emit("POPS LF@%s$%d\n", tmp, codeGen->frameDepth);

        emit("JUMPIFEQ %s LF@%s$%d bool@false\n", labFalse, tmp, codeGen->frameDepth);
        emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", labFalse, tmp, codeGen->frameDepth);

        genNode(data->right, codeGen);
        emit("JUMP %s\n", labEnd);

        emit("LABEL %s\n", labFalse);
        emit("PUSHS bool@false\n");

        emit("LABEL %s\n", labEnd);
        free(tmp);
        free(labEnd);
        free(labFalse);
        return;
    }
    else if (node->token.type == LOGICAL_OR)
    {
        char *labEnd = genUniqueLabel(codeGen, "or_end");
        char *labTrue = genUniqueLabel(codeGen, "or_true");

        genNode(data->left, codeGen);

        char *tmp = genTempVar(codeGen);
        emit("DEFVAR LF@%s$%d\n", tmp, codeGen->frameDepth);
        emit("POPS LF@%s$%d\n", tmp, codeGen->frameDepth);

        char *checkNil = genUniqueLabel(codeGen, "or_check_nil");
        char *goCalcB = genUniqueLabel(codeGen, "or_calc_b");

        emit("JUMPIFEQ %s LF@%s$%d bool@false\n", checkNil, tmp, codeGen->frameDepth);
        emit("JUMP %s\n", labTrue);

        emit("LABEL %s\n", checkNil);
        emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", goCalcB, tmp, codeGen->frameDepth);
        emit("JUMP %s\n", labTrue);

        emit("LABEL %s\n", goCalcB);
        genNode(data->right, codeGen);
        emit("JUMP %s\n", labEnd);

        emit("LABEL %s\n", labTrue);
        emit("PUSHS bool@true\n");

        emit("LABEL %s\n", labEnd);
        free(tmp);
        free(labEnd);
        free(labTrue);
        free(checkNil);
        free(goCalcB);
        return;
    }

    genNode(data->left, codeGen);
    genNode(data->right, codeGen);

    switch (node->token.type)
    {
        case PLUS:
        case MINUS:
        case MULTIPLY:
        case DIVIDE:
            genDynamicArithmetic(node->token.type, codeGen);
            break;

        // Relácie s dynamickou konverziou
        case IS_SMALLER:
            genDynamicComparison(IS_SMALLER, codeGen);
            break;
        case IS_BIGGER:
            genDynamicComparison(IS_BIGGER, codeGen);
            break;
        case IS_EQUAL:
            genDynamicComparison(IS_EQUAL, codeGen);
            break;

        case IS_NOT_EQUAL:  // != je negácia ==
            genDynamicComparison(IS_EQUAL, codeGen);
            emitLine("NOTS");
            break;

        case IS_SMALLER_OR_EQUAL:  // <= je negácia >
            genDynamicComparison(IS_BIGGER, codeGen);
            emitLine("NOTS");
            break;

        case IS_BIGGER_OR_EQUAL:  // >= je negácia <
            genDynamicComparison(IS_SMALLER, codeGen);
            emitLine("NOTS");
            break;

        case KW_IS:
        {
            char *wantedType = genTempVar(codeGen);
            char *val = genTempVar(codeGen);
            char *actualType = genTempVar(codeGen);

            emit("DEFVAR LF@%s$%d\n", wantedType, codeGen->frameDepth);
            emit("DEFVAR LF@%s$%d\n", val, codeGen->frameDepth);
            emit("DEFVAR LF@%s$%d\n", actualType, codeGen->frameDepth);

            emit("POPS LF@%s$%d\n", wantedType, codeGen->frameDepth);
            emit("POPS LF@%s$%d\n", val, codeGen->frameDepth);

            emit("TYPE LF@%s$%d LF@%s$%d\n", actualType, codeGen->frameDepth, val,
                 codeGen->frameDepth);

            char *isTrue = genUniqueLabel(codeGen, "is_true");
            char *isFalse = genUniqueLabel(codeGen, "is_false");
            char *endIs = genUniqueLabel(codeGen, "is_end");
            char *checkExact = genUniqueLabel(codeGen, "is_check_exact");

            emit("JUMPIFNEQ %s LF@%s$%d string@float\n", checkExact, wantedType,
                 codeGen->frameDepth);

            emit("JUMPIFEQ %s LF@%s$%d string@int\n", isTrue, actualType, codeGen->frameDepth);
            emit("JUMPIFEQ %s LF@%s$%d string@float\n", isTrue, actualType, codeGen->frameDepth);
            emit("JUMP %s\n", isFalse);

            emit("LABEL %s\n", checkExact);
            emit("JUMPIFEQ %s LF@%s$%d LF@%s$%d\n", isTrue, actualType, codeGen->frameDepth,
                 wantedType, codeGen->frameDepth);

            emit("LABEL %s\n", isFalse);
            emit("PUSHS bool@false\n");
            emit("JUMP %s\n", endIs);

            emit("LABEL %s\n", isTrue);
            emit("PUSHS bool@true\n");

            emit("LABEL %s\n", endIs);

            free(wantedType);
            free(val);
            free(actualType);
            free(isTrue);
            free(isFalse);
            free(endIs);
            free(checkExact);
            break;
        }

        default:
            errorExit(INTERNAL_ERROR, "Unknown operator in codegen", node->token.line, NULL);
    }
}

void genNodeChildren(AstNode *node, CodeGenerator *codeGen)
{
    if (!node) return;

    switch (node->type)
    {
        case AST_CLASS_DEC:
        case AST_BLOCK:
        case AST_PARAMS:
        {
            // Kontrola, zda má uzel vůbec nějaké děti (strukturu AstN)
            if (node->children == NULL) return;

            AstN *nodeChildren = (AstN *)node->children;

            // Kontrola, zda existuje seznam dětí
            if (nodeChildren->childrenList == NULL) return;

            SLList *nodeList = nodeChildren->childrenList;

            ListItem *currentItem = nodeList->firstItem;

            while (currentItem != NULL)
            {
                // LOOKAHEAD LOGIKA:
                // Ak je aktuálny uzol IF a nasledujúci je ELSE, spracuj ich spolu.
                if (currentItem->item->type == AST_IF_STMT)
                {
                    ListItem *nextItem = currentItem->next;

                    // Musíme zistiť, či 'nextItem' je ELSE.
                    // Keďže parser vracia pre ELSE typ AST_IF_STMT, musíme sa pozrieť hlbšie.
                    // ELSE nemá podmienku (left == NULL).
                    bool isNextElse = false;
                    if (nextItem && nextItem->item->type == AST_IF_STMT)
                    {
                        AstBin *nextData = (AstBin *)nextItem->item->children;
                        if (nextData != NULL && nextData->left == NULL)
                        {
                            isNextElse = true;
                        }
                    }

                    if (isNextElse)
                    {
                        // Máme IF nasledovaný ELSE -> generujeme plný IF-ELSE
                        AstNode *ifNode = currentItem->item;
                        AstNode *elseNode =
                            nextItem->item;  // Toto je ten "fake" IF, ktorý je v skutočnosti ELSE

                        // Získanie dát
                        if (ifNode->children == NULL || elseNode->children == NULL)
                        {
                            // Fallback to standard processing
                            genNode(currentItem->item, codeGen);
                            currentItem = currentItem->next;
                            continue;
                        }

                        AstBin *ifData = (AstBin *)ifNode->children;
                        AstBin *elseData = (AstBin *)elseNode->children;

                        if (ifData == NULL || elseData == NULL || ifData->left == NULL ||
                            ifData->right == NULL || elseData->right == NULL)
                        {
                            // Fallback to standard processing
                            genNode(currentItem->item, codeGen);
                            currentItem = currentItem->next;
                            continue;
                        }

                        AstNode *cond = ifData->left;
                        AstNode *trueBlock = ifData->right;
                        AstNode *falseBlock = elseData->right;  // ELSE blok je vpravo

                        // --- Generovanie kódu (IF-ELSE) ---
                        char *labelElse = genUniqueLabel(codeGen, "if_else");
                        char *labelEnd = genUniqueLabel(codeGen, "if_end");

                        emitLine("# If-Else statement (merged)");

                        // 1. Podmienka
                        genNode(cond, codeGen);

                        char *condRes = genTempVar(codeGen);
                        emit("DEFVAR LF@%s$%d\n", condRes, codeGen->frameDepth);
                        emit("POPS LF@%s$%d\n", condRes, codeGen->frameDepth);

                        emit("JUMPIFEQ %s LF@%s$%d bool@false\n", labelElse, condRes,
                             codeGen->frameDepth);
                        emit("JUMPIFEQ %s LF@%s$%d nil@nil\n", labelElse, condRes,
                             codeGen->frameDepth);
                        free(condRes);

                        // 2. True vetva
                        genNode(trueBlock, codeGen);
                        emit("JUMP %s\n", labelEnd);

                        // 3. Else vetva
                        emit("LABEL %s\n", labelElse);
                        genNode(falseBlock, codeGen);

                        emit("LABEL %s\n", labelEnd);

                        free(labelElse);
                        free(labelEnd);

                        // POSUN: Preskočíme ELSE uzol, lebo sme ho už spracovali
                        currentItem = nextItem->next;
                        continue;
                    }
                }

                // Štandardné spracovanie (samostatný IF, WHILE, iné príkazy)
                genNode(currentItem->item, codeGen);
                currentItem = currentItem->next;
            }
        }
        break;

        case AST_IDENTIFIER:
        case AST_LITERAL:
            break;

        default:
            // AstBin
            {
                if (node->children == NULL) break;
                AstBin *nodeChildren = (AstBin *)node->children;
                if (nodeChildren == NULL) break;

                if (nodeChildren->left)
                {
                    genNode(nodeChildren->left, codeGen);
                }

                if (nodeChildren->right)
                {
                    genNode(nodeChildren->right, codeGen);
                }
            }
            break;
    }
}
