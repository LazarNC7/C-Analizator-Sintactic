#include <stdio.h>
#include "utils.h"
#include "lexer.h"

int main()
{
    char* inbuf=loadFile("testlex.txt");
    Token* t=tokenize(inbuf);
    showTokens(t);

    return 0;
}
