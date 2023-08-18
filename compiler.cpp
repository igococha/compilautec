#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

/*
  Scanner and Parser code inspired by code in Crafting Interpreters by Robert Nystrom
*/

class Token {
public:
  enum Type { LPAREN, RPAREN, PLUS, MINUS, MULT, DIV, POW, NUM, ERR, END };
  Type type;
  string text;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string& source, int first, int last);
};

class Scanner {
private:
  string input;
  int first, current;
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();

};


enum BinaryOp { PLUS, MINUS, MULT, DIV, POW};

class Exp {
public:
  virtual void print() = 0;
  virtual int eval() = 0;
  virtual void rpn(ostringstream& str) = 0;
  virtual void genCode(ostringstream& str) = 0;
  static char binopToChar(BinaryOp op);
  virtual ~Exp() = 0;
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
  ~BinaryExp();
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
  ~NumberExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
  ~ParenthExp();
};


class Parser {
private:
  Scanner* scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  Exp* parseExpression();
  Exp* parseTerm();
  Exp* parseFactor();
  Exp* parsePrimary();
  bool tokenToOp(Token* tk, BinaryOp& op);
public:
  Parser(Scanner* scanner);
  Exp* parse();
};




Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = c; }

Token::Token(Type type, const string& source, int first, int last):type(type) {
  text = source.substr(first,last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
  if (tok.text.empty())
    return outs << tok.type;
  else
    return outs << tok.type << "(" << tok.text << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
  return outs << *tok;
}

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

Token* Scanner::nextToken() {
  Token* token;
  // consume whitespaces
  while (input[current]==' ') current++;
  if (input[current] == '\0') return new Token(Token::END);
  char c  = input[current];
  first = current;
  if (isdigit(c)) {
    current++;
    while (isdigit(input[current]))
      current++;
    token = new Token(Token::NUM,input,first,current-first);
  } else if (strchr("+-*/^()", c)) {
    switch(c) {
    case '(': token = new Token(Token::LPAREN); break;
    case ')': token = new Token(Token::RPAREN); break;
    case '+': token = new Token(Token::PLUS,c); break;
    case '-': token = new Token(Token::MINUS,c); break;
    case '*': token = new Token(Token::MULT,c); break;
    case '/': token = new Token(Token::DIV,c); break;
    case '^': token = new Token(Token::POW,c); break;
    default: cout << "No deberia llegar aca" << endl;
    }
    current++;
  } else {
    token = new Token(Token::ERR, c);
    current++;
  }
  return token;
}

Scanner::~Scanner() { }

// parser


// match and consume next token
bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd()) return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token* temp =current;
    if (previous) delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (check(Token::ERR)) {
      cout << "Parse error, unrecognised character: " << current->text << endl;
      exit(0);
    }
    return true;
  }
  return false;
} 

bool Parser::isAtEnd() {
  return (current->type == Token::END);
} 

Parser::Parser(Scanner* sc):scanner(sc) {
  previous = current = NULL;
  return;
};

Exp* Parser::parse() {
  // get first token
  current = scanner->nextToken();
  if (check(Token::ERR)) {
      cout << "Error en scanner - caracter invalido" << endl;
      exit(0);
  }
  Exp* exp = parseExpression();
  // cout << "last token: " << current << endl;
  if (current) delete current;
  return exp;
}

Exp* Parser::parseExpression() {
  Exp* exp = parseTerm();
  return exp;
}

Exp* Parser::parseTerm() {
  BinaryOp op;
  Exp* exp = parseFactor();
  while(match(Token::MINUS) || match(Token::PLUS)) {
    tokenToOp(previous, op);
    Exp* right = parseFactor();
    exp = new BinaryExp(exp, right, op);
  }
  return exp;
}

Exp* Parser::parseFactor() {
  BinaryOp op;
  Exp* exp = parsePrimary();
  while(match(Token::MULT) || match(Token::DIV) || match(Token::POW)) {
    tokenToOp(previous, op);
    Exp *right = parsePrimary();
    exp = new BinaryExp(exp, right, op);
  }
  return exp;
}

Exp* Parser::parsePrimary() {
  if (match(Token::NUM)) {
    int v = stoi(previous->text);
    return new NumberExp(v);
  }
  if (match(Token::LPAREN)) {
    Exp *exp = parseExpression();
    if (!match(Token::RPAREN)) {
      cout << "Expecting right parenthesis";
      exit(0);
    }
    return new ParenthExp(exp);
  }
  cout << "Couldn't find match for token: " << current->text << endl;
  exit(0);
  return NULL;
}

