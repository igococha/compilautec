#ifndef AST_VISITOR
#define AST_VISITOR

#include "exp_ast.hh"

class ASTVisitor {
public:
  virtual int visit(BinaryExp* e) = 0;
  virtual int visit(NumberExp* e) = 0;
  virtual int visit(ParenthExp* e) = 0;
};


#endif
