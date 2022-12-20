%{
int wordCount = 0, lineCount = 0, charCount = 0;
%}
%%
\n { lineCount++; wordCount++; }
[ \t] { wordCount++; }
. { charCount++; }
%%

int yywrap() {

}

int main() {
    yylex();
    printf("Number of characters: %d\nNumber of words: %d\nNumber of lines: %d", charCount, wordCount, lineCount);
    return 0;
}