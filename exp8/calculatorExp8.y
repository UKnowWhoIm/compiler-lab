%{
int yylex(void);
void yyerror(char *);
%}

%token INT

%%
S: S E '\n' { printf("Res: %d \n", $2); }| ;
E:  E '+' T { $$ = $1 + $3; }
    | E '-' T { $$ = $1 - $3; }
    | T { $$ = $1; } ;
T:  T '*' F { $$ = $1 * $3; }
    | T '/' F { $$ = $1 / $3; } 
    | F { $$ = $1; };
F:  INT { $$ = $1; }
    | '(' E ')' { $$ = $2; }
%%

void yyerror(char* error) {
    printf("Error: %s", error);
}

int main(void)
{
    yyparse();
    return 0;
}
