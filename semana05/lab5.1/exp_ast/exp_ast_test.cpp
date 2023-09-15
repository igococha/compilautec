#include <sstream>
#include <iostream>
#include <iostream>

#include "aexp_parser.hh"

#include "exp_ast.hh"
#include "ast_printer.hh"
//#include "ast_evaluator.hh"

using namespace std;

int main(int argc, const char* argv[]) {

  bool useparser = true;
  Exp *exp=NULL;

  if (useparser) {
  
    if (argc != 2) {
      cout << "Incorrect number of arguments" << endl;
      exit(1);
    }
    
    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    exp = parser.parse(); 
    
  } else {
 
    exp = new BinaryExp( new NumberExp(2), new NumberExp(3), EXP);

  }

  ASTPrinter printer;
  //ASTEvaluator evaluator;
  
  printer.print(exp);
  
  //cout << "eval " << evaluator.eval(exp) << endl;
  
  delete exp;
    

}
