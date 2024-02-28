#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>
#include "lexer.h"
#include "utils.h"
#include <stdlib.h>

Token *tokens; // single linked list of tokens
Token *lastTk; // the last token in list

int line = 1; // the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
    Token *tk = safeAlloc(sizeof(Token));
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastTk)
    {
        lastTk->next = tk;
    }
    else
    {
        tokens = tk;
    }
    lastTk = tk;
    return tk;
}

char *extract(const char *begin, const char *end)
{
    int len = end - begin;
    char *text = malloc(len + 1);
    if (text == NULL)
    {
        err("Memory allocation failed in extract");
    }
    strncpy(text, begin, len);
    text[len] = '\0';
    return text;
}

void regexCheck(const char *pch)
{
    regex_t regex;
    int reti;

    const char *int_pattern = "[0-9]+";
    const char *double_pattern = "[0-9]+ ( '.' [0-9]+ ( [eE] [+-]? [0-9]+ )? | ( '.' [0-9]+ )? [eE] [+-]? [0-9]+ )";
    const char *char_pattern = "['] [^'] [']";
    const char *string_pattern = "\"[^\"]*\"";

    reti = regcomp(&regex, int_pattern, REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex for INT\n");
        exit(EXIT_FAILURE);
    }

    reti = regexec(&regex, pch, 0, NULL, 0);
    if (!reti)
    {
        addTk(INT);
    }
    else
    {

        reti = regcomp(&regex, double_pattern, REG_EXTENDED);
        if (reti)
        {
            fprintf(stderr, "Could not compile regex for DOUBLE\n");
            exit(EXIT_FAILURE);
        }
        reti = regexec(&regex, pch, 0, NULL, 0);
        if (!reti)
        {
            addTk(DOUBLE);
        }
        else
        {

            reti = regcomp(&regex, char_pattern, REG_EXTENDED);
            if (reti)
            {
                fprintf(stderr, "Could not compile regex for CHAR\n");
                exit(EXIT_FAILURE);
            }
            reti = regexec(&regex, pch, 0, NULL, 0);
            if (!reti)
            {
                addTk(CHAR);
            }
            else
            {

                reti = regcomp(&regex, string_pattern, REG_EXTENDED);
                if (reti)
                {
                    fprintf(stderr, "Could not compile regex for STRING\n");
                    exit(EXIT_FAILURE);
                }
                reti = regexec(&regex, pch, 0, NULL, 0);
                if (!reti)
                {
                    addTk(STRING);
                }
                else
                {
                    err("invalid char: %c (%d)", *pch, *pch);
                }
            }
        }
    }
}
Token *tokenize(const char *pch)
{
    // printf("1\n");
    const char *start;
    Token *tk;
    for (;;)
    {
        // printf("%d\n", i++);
        switch (*pch)
        {
        case ' ':
        case '\t':
            pch++;
            break;
        case '\r': // handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
            if (pch[1] == '\n')
                pch++;
            // fallthrough to \n
        case '\n':
            line++;
            pch++;
            break;
        case '\0':
        {
            addTk(END);
            return tokens;
        }
        case EOF:
        {
            addTk(END);
            return tokens;
        }
        case ',':
            addTk(COMMA);
            pch++;
            break;
        case ';':
            addTk(SEMICOLON);
            pch++;
            break;
        case '(':
            addTk(LPAR);
            pch++;
            break;
        case ')':
            addTk(RPAR);
            pch++;
            break;
        case '[':
            addTk(LBRACKET);
            pch++;
            break;
        case ']':
            addTk(RBRACKET);
            pch++;
            break;
        case '{':
            addTk(LACC);
            pch++;
            break;
        case '}':
            addTk(RACC);
            pch++;
            break;
        case '=':
            if (pch[1] == '=')
            {
                addTk(EQUAL);
                pch += 2;
            }
            else
            {
                addTk(ASSIGN);
                pch++;
            }
            break;
        case '+':
        {
            addTk(ADD);
            pch++;
            break;
        }
        case '-':
        {
            addTk(SUB);
            pch++;
            break;
        }
        case '*':
        {
            addTk(SUB);
            pch++;
            break;
        }
        case '/':
        {
            if (pch[1] == '/')
            {

                while (*pch != '\n')
                    pch++;
            }
            else
            {
                addTk(DIV);
                pch++;
            }
            break;
        }
        case '.':
        {
            addTk(DOT);
            pch++;
            break;
        }
        case '&':
        {
            addTk(AND);
            pch += 2;
            break;
        }
        case '|':
        {
            addTk(OR);
            pch += 2;
            break;
        }
        case '!':
        {
            if (pch[1] == '=')
            {
                addTk(NOTEQ);
                pch += 2;
            }
            else
            {
                addTk(NOT);
                pch++;
            }
            break;
        }
        case '<':
        {
            if (pch[1] == '=')
            {
                addTk(LESSEQ);
                pch += 2;
            }
            else
            {
                addTk(LESS);
                pch++;
            }
            break;
        }
        case '>':
        {
            if (pch[1] == '=')
            {
                addTk(GREATEREQ);
                pch += 2;
            }
            else
            {
                addTk(GREATER);
                pch++;
            }
            break;
        }

        default:
        {

            
            if (isalpha(*pch) || *pch == '_')
            {
                for (start = pch; isalnum(*pch) || *pch == '_'; pch++)
                {
                }
                char *text = extract(start, pch);

                if (strcmp(text, "char") == 0)
                    addTk(TYPE_CHAR);
                else if (strcmp(text, "int") == 0)
                    addTk(TYPE_INT);
                else if (strcmp(text, "double") == 0)
                    addTk(TYPE_DOUBLE);
                else if (strcmp(text, "if") == 0)
                    addTk(IF);
                else if (strcmp(text, "else") == 0)
                    addTk(ELSE);
                else if (strcmp(text, "return") == 0)
                    addTk(RETURN);
                else if (strcmp(text, "struct") == 0)
                    addTk(STRUCT);
                else if (strcmp(text, "void") == 0)
                    addTk(VOID);
                else if (strcmp(text, "while") == 0)
                    addTk(WHILE);
                else
                {
                    tk = addTk(ID);
                    tk->text = text;
                }
            }
            else
            {
                regexCheck(pch);
                pch++;
            }
        }
        }
    }
}

void showTokens(const Token *tokens)
{
    const char *tokenStrings[] = {
        "ID",

        "TYPE_CHAR", "TYPE_DOUBLE", "TYPE_INT", "IF", "ELSE", "RETURN", "STRUCT", "VOID", "WHILE",

        "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END",

        "ASSIGN", "EQUAL", "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ",

        "INT", "DOUBLE", "CHAR", "STRING"};
    for (const Token *tk = tokens; tk; tk = tk->next)
    {
        printf("%d %s\n", tk->line, tokenStrings[tk->code]);
    }
}
