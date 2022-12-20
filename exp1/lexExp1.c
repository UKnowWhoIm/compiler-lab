#include <stdio.h>
#include <ctype.h>
#include <string.h>

FILE *outputFp;
__uint8_t stateMask = 15; // 1111
__uint8_t shouldShiftMask = 16; // 10000

char readToken(FILE* fp) {
    char buff[1];
    if (fread(buff, 1, 1, fp) == 0) {
        return -1;
    }
    return buff[0];
}

void error(const char* message) {
    fprintf(outputFp, "ERROR: %s\n", message);
}

void errorWithAdditionalValue(const char* text, const char* additionalValue) {
    fprintf(outputFp, "ERROR: %s -> %s\n", text, additionalValue);
}

void createToken(const char* message) {
    fprintf(outputFp, "%s\n", message);
}

void createTokenWithAdditionalValue(const char* text, char additionalValue[100]) {
    fprintf(outputFp, "%s -> %s\n", text, additionalValue);
}

__uint8_t isEndOfToken(char token) {
    return token == ' ' || token == '\n' || token == -1;
}

__uint8_t isKeyword(char buff[100]) {
    char keywords[][10] = {
        "if", "else", "void", "int", "char", "float", "double", "unsigned", "const", "return",
        "break", "continue", "while", "for", "do", "typedef"
    };
    for (__uint8_t i = 0; i < sizeof(keywords) / 10; i++) {
        if (strcmp(keywords[i], buff) == 0) {
            return 1;
        }
    }
    return 0;
}

__uint8_t getState(__uint8_t res) {
    return res & stateMask;
}

__uint8_t getShouldShift(__uint8_t res) {
    return res & shouldShiftMask;
}

int handleState0NonArithematicOp(char* current, const char lookAhead) {
    switch(*current) {
        case '<':
            if (lookAhead == '<') {
                *current = -1;
                createToken("Bitwise Op: << -> Left Shift");
                return 0;
            }
            if (lookAhead == '=') {
                *current = -1;
                createToken("Relational Op: <= -> Less than equal to");
                return 0;
            }
            createToken("Relational Op: < -> Less Than");
            return 0;
        case '=':
            if (lookAhead == '=') {
                *current = -1;
                createToken("Relational Op: == -> Equality");
                return 0;
            }
            createToken("Assignment Op: = -> Equal");
            return 0;
        case '>':
            if (lookAhead == '<') {
                *current = -1;
                createToken("Bitwise Op: >> -> Right Shift");
                return 0;
            }
            if (lookAhead == '=') {
                *current = -1;
                createToken("Relational Op: >= -> Greater than equal to");
                return 0;
            }
            createToken("Relational Op: > -> Greater Than");
            return 0;
        case '&':
            if (lookAhead == '&') {
                *current = -1;
                createToken("Logical Op: && -> AND");
                return 0;
            }
            createToken("Bitwise Op: & -> AND");
            return 0;
        case '|':
            if (lookAhead == '|') {
                *current = -1;
                createToken("Logical Op: || -> OR");
                return 0;
            }
            createToken("Bitwise Op: | -> OR");
            return 0;
        case '!':
            if (lookAhead == '=') {
                *current = -1;
                createToken("Relational Op: != -> Not Equal to");
                return 0;
            }
            createToken("Logical Op: ! -> Logical Not");
            return 0;
        case '?':
            createToken("Conditional Op: ? -> Question");
        default:
            return -1;
    }
}

int handleState0IdentierKeywordInteger(const char current, char currentBuff[100]) {
    if (current == '_' || isalpha(current)) {
        strncat(currentBuff, &current, 1);
        return 6;
    }
    if (isdigit(current)) {
        strncat(currentBuff, &current, 1);
        return 7;
    }
    return -1;
}

int handleState0FwdSlash(char *current, const char lookAhead) {
    if (*current != '/') {
        return -1;
    }
    switch (lookAhead) {
        case '=':
            createToken("Assignment Op: /= -> Divide equal to");
            *current = -1;
            return 0;
        case '/':
            createToken("Avoiding single line comment");
            *current = -1;
            return 8;
        case '*':
            createToken("Avoiding multi line comment");
            *current = -1;
            return 9;
        default:
            createToken("Arithematic Op: / -> Division");
            return 0;
    }
}

