/**
 * @file scanner.c
 * @author Samuel Vajda (xvajdas00)
 * @brief Scanner (lexical analyzer) implementation for IFJ25 compiler
 */

#include "scanner.h"
#include "error.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// ==================== HELPER FUNCTIONS ====================

/**
 * @brief Check if character is whitespace (excluding newline)
 * @param character Character to check
 * @return true if whitespace (space or tab), false otherwise
 */
static bool isWhiteSpace(int character)
{
    return character == ' ' || character == '\t';
}

/**
 * @brief Check if character can start an identifier
 * @param character Character to check
 * @return true if letter or underscore, false otherwise
 */
static bool isIdentifierStart(int character)
{
    return isalpha(character) || character == '_';
}

/**
 * @brief Check if character can be part of identifier
 * @param character Character to check
 * @return true if alphanumeric or underscore, false otherwise
 */
static bool isIdentifierPart(int character)
{
    return isalnum(character) || character == '_';
}

/**
 * @brief Allocate and initialize token value
 * @return Pointer to allocated TokenValue or NULL on error
 */
static TokenValue *allocTokenValue(void)
{
    TokenValue *value = (TokenValue *)malloc(sizeof(TokenValue));
    if (value)
    {
        value->stringVal = NULL;
        value->intVal = 0;
        value->floatVal = 0.0;
    }
    return value;
}

/**
 * @brief Convert hex digit character to integer value
 * @param hexChar Hex digit character
 * @return Integer value 0-15, or -1 on error
 */
static int hexToInt(int hexChar)
{
    if (hexChar >= '0' && hexChar <= '9') return hexChar - '0';
    if (hexChar >= 'a' && hexChar <= 'f') return hexChar - 'a' + 10;
    if (hexChar >= 'A' && hexChar <= 'F') return hexChar - 'A' + 10;
    return -1;
}

// ==================== SCANNER INIT/DISPOSE ====================

void initScanner(Scanner *scanner, FILE *source)
{
    scanner->source = source;
    scanner->line = 1;
    scanner->prologueRead = false;
    scanner->isLookingAhead = false;
    scanner->lastWasEOL = false;
    memset(&scanner->lookAheadToken, 0, sizeof(Token));
}

void disposeScanner(Scanner *scanner)
{
    if (scanner->isLookingAhead)
    {
        freeToken(&scanner->lookAheadToken);
    }
}

void freeToken(Token *token)
{
    if (token->value)
    {
        if (token->value->stringVal)
        {
            free(token->value->stringVal);
        }
        free(token->value);
        token->value = NULL;
    }
}

// ==================== KEYWORD IDENTIFICATION ====================

/**
 * @brief Checks if a given string matches a reserved keyword
 * @param string The identifier string to check
 * @return The corresponding keyword TokenType, or IDENTIFIER if no match found
 */
TokenType identifyKeyword(const char *string)
{
    static const struct
    {
        const char *keyword;
        TokenType type;
    } KEYWORDS[] = {{"class", KW_CLASS},    {"if", KW_IF},
                    {"else", KW_ELSE},      {"is", KW_IS},
                    {"null", KW_VAL_NULL},  {"return", KW_RETURN},
                    {"var", KW_VAR},        {"while", KW_WHILE},
                    {"Ifj", KW_IFJ},        {"static", KW_STATIC},
                    {"import", KW_IMPORT},  {"for", KW_FOR},
                    {"Num", KW_TYPE_NUM},   {"String", KW_TYPE_STRING},
                    {"Null", KW_TYPE_NULL}, {"Bool", KW_TYPE_BOOL},
                    {"true", KW_VAL_TRUE},  {"false", KW_VAL_FALSE},
                    {NULL, IDENTIFIER}};

    for (int i = 0; KEYWORDS[i].keyword != NULL; i++)
    {
        if (strcmp(string, KEYWORDS[i].keyword) == 0)
        {
            return KEYWORDS[i].type;
        }
    }
    return IDENTIFIER;
}

// ==================== STRING PROCESSING ====================

/**
 * @brief Process escape sequence in string literal
 * @param scanner Scanner state
 * @param buffer String buffer
 * @param buffCapacity Current buffer capacity
 * @param strLen Current string length
 * @return 0 on success, 1 on error
 */
