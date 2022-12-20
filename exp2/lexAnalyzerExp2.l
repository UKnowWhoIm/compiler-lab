%%
(if|else|int|char|float|double|switch) { printf("Keyword: %s\n", yytext); }
\".+\" { printf("String literal: %s\n", yytext); }
\'(.?|[\\]).\' { printf("Char literal: %s\n", yytext); } 
[a-zA-Z_][a-zA-Z_0-9]* { printf("Identifier: %s\n", yytext); }
(\(|\)|\{|\}|\[|\]) { printf("Brackets: %s\n", yytext); }
; { printf("Semicolon: %s\n", yytext); }
[\+|\-|\*|\/|\^|\%] { printf("Arithemetic Operator: %s\n", yytext); }
(!|&&|\|\|) { printf("Logical Operator: %s\n", yytext); }
(==|!=|<=|>=|<|>) { printf("Relational Operator: %s\n", yytext); }
[\+|\-]?0[0-7]+ { printf("Octal Literal: %s\n", yytext); }
[\+|\-]?0x[0-9a-fA-F]+ { printf("Octal Literal: %s\n", yytext); }
[\+|\-]?[0-9]+ { printf("Integer Literal: %s\n", yytext); }
[ \t\r\n] { /* ignore whitespace */ }
%%

int yywrap() {

}

int main() {
    yylex();
    return 0;
}