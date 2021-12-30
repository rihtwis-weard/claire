#pragma once

#include "ast.hpp"

namespace claire::parser {

  class ASTPrettyPrinter : ASTVisitor<std::string> {

  public:
    std::string pretty_print(ASTNode *node) {
      auto repr = indent(node->level());
      auto name = std::visit(*this, node->as_variant());
      repr += "└──" + name + "\n";

      for (auto const &child : node->children()) {
        repr += pretty_print(child.get());
      }
      return repr;
    }

    static std::string indent(std::size_t level) {
      std::string s{};
      for (std::size_t i = 0; i < level; i++) {
        s += "\t";
      }
      return s;
    }

    std::string operator()(ASTNode const *) override {
      return "ASTNode: root";
    }

    std::string operator()(ProgramDecl const *) override {
      return "ProgramDecl";
    }

    std::string operator()(ModuleDecl const *decl) override {
      return "ModuleDecl: " + decl->id();
    }

    std::string operator()(StringExpr const *expr) override {
      return "StringLiteral: " + expr->id();
    }

    std::string operator()(ExternDecl const *decl) override {
      return "ExternDecl: " + decl->id();
    }

    std::string operator()(IdentifierExpr const *expr) override {
      return expr->id();
    }

    std::string operator()(ModuleAccessExpr const *expr) override {
      return expr->module_name() + "." + expr->id();
    }

    std::string operator()(FunctionCallExpr const *expr) override {
      return "FunctionCallExpr: " + std::visit(*this, expr->callee()->as_variant());
    }
  };

} // namespace claire::parser