static int processEscape(Scanner *scanner, char **buffer, size_t *buffCapacity, size_t *strLen)
{
    int character = fgetc(scanner->source);
    unsigned char escaped = 0;

    switch (character)
    {
        case '"':
            escaped = '"';
            break;
        case 'n':
            escaped = '\n';
            break;
        case 'r':
            escaped = '\r';
            break;
        case 't':
            escaped = '\t';
            break;
        case '\\':
            escaped = '\\';
            break;
        case '0':
            escaped = '\0';
            break;
        case 'x':
        {
            // Hex escape sequence \xdd
            int hex1 = fgetc(scanner->source);
            int hex2 = fgetc(scanner->source);

            int value1 = hexToInt(hex1);
            int value2 = hexToInt(hex2);

            if (value1 == -1 || value2 == -1) return 1;

            escaped = (unsigned char)(value1 * 16 + value2);

            if (escaped > 0x7F)
            {
                // hex escape out of ASCII range
                return 1;
            }

            break;
        }
        default:
            return 1;
    }

    // Ensure buffer buffCapacity
    if (*strLen >= *buffCapacity - 1)
    {
        *buffCapacity *= 2;
        char *newBuff = (char *)realloc(*buffer, *buffCapacity);
        if (!newBuff) return 1;
        *buffer = newBuff;
    }

    (*buffer)[(*strLen)++] = escaped;
    return 0;
}

/**
 * @brief Read string literal
 * @param scanner Scanner state
 * @param token Token to fill
 * @return 0 on success, 1 on error
 */
static int readString(Scanner *scanner, Token *token)
{
    size_t buffCapacity = 32;
    size_t strLen = 0;
    char *buffer = (char *)malloc(buffCapacity);
    if (!buffer) return 1;

    int character;
    while ((character = fgetc(scanner->source)) != EOF)
    {
        if (character == '"')
        {
            // End of string
            buffer[strLen] = '\0';
            token->type = STRING_LITERAL;
            token->value = allocTokenValue();
            if (!token->value)
            {
                free(buffer);
                return 1;
            }
            token->value->stringVal = buffer;
            return 0;
        }
        else if (character == '\n')
        {
            // Newline not allowed in regular string
            free(buffer);
            return 1;
        }
        else if (character == '\\')
        {
            // Escape sequence
            if (processEscape(scanner, &buffer, &buffCapacity, &strLen) != 0)
            {
                free(buffer);
                return 1;
            }
            continue;
        }
        else if ((unsigned char)character < 32)
        {
            // Newline is allowed ONLY as an escape - "\n"
            if (character == '\n')
            {
                free(buffer);
                return 1;   // literal newline inside string (error)
            }

            // Other control chars - lex error
            free(buffer);
            return 1;
        }
        else
        {
            // Regular character
            if (strLen >= buffCapacity - 1)
            {
                buffCapacity *= 2;
                char *newBuff = (char *)realloc(buffer, buffCapacity);
                if (!newBuff)
                {
                    free(buffer);
                    return 1;
                }
                buffer = newBuff;
            }
            buffer[strLen++] = (char)character;
        }
    }

    free(buffer);
    return 1;  // EOF before closing quote
}

/**
 * @brief Read multiline string literal (""")
 * @param scanner Scanner state
 * @param token Token to fill
 * @return 0 on success, 1 on error
 */
