#pragma once

#include <iomanip>
#include <memory>
#include <utility>
#include <variant>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "../utils.hpp"
#include "../visitor.hpp"

namespace claire::parser {

  class ASTVisitor;

  class ASTNode;
  class ProgramDecl;
  class IdentifierExpr;
  class StringExpr;
  class FunctionCallExpr;
  class ModuleDecl;
  class ExternDecl;

  // TODO(rihwis-weard): can use custom visitor pattern instead of `std::visit` and `std::variant`
  //                     and use virtual dispatch
  using ASTNodeVariant = std::variant<ASTNode const *, StringExpr const *,
    IdentifierExpr const *, FunctionCallExpr const *, ProgramDecl const *,
    ModuleDecl const *, ExternDecl const *>;

  class ASTNode {
  protected:
    std::string                           id_;
    std::vector<std::unique_ptr<ASTNode>> children_;

  public:
    explicit ASTNode(std::string id = "")
      : id_{std::move(id)} {
    }

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

    [[nodiscard]] virtual std::string const &id() const {
      return id_;
    }

    [[nodiscard]] virtual std::vector<std::unique_ptr<ASTNode>> const &children() const {
      return children_;
    }

    [[nodiscard]] virtual ASTNodeVariant as_variant() const {
      return this;
    }
  };

  class Decl : virtual public ASTNode {};

  class Expr : virtual public ASTNode {};

  class ProgramDecl : public Decl {

  public:
    explicit ProgramDecl(std::string id)
      : ASTNode{std::move(id)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "ProgramDecl";
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class ModuleDecl : public Decl {
  public:
    explicit ModuleDecl(std::string id)
      : ASTNode{std::move(id)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "ModuleDecl: " + id_;
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  struct FunctionArg {
    std::string name;
    std::string type;
  };

  class StringExpr : public Expr {
  public:
    explicit StringExpr(std::string id)
      : ASTNode{std::move(id)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "StringLiteral: " + id_;
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }

    [[nodiscard]] std::string value() const {
      return id_.substr(1, id_.size() - 2);
    }
  };

  class ExternDecl : public Decl {
    std::vector<FunctionArg>    args_;
    std::string                 return_type_;
    std::unique_ptr<StringExpr> linkage_name_;

  public:
    ExternDecl(std::string id, std::vector<FunctionArg> &&args, std::string return_type,
      std::unique_ptr<StringExpr> &&linkage_name)
      : ASTNode{std::move(id)}
      , args_{std::move(args)}
      , return_type_{std::move(return_type)}
      , linkage_name_{std::move(linkage_name)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "ExternDecl: " + id_;
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }

    [[nodiscard]] std::vector<FunctionArg> const &args() const {
      return args_;
    }

    [[nodiscard]] std::string return_type() const {
      return return_type_;
    }

    [[nodiscard]] std::string linkage_name() const {
      return linkage_name_->value();
    }
  };

  class IdentifierExpr : public Expr {

  public:
    explicit IdentifierExpr(std::string id)
      : ASTNode{std::move(id)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return id_;
    }
  };

  class ModuleAccessExpr : public Expr {
    std::vector<std::string> module_names_;

  public:
    explicit ModuleAccessExpr(IdentifierExpr const &expr)
      : ASTNode{expr.id()} {
    }

    [[nodiscard]] std::string to_string() const override {
      return join(module_names_.begin(), module_names_.end(), ".") + "." + id_;
    }

    [[nodiscard]] std::string const &module_name() const {
      // TODO(rihtwis-weard): support for nested modules
      return module_names_[0];
    }

    void grow(std::string const &id) {
      module_names_.push_back(id_);
      id_ = id;
    }
  };

  class FunctionCallExpr : public Expr {
    std::unique_ptr<Expr> callee_;

  public:
    explicit FunctionCallExpr(std::unique_ptr<Expr> &&callee)
      : callee_{std::move(callee)} {
    }

    [[nodiscard]] std::string to_string() const override {
      return "FunctionCallExpr: " + callee_->to_string();
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }

    [[nodiscard]] Expr const *callee() const {
      return callee_.get();
    }
  };

  class ASTVisitor
    : public Visitor<llvm::Value *, ASTNode, ProgramDecl, StringExpr, IdentifierExpr,
        FunctionCallExpr, ModuleDecl, ExternDecl> {

  public:
    llvm::Value *operator()(ASTNode const *) override {
      return nullptr;
    }
  };

} // namespace claire::parser
