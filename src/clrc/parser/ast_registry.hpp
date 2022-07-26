
// This code is auto-generated by generate_ast_node_registry.py, do not manually modify!!!
#pragma once
#include <variant>

namespace claire::parser {
class ASTNode;
class ProgramDecl;
class IdentifierExpr;
class StringExpr;
class FunctionCallExpr;
class IdentifierSeq;
class ExpressionSequence;
class NamespaceAccessExpr;


using ASTNodeVariant = std::variant<ASTNode const *,ProgramDecl const *,IdentifierExpr const *,StringExpr const *,FunctionCallExpr const *,IdentifierSeq const *,ExpressionSequence const *,NamespaceAccessExpr const *>;

template <typename R>
class ASTVisitor : public Visitor<R, ASTNode,ProgramDecl,IdentifierExpr,StringExpr,FunctionCallExpr,IdentifierSeq,ExpressionSequence,NamespaceAccessExpr> {

public:
  R operator()(ASTNode const *) override {
    return nullptr;
  }

};

} // namespace claire::parser