static int readMultilineString(Scanner *scanner, Token *token)
{
    size_t buffCapacity = 64;
    size_t strLen = 0;
    char *buffer = (char *)malloc(buffCapacity);
    if (!buffer) return 1;

    bool firstLine = true;
    int character;

    while ((character = fgetc(scanner->source)) != EOF)
    {
        if (character == '"')
        {
            // Check for closing """
            int next1 = fgetc(scanner->source);
            if (next1 == '"')
            {
                int next2 = fgetc(scanner->source);
                if (next2 == '"')
                {
                    // Found closing """
                    buffer[strLen] = '\0';
                    token->type = MULTILINE_STRING_LITERAL;
                    token->value = allocTokenValue();
                    if (!token->value)
                    {
                        free(buffer);
                        return 1;
                    }
                    token->value->stringVal = buffer;
                    return 0;
                }
                else
                {
                    // Not closing, store the quotes
                    if (strLen + 2 >= buffCapacity)
                    {
                        buffCapacity *= 2;
                        char *newBuff = (char *)realloc(buffer, buffCapacity);
                        if (!newBuff)
                        {
                            free(buffer);
                            return 1;
                        }
                        buffer = newBuff;
                    }
                    buffer[strLen++] = '"';
                    buffer[strLen++] = (char)next1;
                    ungetc(next2, scanner->source);
                }
            }
            else
            {
                if (strLen >= buffCapacity - 1)
                {
                    buffCapacity *= 2;
                    char *newBuff = (char *)realloc(buffer, buffCapacity);
                    if (!newBuff)
                    {
                        free(buffer);
                        return 1;
                    }
                    buffer = newBuff;
                }
                buffer[strLen++] = '"';
                ungetc(next1, scanner->source);
            }
        }
        else
        {
            if (character == '\n')
            {
                scanner->line++;
                if (firstLine)
                {
                    firstLine = false;
                    continue;  // Skip first newline
                }
            }

            if (strLen >= buffCapacity - 1)
            {
                buffCapacity *= 2;
                char *newBuff = (char *)realloc(buffer, buffCapacity);
                if (!newBuff)
                {
                    free(buffer);
                    return 1;
                }
                buffer = newBuff;
            }
            buffer[strLen++] = (char)character;
        }
    }

    free(buffer);
    return 1;  // EOF before closing """
}

// =================== NUMBER PROCESSING ===================

/**
 * @brief Read numeric literal (int or float)
 * @param scanner Scanner state
 * @param token Token to fill
 * @param firstChar First character already read
 * @return 0 on success, 1 on error
 */
static int readNumber(Scanner *scanner, Token *token, int firstChar)
{
    size_t buffCapacity = 32;
    size_t strLen = 0;
    char *buffer = (char *)malloc(buffCapacity);
    if (!buffer) return 1;

    buffer[strLen++] = (char)firstChar;
    bool isFloat = false;
    bool isHex = false;

    // Check for hex literal (0x)
    if (firstChar == '0')
    {
        int next = fgetc(scanner->source);
        if (next == 'x' || next == 'X')
        {
            isHex = true;
            if (strLen >= buffCapacity - 1)
            {
                buffCapacity *= 2;
                char *newBuff = (char *)realloc(buffer, buffCapacity);
                if (!newBuff)
                {
                    free(buffer);
                    return 1;
                }
                buffer = newBuff;
            }
            buffer[strLen++] = (char)next;
        }
        else
        {
            ungetc(next, scanner->source);
        }
    }

    int character;
    while ((character = fgetc(scanner->source)) != EOF)
    {
        if (isHex)
        {
            if (!isxdigit(character))
            {
                ungetc(character, scanner->source);
                break;
            }
        }
        else if (isdigit(character))
        {
            // Continue
        }
        else if (character == '.')
        {
            if (isFloat)
            {
                ungetc(character, scanner->source);
                break;
            }

            int nextChar = fgetc(scanner->source);
            if (!isdigit(nextChar))
            {
                ungetc(nextChar, scanner->source);
                ungetc(character, scanner->source);
                break;
            }
            ungetc(nextChar, scanner->source);

            isFloat = true;
        }
        else if (character == 'e' || character == 'E')
        {
            isFloat = true;
    buffer[strLen++] = character;

    int sign = fgetc(scanner->source);
    if (sign == '+' || sign == '-')
    {
        buffer[strLen++] = sign;
    }
    else
    {
        ungetc(sign, scanner->source);
    }

    int next = fgetc(scanner->source);
    if (!isdigit(next))
    {
        free(buffer);
        return 1;   // INVALID EXPONENT → lexical error
    }

    buffer[strLen++] = next;
    continue;
        }
        else
        {
            ungetc(character, scanner->source);
            break;
        }

        if (strLen >= buffCapacity - 1)
        {
            buffCapacity *= 2;
            char *newBuff = (char *)realloc(buffer, buffCapacity);
            if (!newBuff)
            {
                free(buffer);
                return 1;
            }
            buffer = newBuff;
        }
        buffer[strLen++] = (char)character;
    }

    // Check for hex numbers without numbers (0x...)
    if (isHex && strLen <= 2)
    {
        free(buffer);
        return 1;
    }


    buffer[strLen] = '\0';

    token->value = allocTokenValue();
    if (!token->value)
    {
        free(buffer);
        return 1;
    }

    if (isFloat)
    {
        token->type = FLOAT_LITERAL;
        token->value->floatVal = strtod(buffer, NULL);
    }
    else
    {
        token->type = INT_LITERAL;
        token->value->intVal = strtoll(buffer, NULL, isHex ? 16 : 10);
    }

    free(buffer);
    return 0;
}

