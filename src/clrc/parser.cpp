#include <iostream>

#include "parser.hpp"

namespace claire {

  auto Parser::next_state(ParseState prev, Token const &token) {
    return static_cast<ParseState>(
      parse_trans[utype(prev) + (utype(token.kind) * utype(ParseState::eCount))]);
  }

  std::unique_ptr<ASTNode> Parser::parse(std::vector<Token> const &tokens) {
    auto root  = std::make_unique<ProgramDecl>();
    auto state = ParseState::eNewScope;

    //      IO.puts("Hello world!")
    for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {

      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
        return root;
      }

      switch (state) {
      case ParseState::eIdentifierExpr: {
        root->add(parse_identifier_expr(state, tok));
        break;
      }
      default:
        break;
      }
    }

    return root;
  }

  std::unique_ptr<Expr> Parser::parse_identifier_expr(
    ParseState &state, std::vector<Token>::const_iterator tok) {
    std::unique_ptr<IdentifierExpr> node = std::make_unique<IdentifierExpr>(tok->repr);

    for (; state > ParseState::eFinal;) {
      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
        return node;
      }

      switch (state) {
      case ParseState::eNewAccessExpr: {
        return parse_access_expr(tok, std::move(node));
      }
      default:
        break;
      }
    }

    return node;
  }

  std::unique_ptr<Expr> Parser::parse_access_expr(
    std::vector<Token>::const_iterator tok, std::unique_ptr<IdentifierExpr> &&expr) {

    std::unique_ptr<Expr> node = std::make_unique<AccessExpr>(std::move(expr));

    for (auto state = ParseState::eNewAccessExpr; state > ParseState::eFinal;) {
      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
        return node;
      }

      if (state == ParseState::eGrowAccessExpr) {
        node->add(std::make_unique<IdentifierExpr>(tok->repr));
      } else if (state == ParseState::eFunctionCallExpr) {
        node = parse_function_call_expr(tok, std::move(node));
      }
    }
    return node;
  }

  std::unique_ptr<Expr> Parser::parse_function_call_expr(
    std::vector<Token>::const_iterator tok, std::unique_ptr<Expr> &&callee) {
    auto node = std::make_unique<FunctionCallExpr>(std::move(callee));

    for (auto state = ParseState::eFunctionCallExpr; state > ParseState::eFinal;) {
      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
        return node;
      }

      if (state == ParseState::eFunctionArgs) {
        node->add(std::make_unique<StringExpr>(tok->repr));
      }
    }

    return node;
  }

} // namespace claire
