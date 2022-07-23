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

  template <typename R>
  class ASTVisitor;

  class ASTNode;
  class ProgramDecl;
  class IdentifierExpr;
  class StringExpr;
  class FunctionCallExpr;
  class ModuleDecl;
  class ExternDecl;
  class ModuleAccessExpr;
  class FunctionDef;

  // TODO(rihwis-weard): can use custom visitor pattern instead of `std::visit` and `std::variant`
  //                     and use virtual dispatch
  using ASTNodeVariant =
    std::variant<ASTNode const *, StringExpr const *, IdentifierExpr const *,
      FunctionCallExpr const *, ProgramDecl const *, ModuleDecl const *,
      ExternDecl const *, ModuleAccessExpr const *, FunctionDef const *>;

  class ASTNode {
  protected:
    std::string                           id_;
    std::vector<std::unique_ptr<ASTNode>> children_;

#ifdef CTEST
    std::size_t level_;
#endif

  public:
#ifdef CTEST
    explicit ASTNode(std::string id = "", std::size_t level = 0)
      : id_{std::move(id)}
      , level_{level} {
    }
#else
    explicit ASTNode(std::string id = "")
      : id_{std::move(id)} {
    }
#endif

    void add(std::unique_ptr<ASTNode> &&node) {
#ifdef CTEST
      node->update_level(this);
#endif
      children_.emplace_back(std::move(node));
    }

    virtual ~ASTNode() = default;

    [[nodiscard]] virtual std::string const &id() const {
      return id_;
    }

#ifdef CTEST
    [[nodiscard]] virtual std::size_t level() const {
      return level_;
    }
#endif

    [[nodiscard]] virtual std::vector<std::unique_ptr<ASTNode>> const &children() const {
      return children_;
    }

    [[nodiscard]] virtual ASTNodeVariant as_variant() const {
      return this;
    }

  private:
#ifdef CTEST
    void update_level(ASTNode const *parent) {
      level_ = parent->level_ + 1;
      for (auto const &child : children_) {
        child->update_level(this);
      }
    }
#endif
  };

  class Decl : virtual public ASTNode {};

  class Expr : virtual public ASTNode {};

  class ProgramDecl : public Decl {

  public:
    explicit ProgramDecl(std::string id)
      : ASTNode{std::move(id)} {
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

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  struct FunctionArg {
    std::string name;
    std::string type;
  };

  class FunctionBody : public Decl {
  public:
    FunctionBody()
      : ASTNode{} {
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class FunctionDef : public Decl {
    std::unique_ptr<IdentifierExpr> name_;
    std::vector<FunctionArg>        args_;

  public:
    FunctionDef(std::string id, std::unique_ptr<IdentifierExpr> &&name,
      std::vector<FunctionArg> &&args, std::unique_ptr<ASTNode> &&body)
      : ASTNode{std::move(id)}
      , name_{std::move(name)}
      , args_{std::move(args)} {

      add(std::move(body));
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }
  };

  class StringExpr : public Expr {
  public:
    explicit StringExpr(std::string id)
      : ASTNode{std::move(id)} {
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
  };

  class ModuleAccessExpr : public Expr {
    std::vector<std::string> module_names_;

  public:
    explicit ModuleAccessExpr(IdentifierExpr const &expr)
      : ASTNode{expr.id()} {
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
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
    //    std::unique_ptr<Expr> callee_;

  public:
    //    explicit FunctionCallExpr(std::unique_ptr<Expr> &&callee)
    //      : callee_{std::move(callee)} {
    //    }

    explicit FunctionCallExpr() {
    }

    [[nodiscard]] ASTNodeVariant as_variant() const override {
      return this;
    }

    //    [[nodiscard]] Expr const *callee() const {
    //      return callee_.get();
    //    }
  };

  template <typename R = llvm::Value *>
  class ASTVisitor
    : public Visitor<R, ASTNode, ProgramDecl, StringExpr, IdentifierExpr,
        FunctionCallExpr, ModuleDecl, ExternDecl, ModuleAccessExpr, FunctionDef,
        FunctionBody> {

  public:
    R operator()(ASTNode const *) override {
      return nullptr;
    }
  };

} // namespace claire::parser