// ==================== COMMENT HANDLING ====================

/**
 * @brief Skip line comment
 * @param scanner Scanner state
 */
static void skipLineComment(Scanner *scanner)
{
    int character;
    while ((character = fgetc(scanner->source)) != EOF && character != '\n')
    {
        // Skip until newline
    }
    if (character == '\n')
    {
        scanner->line++;
    }
}

/**
 * @brief Skip block comment (supports nesting)
 * @param scanner Scanner state
 * @return 0 on success, 1 on error (unterminated comment)
 */
static int skipBlockComment(Scanner *scanner)
{
    int depth = 1;
    int character;

    while (depth > 0 && (character = fgetc(scanner->source)) != EOF)
    {
        if (character == '\n')
        {
            scanner->line++;
        }
        else if (character == '/')
        {
            int next = fgetc(scanner->source);
            if (next == '*')
            {
                depth++;
            }
            else
            {
                ungetc(next, scanner->source);
            }
        }
        else if (character == '*')
        {
            int next = fgetc(scanner->source);
            if (next == '/')
            {
                depth--;
            }
            else
            {
                ungetc(next, scanner->source);
            }
        }
    }

    return (depth == 0) ? 0 : 1;
}

// ==================== MAIN INTERNAL TOKEN GETTER ===================

/**
 * @brief Low-level function to extract the next raw token from source
 * @details Skips whitespace and comments. Identifies single-char tokens,
 * operators, and delegates complex tokens (strings, numbers, IDs) 
 * to helper functions.
 * @param scanner Scanner state
 * @param token Pointer to where the token data will be stored
 * @return 0 on success, 1 on lexical error
 */
