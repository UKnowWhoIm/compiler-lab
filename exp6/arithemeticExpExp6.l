%{
#include "arithemeticExpExp6.tab.c"
%}

%%
[0-9]+ { return INT; }
[-+\*\/\(\)\n] return *yytext;
[ \t] {}
. yyerror("invalid character\n");
%%

int yywrap(void) {
    return 1;
}
