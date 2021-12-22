#pragma once

#include <vector>

#include "ast.hpp"
#include "state_machine.h"
#include "token.h"

namespace claire {

  class Parser {
  public:
    std::unique_ptr<ASTNode> parse(std::vector<Token> const &tokens);

  private:
    std::unique_ptr<Expr> parse_identifier_expr(
      ParseState &state, std::vector<Token>::const_iterator tok);

    std::unique_ptr<Expr> parse_access_expr(
      std::vector<Token>::const_iterator tok, std::unique_ptr<IdentifierExpr> &&expr);

    std::unique_ptr<Expr> parse_function_call_expr(
      std::vector<Token>::const_iterator tok, std::unique_ptr<Expr> &&callee);

    auto next_state(ParseState prev, Token const &token);
  };
} // namespace claire
