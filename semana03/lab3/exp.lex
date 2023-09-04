%{

%}
ws [ \t]+
digit    [0-9]
number   {digit}+
%%
{ws}          { ; }
"("         printf("LPAREN");
")"         printf("RPAREN\n");
"+"         printf("PLUS\n");
"-"         printf("MINUS\n");
"*"         printf("MULT\n");
"/"         printf("DIV\n");
{number}    printf("number: %d\n", atoi(yytext));
.           printf("error: %s\n",  yytext);

%%
int main() {
  yylex();
  printf("Fin\n");
}