int getRawToken(Scanner *scanner, Token *token)
{
    memset(token, 0, sizeof(Token));
    token->line = scanner->line;

    int character;

    // Skip whitespace and comments
    while (true)
    {
        character = fgetc(scanner->source);

        if (character == EOF)
        {
            token->type = EOF_TOKEN;
            return 0;
        }

        // Skip whitespace (not newline)
        if (isWhiteSpace(character))
        {
            continue;
        }
        if (character == '\r') continue;

        // Handle newline
        if (character == '\n')
        {
            token->type = EOL;
            token->line = scanner->line;
            scanner->line++;
            return 0;
        }

        // Handle comments
        if (character == '/')
        {
            int next = fgetc(scanner->source);
            if (next == '/')
            {
                skipLineComment(scanner);
                token->type = EOL;
                token->line = scanner->line;
                return 0;
            }
            else if (next == '*')
            {
                if (skipBlockComment(scanner) != 0)
                {
                    token->type = TOKEN_ERROR;
                    return 1;
                }
                continue;
            }
            else
            {
                ungetc(next, scanner->source);
                token->type = DIVIDE;
                return 0;
            }
        }

        break;
    }

    // Process tokens
    switch (character)
    {
        case '(':
            token->type = LPAR;
            return 0;
        case ')':
            token->type = RPAR;
            return 0;
        case '{':
            token->type = LCURLY;
            return 0;
        case '}':
            token->type = RCURLY;
            return 0;
        case ',':
            token->type = COMMA;
            return 0;
        case '.':
            token->type = DOT;
            return 0;
        case '+':
            token->type = PLUS;
            return 0;
        case '-':
            token->type = MINUS;
            return 0;
        case '*':
            token->type = MULTIPLY;
            return 0;
        case '?':
            token->type = TERNARY_QUESTION;
            return 0;
        case ':':
            token->type = TERNARY_COLON;
            return 0;
        case '=':
        {
            int next = fgetc(scanner->source);
            if (next == '=')
            {
                token->type = IS_EQUAL;
            }
            else
            {
                ungetc(next, scanner->source);
                token->type = ASSIGN;
            }
            return 0;
        }

        case '<':
        {
            int next = fgetc(scanner->source);
            if (next == '=')
            {
                token->type = IS_SMALLER_OR_EQUAL;
            }
            else
            {
                ungetc(next, scanner->source);
                token->type = IS_SMALLER;
            }
            return 0;
        }

        case '>':
        {
            int next = fgetc(scanner->source);
            if (next == '=')
            {
                token->type = IS_BIGGER_OR_EQUAL;
            }
            else
            {
                ungetc(next, scanner->source);
                token->type = IS_BIGGER;
            }
            return 0;
        }

        case '!':
        {
            int next = fgetc(scanner->source);
            if (next == '=')
            {
                token->type = IS_NOT_EQUAL;
                return 0;
            }
            else
            {
                ungetc(next, scanner->source);
                token->type = LOGICAL_NOT;
                return 0;
            }
        }

        case '&':
        {
            int next = fgetc(scanner->source);
            if (next == '&')
            {
                token->type = LOGICAL_AND;
                return 0;
            }
            else
            {
                token->type = TOKEN_ERROR;
                return 1;
            }
        }

        case '|':
        {
            int next = fgetc(scanner->source);
            if (next == '|')
            {
                token->type = LOGICAL_OR;
                return 0;
            }
            else
            {
                token->type = TOKEN_ERROR;
                return 1;
            }
        }

        case '"':
        {
            // Check for multiline string
            int next1 = fgetc(scanner->source);
            if (next1 == '"')
            {
                int next2 = fgetc(scanner->source);
                if (next2 == '"')
                {
                    return readMultilineString(scanner, token);
                }
                else
                {
                    ungetc(next2, scanner->source);
                    // Empty string
                    token->type = STRING_LITERAL;
                    token->value = allocTokenValue();
                    if (!token->value) return 1;
                    token->value->stringVal =
                        (char *)malloc(1);  // Because empty string also needs '\0'
                    if (!token->value->stringVal) return 1;
                    token->value->stringVal[0] = '\0';
                    return 0;
                }
            }
            else
            {
                ungetc(next1, scanner->source);
                return readString(scanner, token);
            }
        }

        default:
        {
            // Identifiers and KEYWORDS
            if (isIdentifierStart(character))
            {
                size_t buffCapacity = 32;
                size_t strLen = 0;
                char *buffer = (char *)malloc(buffCapacity);
                if (!buffer) return 1;

                buffer[strLen++] = (char)character;

                // Check for global identifier (__xxx)
                bool isGlobal = false;
                if (character == '_')
                {
                    int next = fgetc(scanner->source);
                    if (next == '_')
                    {
                        isGlobal = true;
                        if (strLen >= buffCapacity - 1)
                        {
                            buffCapacity *= 2;
                            char *newBuff = (char *)realloc(buffer, buffCapacity);
                            if (!newBuff)
                            {
                                free(buffer);
                                return 1;
                            }
                            buffer = newBuff;
                        }
                        buffer[strLen++] = (char)next;
                    }
                    else
                    {
                        ungetc(next, scanner->source);
                    }
                }

                // Read rest of identifier
                while ((character = fgetc(scanner->source)) != EOF && isIdentifierPart(character))
                {
                    if (strLen >= buffCapacity - 1)
                    {
                        buffCapacity *= 2;
                        char *newBuff = (char *)realloc(buffer, buffCapacity);
                        if (!newBuff)
                        {
                            free(buffer);
                            return 1;
                        }
                        buffer = newBuff;
                    }
                    buffer[strLen++] = (char)character;
                }
                ungetc(character, scanner->source);

                buffer[strLen] = '\0';

                if (isGlobal)
                {
                    token->type = GLOBAL_IDENTIFIER;
                }
                else
                {
                    token->type = identifyKeyword(buffer);
                }

                token->value = allocTokenValue();
                if (!token->value)
                {
                    free(buffer);
                    return 1;
                }
                token->value->stringVal = buffer;
                return 0;
            }

            // Numbers
            if (isdigit(character))
            {
                return readNumber(scanner, token, character);
            }

            // Unknown character
            token->type = TOKEN_ERROR;
            return 1;
        }
    }
}

// ==================== PUBLIC TOKEN GETTER ===================

/**
 * @brief Main public API to get the next valid token
 * @details Checks the lookahead buffer first. If empty, calls getRawToken.
 * Also handles filtering of redundant EOL tokens (merges multiple newlines).
 * @param scanner Scanner state
 * @param token Pointer to where the token data will be stored
 * @return 0 on success, 1 on lexical error
 */
