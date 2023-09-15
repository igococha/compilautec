#ifndef AST_EVALUATOR
#define AST_EVALUATOR

#include "exp_ast.hh"
#include "ast_visitor.hh"

class ASTEvaluator : public ASTVisitor {
public:
  int eval(Exp*);
  // metodos visit

};


#endif
