#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>
#include "lexer.h"
#include "utils.h"
#include <math.h>
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
            addTk(MUL);
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
                if (*pch == '\'') {
        //printf("CHAR\n");
        tk=addTk(CHAR);
        pch++;
        int index=0;
        char val;
        while (*pch != '\''){
            val=*pch;
            pch++;
            index++;
            
        }
        if(index!=1) err("The char value is not a string\n");
            pch++;
            tk->c=val;
    } else if (*pch == '"') {
        //printf("STRING\n");
        char* val=(char*)malloc(100*sizeof(char));
        tk=addTk(STRING);
        
        pch++;
        int i=0;
        while (*pch != '"'){
            val[i++]=*pch;
            pch++;
        }
            pch++;
            
            val[i]='\0';
            tk->text=val;
            printf("%s\n", tk->text);
    } else {
        //printf("INT/DOUBLE\n");
        char *endptr;
        char *endptri;
        double f = strtod(pch, &endptr);
        int i=strtol(pch,&endptri,10);
        //printf("%f\n",f);
        if (endptr != pch) {
            if (endptr!=endptri){
                tk=addTk(DOUBLE);
                tk->d=f;
            }
            else{
                tk=addTk(INT);
                tk->i=i;
            }
            pch = endptr;
            }
        }
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
        
        printf("%d %s", tk->line, tokenStrings[tk->code]);
        switch(tk->code){
            case 0:printf(":%s\n", tk->text); break;
            case 34: printf(":%d\n", tk->i);break;
            case 35:printf(":%f\n", tk->d);break;
            case 36:printf(":'%c'\n", tk->c); break;
            case 37: printf(":\"%s\"\n", tk->text); break;
            default: printf("\n");
        }
    }
}