int getNextToken(Scanner *scanner, Token *token)
{
    if (scanner->isLookingAhead)
    {
        *token = scanner->lookAheadToken;
        scanner->isLookingAhead = false;
        memset(&scanner->lookAheadToken, 0, sizeof(Token));

        if (token->type == EOL)
        {
            scanner->lastWasEOL = true;
        }
        else
        {
            scanner->lastWasEOL = false;
        }
        return 0;
    }

    int result = getRawToken(scanner, token);
    if (result != 0)
    {
        return result;
    }

    // Filter multiple consecutive EOLs
    if (token->type == EOL)
    {
        if (scanner->lastWasEOL)
        {
            // Skip this EOL and get next token
            return getNextToken(scanner, token);
        }
        scanner->lastWasEOL = true;
    }
    else
    {
        scanner->lastWasEOL = false;
    }

    return 0;
}

/**
 * @brief Peeks at the next token without consuming it from the parser's perspective
 * @details Stores the token in the scanner's lookahead buffer. Subsequent calls
 * to getNextToken will return this stored token.
 * @param scanner Scanner state
 * @param token Pointer where the lookahead token data will be copied
 * @return 0 on success, 1 on lexical error
 */
int lookAhead(Scanner *scanner, Token *token)
{
    if (scanner->isLookingAhead)
    {
        *token = scanner->lookAheadToken;
        return 0;
    }

    int result = getNextToken(scanner, &scanner->lookAheadToken);
    if (result != 0)
    {
        return result;
    }

    scanner->isLookingAhead = true;
    *token = scanner->lookAheadToken;
    return 0;
}

/**
 * @brief Reads and validates the mandatory prologue: import "ifj25" for Ifj
 * @param scanner Scanner state
 * @return 0 if valid prologue (exits program on error)
 */
int readPrologue(Scanner *scanner)
{
    Token token;

    // 0. Skip empty lines or comments before the actual prologue
    do
    {
        // If getNextToken fails (e.g. encounters invalid char like '\'), 
        // it is a LEXICAL ERROR (Exit Code 1)
        if (getNextToken(scanner, &token) != 0)
        {
            errorExit(LEXICAL_ERROR, "Lexical error before prologue", scanner->line, NULL);
        }
    } while (token.type == EOL);

    // 1. Check for 'import' keyword
    // If token is valid (e.g. '+') but not 'import', it is a SYNTAX ERROR (Exit Code 2)
    if (token.type != KW_IMPORT)
    {
        errorExit(SYNTAX_ERROR, "Prologue must start with 'import'", scanner->line, &token);
    }
    freeToken(&token);

    // 2. Check for "ifj25" string literal
    if (getNextToken(scanner, &token) != 0)
    {
        errorExit(LEXICAL_ERROR, "Lexical error in prologue", scanner->line, NULL);
    }
    if (token.type != STRING_LITERAL)
    {
        errorExit(SYNTAX_ERROR, "Expected string literal after import", scanner->line, &token);
    }
    if (!token.value || !token.value->stringVal || strcmp(token.value->stringVal, "ifj25") != 0)
    {
        errorExit(SYNTAX_ERROR, "Imported module must be \"ifj25\"", scanner->line, &token);
    }
    freeToken(&token);

    // 3. Check for 'for' keyword
    if (getNextToken(scanner, &token) != 0)
    {
        errorExit(LEXICAL_ERROR, "Lexical error in prologue", scanner->line, NULL);
    }
    if (token.type != KW_FOR)
    {
        errorExit(SYNTAX_ERROR, "Expected 'for' after module name", scanner->line, &token);
    }
    freeToken(&token);

    // 4. Check for 'Ifj' keyword
    if (getNextToken(scanner, &token) != 0)
    {
        errorExit(LEXICAL_ERROR, "Lexical error in prologue", scanner->line, NULL);
    }
    if (token.type != KW_IFJ)
    {
        errorExit(SYNTAX_ERROR, "Expected 'Ifj' after for", scanner->line, &token);
    }
    freeToken(&token);

    // 5. Check for End of Line (prologue must be on its own line)
    if (getNextToken(scanner, &token) != 0)
    {
        errorExit(LEXICAL_ERROR, "Lexical error in prologue", scanner->line, NULL);
    }
    if (token.type != EOL && token.type != EOF_TOKEN)
    {
        errorExit(SYNTAX_ERROR, "Prologue must end with newline", scanner->line, &token);
    }
    freeToken(&token);

    scanner->prologueRead = true;
    return 0; // Success
}