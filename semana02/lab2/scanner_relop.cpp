#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

class Scanner {
public:
  Scanner(const char* in_s);
  bool checkRelOp();
  ~Scanner();
  string getLexema();
private:
  string input;
  int first, current;
  int state;
  char nextChar();
  void rollBack();
  void startLexema();
};

Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

/* Matches relational operator form current position */
bool Scanner::checkRelOp() {
  char c;
  state = 0;
  startLexema();
  while(1) {
    switch(state) {
    case 0:
      c = nextChar();
      if (c == '<') state =1;
      else if (c == '=') state = 5;
      else if (c == '>') state = 6;
      else return false;
      break;
    case 1:
      c = nextChar();
      if (c == '=') state =2;
      else if (c == '>') state = 3;
      else  state = 4;
      break;
    case 2: return true;
    case 3: return true;
    case 4: rollBack(); return true;
    case 5: return true;
    case 6:
      c = nextChar();
      if (c == '=') state =7;
      else  state = 8;
      break;
    case 7: return true;
    case 8: rollBack(); return true;
    }
  }
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
  if (current == 0) first = 0;
  else first = current-1;
  return;
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

  Scanner scanner(argv[1]);
  if (scanner.checkRelOp())
    cout << "Matched: " << scanner.getLexema() << endl;
  else
    cout << "Failed to match Relational Operator" << endl;
}


