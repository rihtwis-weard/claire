#pragma once

#include <iomanip>
#include <memory>
#include <utility>
#include <variant>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "../visitor.hpp"

namespace claire::parser {

  class ASTVisitor;

  class ASTNode;
  class ProgramDecl;
  class AccessExpr;
  class IdentifierExpr;
  class StringExpr;
  class FunctionCallExpr;
  class ModuleDecl;
  class ExternDecl;

  // TODO(rihwis-weard): can use custom visitor pattern instead of `std::visit` and `std::variant`
  //                     and use virtual dispatch
  using ASTNodeVariant = std::variant<AccessExpr const *, ASTNode const *,
    StringExpr const *, IdentifierExpr const *, FunctionCallExpr const *,
    ProgramDecl const *, ModuleDecl const *, ExternDecl const *>;

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

    virtual ~ASTNode() = default;

    [[nodiscard]] virtual std::vector<std::unique_ptr<ASTNode>> const &children() const {
      return children_;
    }

    [[nodiscard]] virtual ASTNodeVariant as_variant() const {
      return this;
    }
  };

  class Decl : public ASTNode {};

  class ProgramDecl : public Decl {
  public:
    [[nodiscard]] std::string to_string() const override {
      return "ProgramDecl";
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class ModuleDecl : public Decl {
  public:
    [[nodiscard]] std::string to_string() const override {
      return "ModuleDecl";
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class ExternDecl : public Decl {
  public:
    // TODO: needs FunctionArgExprs, method name, and linkage name

    [[nodiscard]] std::string to_string() const override {
      return "ExternDecl";
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
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

    [[nodiscard]] std::string name() const {
      return name_.substr(1, name_.size() - 2);
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
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

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class ASTVisitor
    : public Visitor<llvm::Value *, ASTNode, ProgramDecl, StringExpr, IdentifierExpr,
        FunctionCallExpr, AccessExpr, ModuleDecl, ExternDecl> {

  public:
    llvm::Value *operator()(ASTNode const *) override {
      return nullptr;
    }
  };

} // namespace claire::parser
