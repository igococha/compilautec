#include "ast_evaluator.hh"

int ASTEvaluator::eval(Exp* e) {
  return e->accept(this);
}

int ASTEvaluator::visit(BinaryExp* e) {
  // mirar codigo en exp.cpp
  return result;
}

int ASTEvaluator::visit(NumberExp* e) {
  // return ????;
}

int ASTEvaluator::visit(ParenthExp* ep) { 
  // return ???;
}
