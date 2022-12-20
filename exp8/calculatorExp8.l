%{
#include <stdio.h>
#include "calculatorExp8.tab.c"
%}

%%
[0-9]+ {  
    yylval = atoi(yytext);
    return INT; 
}
[-+\*\/\(\)\n] return *yytext;
[ \t] {}
. yyerror("invalid character\n");
%%

int yywrap(void) {
    return 1;
}
