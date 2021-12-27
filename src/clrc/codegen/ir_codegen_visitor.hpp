#pragma once

#include "../parser/ast_visitor.hpp"

namespace claire::codegen {
  class IRCodegenVisitor : public parser::ASTVisitor {
  public:
    void visit(StringExpr &expr) override {
    }
  };
} // namespace claire::codegen
