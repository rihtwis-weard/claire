#pragma once

#include <utility>
#include <vector>

#include <robin_hood.h>

#include "ast.hpp"
#include "state_machine.h"
#include "token.h"

namespace claire::parser {

  using token_iterator = std::vector<Token>::const_iterator;

  std::unique_ptr<IdentifierExpr> parse_simple_identifier_expression(token_iterator &tok);

  std::unique_ptr<IdentifierSeq> parse_identifier_sequence(token_iterator &tok);

  std::unique_ptr<FunctionCallExpr> parse_function_call_expression(
    token_iterator &tok, std::unique_ptr<Expr> &&callee);

  class Parser {
    std::string stdlib_path_;
    //    robin_hood::unordered_map<std::string, ASTNode *> mod_map_;

  public:
    explicit Parser(std::string stdlib_path)
      : stdlib_path_{std::move(stdlib_path)} {
    }

    template <typename RootNodeType = ProgramDecl>
    std::unique_ptr<ASTNode> parse(
      std::vector<Token> const &tokens, std::string const &id = "main");

  private:
    std::unique_ptr<IdentifierExpr> parse_identifier_expr(
      std::vector<Token>::const_iterator &tok);
    //
    //    std::unique_ptr<Expr> parse_module_access_expr(
    //      std::vector<Token>::const_iterator tok, IdentifierExpr const &expr);

    //    std::unique_ptr<Expr> parse_function_call_expr(
    //      std::vector<Token>::const_iterator tok, std::unique_ptr<Expr> &&callee);

    //
    //    std::unique_ptr<ASTNode> parse_module_open_stmt(ParseState &state,
    //      std::vector<Token>::const_iterator &tok, std::vector<Token> const &tokens);
    //
    //    std::unique_ptr<ExternDecl> parse_extern_decl(ParseState &state,
    //      std::vector<Token>::const_iterator tok, std::vector<Token> const &tokens);

    std::unique_ptr<FunctionDef> parse_function_def(
      std::vector<Token>::const_iterator &tok);

    std::vector<FunctionArg> parse_function_decl_args(
      std::vector<Token>::const_iterator &tok);

    std::unique_ptr<FunctionBody> parse_function_body(
      std::vector<Token>::const_iterator &tok);
  };

} // namespace claire::parser
