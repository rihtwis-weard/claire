#pragma once

#include <memory>

namespace claire {
  class ASTNode {

    std::vector<std::unique_ptr<ASTNode>> children_;

  public:
    void add(std::unique_ptr<ASTNode> &&node) {
      children_.emplace_back(std::move(node));
    }

    void add(std::vector<std::unique_ptr<ASTNode>> &&nodes) {
      children_.insert(children_.end(), std::make_move_iterator(nodes.begin()),
        std::make_move_iterator(nodes.end()));
    }
  };

  class Decl : public ASTNode {};

  class Expr : public ASTNode {};

  class Stmt : public ASTNode {};

  class ProgramDecl : public Decl {};

  class FunctionDecl : public Decl {};

  class VariableDecl : public Decl {};

  class IfStmt : public Stmt {};

  class ReturnStmt : public Stmt {};
} // namespace claire
