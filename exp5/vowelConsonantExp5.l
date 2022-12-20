%{
    int vowelCount = 0;
    int consonantCount = 0;
%}

%%
(a|e|i|o|u|A|E|I|O|U) { vowelCount++; }
[a-zA-Z] { consonantCount++; }
. { }
%%

int yywrap() {

}

int main() {
    yylex();
    printf("Vowels: %d\nConsonant: %d\n", vowelCount, consonantCount);
    return 0;
}