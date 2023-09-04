%{
#include <iostream>
using namespace std;
%}
digito [0-9]
numero {digito}+
%%
{numero}  cout << "numero " << yytext << endl;
\n        cout << "new line" << endl;
[^a]      cout << "char not a: " << yytext << endl;
a         cout << "Encontramos a" << endl;

%%
int main() {
  yylex();
  cout << "FIN" << endl;
}
