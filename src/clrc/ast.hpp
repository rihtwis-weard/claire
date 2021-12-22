#pragma once

#include <iomanip>
#include <memory>
#include <utility>

namespace claire {
  class ASTNode {
  protected:
    std::vector<std::unique_ptr<ASTNode>> children_;

  public:
    void add(std::unique_ptr<ASTNode> &&node) {
      children_.emplace_back(std::move(node));
    }

    [[nodiscard]] virtual std::string to_string() const {
      return "ASTNode: root";
    }

    [[nodiscard]] std::string pretty_print(std::size_t level = 0) const {
      std::string repr{};
      for (std::size_t i = 0; i < level; i++) {
        repr += "\t";
      }
      repr += "└──" + to_string() + "\n";
      for (auto const &child : children_) {
        repr += child->pretty_print(level + 1);
      }
      return repr;
    }
  };

  class Expr : public ASTNode {};

  class StringExpr : public Expr {
    std::string name_;

  public:
    explicit StringExpr(std::string name)
      : name_{std::move(name)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "StringLiteral: " + name_;
    }
  };

  class IdentifierExpr : public Expr {
    std::string name_;

  public:
    explicit IdentifierExpr(std::string name)
      : name_{std::move(name)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return name_;
    }
  };

  class AccessExpr : public Expr {

  public:
    explicit AccessExpr(std::unique_ptr<IdentifierExpr> &&expr) {
      add(std::move(expr));
    }

    [[nodiscard]] std::string to_string() const override {
      std::string repr{};
      for (std::size_t i = 0; i < children_.size(); i++) {
        repr += children_.at(i)->to_string();
        if (i != children_.size() - 1) {
          repr += ".";
        }
      }
      return "AccessExpression: " + repr;
    }
  };

  class FunctionCallExpr : public Expr {
    std::unique_ptr<Expr> callee_;

  public:
    explicit FunctionCallExpr(std::unique_ptr<Expr> &&callee)
      : callee_{std::move(callee)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "FunctionCall: " + callee_->to_string();
    }
  };

} // namespace claire
