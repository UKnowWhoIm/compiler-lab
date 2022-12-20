%{
int yylex(void);
void yyerror(char *);
%}

%token INT

%%
S: S E '\n' { printf("Valid Expression\n"); }| ;
E: E '+' T | E '-' T | T;
T: T '*' F | T '/' F | F;
F: INT | '(' E ')';
%%

void yyerror(char* error) {
    printf("Error: %s", error);
}

int main(void)
{
    yyparse();
    return 0;
}
