#pragma once

#include "../ast.hpp"

namespace claire::parser {
  template <typename... T>
  class Visitor;

  template <typename T>
  class Visitor<T> {
  public:
    virtual void visit(T &visitable) = 0;
  };

  template <typename T, typename... Rest>
  class Visitor<T, Rest...> : public Visitor<Rest...> {
  public:
    using Visitor<Rest...>::visit;

    virtual void visit(T &visitable) = 0;
  };

  class ASTVisitor
    : public Visitor<StringExpr, IdentifierExpr, FunctionCallExpr, AccessExpr> {};
} // namespace claire::parser
