#include "parser.hpp"

namespace claire {
  std::unique_ptr<ASTNode> Parser::parse(std::vector<Token> const &tokens) {
    return parse_program_decl(tokens);
  }

  // TODO(rihtwis-weard): can use coroutines for lexing
  std::unique_ptr<ProgramDecl> Parser::parse_program_decl(
    std::vector<Token> const &tokens) {
    // Program = Decls
    auto program_node = std::make_unique<ProgramDecl>();
    auto curr_tok     = tokens.begin();

    // Decls = (Decl | Stmt | Expr)+
    std::unique_ptr<ASTNode> node = std::make_unique<ASTNode>();
    while (curr_tok != tokens.end()) {
      switch (curr_tok->kind) {
      case TokenKind::eIdentifier:
        curr_tok++;
        if (curr_tok != tokens.end()) {
          // TODO(rihtwis-weard): do more specific lexing upfront to simplify parsing
          if (curr_tok->kind == TokenKind::eOperator) {
            node.reset(parse_call_expr());
            break;
          }
        } else {
          throw std::runtime_error{"expected token after identifier!"};
        }
        break;
      default:
        break;
      }
    }
    return program_node;
  }
} // namespace claire
