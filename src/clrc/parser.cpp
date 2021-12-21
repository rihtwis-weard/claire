#include <iostream>

#include "parser.hpp"

namespace claire {

  auto Parser::next_state(ParseState prev, Token const &token) {
    return static_cast<ParseState>(parse_trans[utype(prev) + utype(token.kind)]);
  }

  std::unique_ptr<ASTNode> Parser::parse(std::vector<Token> const &tokens) {
    auto root  = std::make_unique<ASTNode>();
    auto state = ParseState::eNewScope;

    //      IO.puts("Hello world!")
    for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {

      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
        return root;
      }

      switch (state) {
      case ParseState::eExpr: {
        auto expr = parse_expr(state, tok);
        std::cout << "Expression: | " << tok->repr << " |" << std::endl;
        break;
      }
      default:
        break;
      }
    }

    return root;
  }

  std::unique_ptr<Expr> Parser::parse_expr(
    ParseState &state, std::vector<Token>::const_iterator tok) {
    auto node = std::make_unique<Expr>(tok->repr);

    for (; state > ParseState::eFinal; state = next_state(state, *(++tok))) {
      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
        return node;
      }

      switch (state) {

      default:
        break;
      }
    }

    return node;
  }

} // namespace claire