int handleState0Arithematic(char *current, const char lookAhead) {
    switch (*current) {
        case '+':
            if (isEndOfToken(lookAhead)) {
                createToken("Arithematic Op: + -> Addition");
                return 0;
            }
            if (lookAhead == '=') {
                *current = -1;
                createToken("Assignment Op: += -> Plus Equal to");
                return 0;
            }
            return -1;
        case '-':
            if (isEndOfToken(lookAhead)) {
                createToken("Arithematic Op: - -> Subtraction");
                return 0;
            }
            if (lookAhead == '=') {
                *current = -1;
                createToken("Assignment Op: -= -> Minus Equal to");
                return 0;
            }
            if (lookAhead == '>') {
                *current = -1;
                createToken("Assignment Op: -> -> Arrow operator");
                return 0;
            }
            return -1;
        case '*':
            if (isEndOfToken(lookAhead)) {
                createToken("Arithematic Op: * -> Multiplication");
                return 0;
            }
            if (lookAhead == '=') {
                *current = -1;
                createToken("Assignment Op: *= -> Multiply Equal to");
                return 0;
            }
            return -1;
        case '%':
            if (isEndOfToken(lookAhead)) {
                createToken("Arithematic Op: %% -> Modulus");
                return 0;
            }
            return -1;
        default:
            return -1;
    }
}

int handleState0StringCharLiterals(const char current) {
    if (current == '\"') {
        return 10;
    }
    if (current == '\'') {
        return 11;
    }
    return -1;
}

int handleState0Brackets(const char current) {
    if (current == ')') {
        createToken("Close Bracket -> Normal");
    } else if (current == '(') {
        createToken("Open Bracket -> Normal");
    } else if (current == ']') {
        createToken("Close Bracket -> Square");
    } else if (current == '[') {
        createToken("Open Bracket -> Square");
    } else if (current == '}') {
        createToken("Close Bracket -> Curly");
    } else if (current == '{') {
        createToken("Open Bracket -> Curly");
    } else {
        return -1;
    }
    return 0;
}

int handleState0Punctuations(const char current) {
    if (current == ',') {
        createToken("Seperator -> Comma");
    } else if (current == '.') {
        createToken("Dot");
    } else if (current == ';') {
        createToken("Terminator -> Semicolon");
    } else if (current == ':') {
        createToken("Colon");
    } else {
        return -1;
    }
    return 0;
}

int handleState0HashSymbol(const char current) {
    if (current == '#') {
        return 12;
    }
    return -1;
}

__uint8_t handleState6IdentifierKeyword(const char current, const char lookAhead, char currentBuff[100]) {
    // Returns shouldShift too
    if (isalnum(current) || current == '_') {
        strncat(currentBuff, &current, 1);
        return 6 | shouldShiftMask;
    }
    if (isKeyword(currentBuff)) {
        createTokenWithAdditionalValue("Keyword found", currentBuff);
    } else {
        createTokenWithAdditionalValue("Identifier found", currentBuff);
    }
    strcpy(currentBuff, "");
    return 0;
}

__uint8_t handleState7Integer(const char current, const char lookAhead, char currentBuff[100]) {
    // Returns shouldShift too
    if (isdigit(current)) {
        strncat(currentBuff, &current, 1);
        return 7 | shouldShiftMask;
    }
    createTokenWithAdditionalValue("Integer Literal found", currentBuff);
    strcpy(currentBuff, "");
    return 0;
}

__uint8_t handleState8SingleLineComment(const char current) {
    if (current != '\n') {
        return 8;
    }
    return 0;
}

__uint8_t handleState9MultiLineComment(char *current, char lookAhead) {
    if (*current == '*' && lookAhead == '/') {
        *current = -1;
        return 0;
    }
    if (lookAhead == -1) {
        error("Expected end of comment");
    }
    return 9;
}

__uint8_t handleState10StringLiteral(const char current, const char lookAhead, char strBuff[100],  __uint8_t *isLastBackSlash) {
    if (current == '\"' && !*isLastBackSlash) {
        createTokenWithAdditionalValue("String Literal", strBuff);
        strcpy(strBuff, "");
        return 0;
    }
    if (current == '\\') {
        *isLastBackSlash = !*isLastBackSlash;
    } else {
        *isLastBackSlash = 0;
    }
    if (lookAhead == -1) {
        error("Expected end of String");
    }
    strncat(strBuff, &current, 1);
    return 10;
}

__uint8_t handleState11CharLiteral(char* current, const char lookAhead, char strBuff[100], __uint8_t *isLastBackSlash) {
    if (*current == '\'' && !*isLastBackSlash) {
        createToken("Char literal: Empty");
        return 0;
    }
    *isLastBackSlash = !*isLastBackSlash && *current == '\\';
    if (lookAhead != '\'' && !*isLastBackSlash) {
        error("Char literal can\'t have more than 1 character");
    }
    strncat(strBuff, current, 1);
    if (!*isLastBackSlash) {
        createTokenWithAdditionalValue("Char literal", strBuff);
        strcpy(strBuff, "");
        *current = -1;
        return 0;
    }
    return 11;
}

