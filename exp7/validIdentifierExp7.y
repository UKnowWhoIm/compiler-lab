%{
int yylex(void);
void success();
void yyerror(char *);
%}

%token CHAR
%token DIGIT

%%
S: S ID '\n' { success(); } | S ID ' ' { success(); } | S ID '\t' { success(); } | ;
ID: ALPHA_ | ALPHA_ ALPHA_NUM ; 
ALPHA_: CHAR | CHAR ALPHA_;
ALPHA_NUM: ALPHA_ | DIGIT | ALPHA_ ALPHA_NUM | DIGIT ALPHA_NUM;
%%

void yyerror(char* error) {
    printf("Error: %s", error);
}

void success() {
    printf("\nValid identifier\n");
}

int main(void)
{
    yyparse();
    return 0;
}