bool Parser::tokenToOp(Token* tk, BinaryOp& op)  {
  switch(tk->type) {
  case Token::PLUS: op = PLUS; break;
  case Token::MINUS: op = MINUS; break;
  case Token::MULT: op = MULT; break;
  case Token::DIV: op = DIV; break;
  case Token::POW: op = POW; break;
  default: cout << "Invalid Operator" << endl; return false;
  }
  return true;
}


/* Abstract Syntax Trees for Expressions */

/*
enum BinaryOp { PLUS, MINUS, MULT, DIV, POW};

class Exp {
public:
  virtual void print() = 0;
  virtual int eval() = 0;
  virtual void rpn(ostringstream& str) = 0;
  virtual void genCode(ostringstream& str) = 0;
  static char binopToChar(BinaryOp op);
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  void print();
  int eval();
  void rpn(ostringstream& str);
  void genCode(ostringstream& str);
};
*/

char Exp::binopToChar(BinaryOp op) {
  char  c=' ';
  switch(op) {
  case PLUS: c = '+'; break;
  case MINUS: c = '-'; break;
  case MULT: c = '*'; break;
  case DIV: c = '/'; break;
  case POW: c = '^'; break;
  }
  return c;
}

// Constructors
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
ParenthExp::ParenthExp(Exp *e):e(e){}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
ParenthExp::~ParenthExp(){ delete e; }


// print
void BinaryExp::print() {
  left->print();
  char  c=binopToChar(this->op);;
  cout << ' ' << c << ' ';
  right->print();
}

void NumberExp::print() {
  cout << value;
}

void ParenthExp::print() {
  cout << '(';
  e->print();
  cout << ')';
}

// eval
int BinaryExp::eval() {
  int v1 = left->eval();
  int v2 = right->eval();
  int result = 0;
  switch(this->op) {
  case PLUS: result = v1+v2; break;
  case MINUS: result = v1-v2; break;
  case MULT: result = v1 * v2; break;
  case DIV: result = v1 / v2; break;
  case POW:
    result = 1;
    while (v2 > 0) { result *= v1; v2--; }
    break;
  }
  return result;
}

int NumberExp::eval() {
  return value;
}

int ParenthExp::eval() {
  return e->eval();
}

// rpn
void BinaryExp::rpn(ostringstream& str) {
  left->rpn(str);
  right->rpn(str);
  char c = binopToChar(this->op);
  str << c << ' ';
}

void NumberExp::rpn(ostringstream& str) {
  str << value << ' ';
}

void ParenthExp::rpn(ostringstream& str) {
  e->rpn(str);
}

// genCode
void BinaryExp::genCode(ostringstream& str) {
  left->genCode(str);
  right->genCode(str);
  char  c=' ';
  switch(this->op) {
  case PLUS: str << "add"; break;
  case MINUS: str << "sub"; break;
  case MULT: str << "mult"; break;
  case DIV: str << "div"; break;
  case POW: str << "pop" << endl << "pop" << endl << "push 1"; break;
  }
  str << endl;
}

void NumberExp::genCode(ostringstream& str) {
  str << "push " << value << endl;
}

void ParenthExp::genCode(ostringstream& str) {
  e->genCode(str);
}

// ---------------------------------------------------


// ---------------------------------------------------

int main(int argc, const char* argv[]) {
  
  if (argc != 2 && argc != 3) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  Scanner scanner(argv[1]);

  Parser parser(&scanner);
  Exp *exp = parser.parse();

  cout << "expr: ";
  exp->print();
  cout << endl;

  cout << "eval: ";
  cout << exp->eval() << endl;

  std::ostringstream rpn_str;
  exp->rpn(rpn_str);
  cout << " rpn: " << rpn_str.str() << endl;

  std::ostringstream code_str;
  exp->genCode(code_str);
  delete exp;

  /*
  Token* tk = scanner.nextToken();
  while (tk->type != Token::END) {
    cout << "next token " << tk << endl;
    delete tk;
    tk =  scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;
  */

  ofstream outfile;
  string outfname;
  if  (argc == 3)
    outfname = argv[2];
  else
    outfname = "out.sm";
  cout  << "Writing compiled code to: " << outfname << endl;
  outfile.open(outfname);
  outfile << code_str.str();
  outfile.close();
  
}


