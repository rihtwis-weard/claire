#pragma once

#include <iomanip>
#include <memory>
#include <utility>

namespace claire {
  class ASTNode {
    std::size_t level_;

  protected:
    std::vector<std::unique_ptr<ASTNode>> children_;

  public:
    explicit ASTNode(std::size_t level = 0)
      : level_{level} {
    }

    void add(std::unique_ptr<ASTNode> &&node) {
      node->level_ = level_ + 1;
      for (auto &child : children_) {
        child->level_ = level_ + 2;
      }
      children_.emplace_back(std::move(node));
    }

    //    void add(std::vector<std::unique_ptr<ASTNode>> &&nodes) {
    //      children_.insert(children_.end(), std::make_move_iterator(nodes.begin()),
    //        std::make_move_iterator(nodes.end()));
    //    }

    [[nodiscard]] virtual std::string to_string() const {
      return "ASTNode: root";
    }

    [[nodiscard]] std::string pretty_print() const {
      std::string repr{};
      for (std::size_t i = 0; i < level_; i++) {
        repr += "    ";
      }
      repr += "└──" + to_string() + "\n";
      for (auto const &child : children_) {
        for (std::size_t i = 0; i < child->level_; i++) {
          repr += "    ";
        }
        repr += "├──" + child->to_string() + "\n";
      }
      return repr;
    }
  };

  class Expr : public ASTNode {
  public:
    explicit Expr()
      : ASTNode{} {
    }
  };

  class StringExpr : public Expr {
    std::string name_;

  public:
    explicit StringExpr(std::string name)
      : Expr{}
      , name_{std::move(name)} {
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
      for (auto const &child : children_) {
        repr += child->to_string() + ".";
      }
      return "AccessExpression: " + repr;
    }
  };

  class FunctionCallExpr : public Expr {
    std::unique_ptr<Expr> callee_;

  public:
    explicit FunctionCallExpr(std::unique_ptr<Expr> &&callee)
      : Expr{}
      , callee_{std::move(callee)} {
    }

    [[nodiscard]] std::string to_string() const override {
      std::string repr{"FunctionCall: " + callee_->to_string() + "\n"};
      for (auto const &child : children_) {
        repr += "├──" + child->to_string() + "\n";
      }
      return repr;
    }
  };

} // namespace claire
