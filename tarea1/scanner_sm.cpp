#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

/*
  Autores:
  Estudiante1
  Estudiante2
*/

// Incluir estructura de datos que permita la busqueda de las palabras reservadas

class Token {
public:
  enum Type { CADENA,  ERR, END };
  static const char* token_names[10]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char* Token::token_names[10] = { "CADENA", "ERR", "END" };

Token::Token(Type type):type(type) { lexema = ""; }


Token::Token(Type type, const string source):type(type) {
  lexema = source;
}

// Modificar
std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
  if (tok.lexema.empty())
    return outs << Token::token_names[tok.type];
  else
    return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
  return outs << *tok;
}


class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  
private:
  string input;
  int first, current;
  int state;
  char nextChar();
  void rollBack();
  void startLexema();
  void incrStartLexema();
  string getLexema();
};


Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

Token* Scanner::nextToken() {
  Token* token;
  char c;
  c = nextChar();
  while (c == ' ' || c == '\t' || c  == '\n') c = nextChar();
  startLexema();
  if (isdigit(c) || isalpha(c)) {
    c = nextChar();
    while (isdigit(c) || isalpha(c)) c = nextChar();
    rollBack();
    return new Token(Token::CADENA,getLexema());
  } else if (c ==  '\0') {
    return new Token(Token::END);
  } else {
    return new Token(Token::ERR,getLexema());
  }
  return NULL;
}

Scanner::~Scanner() { }

char Scanner::nextChar() {
  int c = input[current];
  if (c != '\0') current++;
  return c;
}

void Scanner::rollBack() { // retract
  if (input[current] != '\0')
    current--;
}

void Scanner::startLexema() {
  first = current-1;  
  return;
}

void Scanner::incrStartLexema() {
  first++;
}


string Scanner::getLexema() {
  return input.substr(first,current-first);
}





// ---------------------------------------------------

int main(int argc, const char* argv[]) {
  
  if (argc != 2) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  // Cambiar:
  // input es ahora el nombre de archivo con las instruccion de la SM
  

  Scanner scanner(argv[1]);
  Token* tk = scanner.nextToken();
  while (tk->type != Token::END) {
    cout << "next token " << tk << endl;
    delete tk;
    tk =  scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;

}


