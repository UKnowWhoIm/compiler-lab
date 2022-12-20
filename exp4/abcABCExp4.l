%%
abc { printf("ABC"); }
%%

int yywrap() {

}

int main() {
    yylex();
    return 0;
}