__uint8_t handleState12HashSymbol(char current, char strBuff[100]) {
    // Returns shouldShift too
    if (!isalpha(current)) {
        if (strcmp(strBuff, "include") == 0) {
            createToken("Linkage: include -> Include header file");
            strcpy(strBuff, "");
            // Switch to state 13 without shifting
            return 13;
        } else if (strcmp(strBuff, "define") == 0) {
            createToken("Macro: define -> Macro definition");
            strcpy(strBuff, "");
            return 0;
        }
    }
    strncat(strBuff, &current, 1);
    return 12 | shouldShiftMask;
}

__uint8_t handleState13IncludeLt(char current) {
    if (current == ' ') {
        return 13;
    }
    if (current == '<') {
        createToken("Linkage: < Begin header filename");
        return 14;
    }
    error("Expected \'<\'");
    return 0;
}

__uint8_t handleState14IncludeName(char current, char strBuff[100]) {
    if (isalnum(current) || current == '.' || current == '_' || current == '-') {
        strncat(strBuff, &current, 1);
        return 14;
    }
    if (isEndOfToken(current)) {
        error("Expected \'>\'");
        strcpy(strBuff, "");
        return 0;
    }
    if (current == '>') {
        createTokenWithAdditionalValue("Linkage: header file", strBuff);
        createToken("Linkage: > -> End header filename");
        strcpy(strBuff, "");
        return 0;
    }
}

void lex(const char* fileName) {
    FILE* sourceFile = fopen(fileName, "r");
    if (sourceFile == NULL) {
        return error("File can\'t be opened");
    }
    __uint8_t state = 0, isEOF = 0, shouldShift = 1, isLastBackSlashInLiteral = 0;
    int temp;
    char current = -1, lookAhead = -1;
    char currentBuff[100] = "";
    while (1) {
        if (current == -1) {
            // Window slide by 2
            current = readToken(sourceFile);
        } else if (shouldShift) {
            // Slide by 1
            current = lookAhead;
        }
        if (shouldShift)
            lookAhead = readToken(sourceFile);
        if (current == -1) {
            break;
        }
        shouldShift = 1;
        switch (state) {
            case 0:
                if (isEndOfToken(current)) {
                    break;
                }
                temp = handleState0NonArithematicOp(&current, lookAhead);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0IdentierKeywordInteger(current, currentBuff);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0FwdSlash(&current, lookAhead);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0Arithematic(&current, lookAhead);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0StringCharLiterals(current);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0Brackets(current);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0Punctuations(current);
                if (temp != -1) {
                    state = temp;
                    break;
                }
                temp = handleState0HashSymbol(current); 
                if (temp != -1) {
                    state = temp;
                    break;
                }
                break;
            case 6:
                // Identifier or Keyword
                temp = handleState6IdentifierKeyword(current, lookAhead, currentBuff);
                shouldShift = getShouldShift(temp);
                state = getState(temp);
                break;
            case 7:
                // Integer Literal
                temp = handleState7Integer(current, lookAhead, currentBuff);
                shouldShift = getShouldShift(temp);
                state = getState(temp);
                break;
            case 8:
                state = handleState8SingleLineComment(current);
                break;
            case 9:
                state = handleState9MultiLineComment(&current, lookAhead);
                break;
            case 10:
                state = handleState10StringLiteral(current, lookAhead, currentBuff, &isLastBackSlashInLiteral);
                break;
            case 11:
                state = handleState11CharLiteral(&current, lookAhead, currentBuff, &isLastBackSlashInLiteral);
                break;
            case 12:
                temp = handleState12HashSymbol(current, currentBuff);
                state = getState(temp);
                shouldShift = getShouldShift(temp);
                break;
            case 13:
                state = handleState13IncludeLt(current);
                break;
            case 14:
                state = handleState14IncludeName(current, currentBuff);
                break;
        }
    }
    fclose(sourceFile);
}

int main(int argc, char** argv) {
    if (argc > 2) {
        outputFp = fopen(argv[2], "w");
    } else {
        outputFp = stdout;
    }
    if (argc == 1) {
        printf("Usage: input-file [output-file]\n");
    } else {
        lex(argv[1]);
    }
    fclose(outputFp);
    return 0;
}
