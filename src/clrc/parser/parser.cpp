#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

namespace claire::parser {

  auto Parser::next_state(ParseState prev, Token const &token) {
    return static_cast<ParseState>(
      parse_trans[utype(prev) + (utype(token.kind) * utype(ParseState::eCount))]);
  }

  template <typename ASTNodeType>
  std::unique_ptr<ASTNode> Parser::parse(std::vector<Token> const &tokens) {
    auto root  = std::make_unique<ASTNodeType>();
    auto state = ParseState::eNewScope;

    //      open IO
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
      case ParseState::eOpenModuleDecl: {
        // TODO(rihtwis-weard): elide `open ModuleName` declaration for stdlib modules
        root->add(parse_open_module_decl(state, tok));
        break;
      }
      default:
        break;
      }
    }

    return root;
  }

  template std::unique_ptr<ASTNode> Parser::parse<ProgramDecl>(
    std::vector<Token> const &tokens);

  template std::unique_ptr<ASTNode> Parser::parse<ModuleDecl>(
    std::vector<Token> const &tokens);

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

  std::unique_ptr<ASTNode> Parser::parse_open_module_decl(
    ParseState &state, std::vector<Token>::const_iterator tok) {

    auto module_root = std::make_unique<ASTNode>();

    for (; state > ParseState::eFinal;) {
      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
        return module_root;
      }

      if (state == ParseState::eIdentifierExpr) {
        // TODO(rihwis-weard): correct way to cache repeated ASTNode/Module resolutions and returning
        //                     unique_ptr to ASTNode? May need to use shared_ptr/weak_ptrs all around
        //        if (auto mod = mod_map_.find(tok->repr); mod != mod_map_.end()) {
        //          return std::unique_ptr<ASTNode>{mod->second.get()};
        //        }

        // TODO(rihwis-weard): don't hard-code module lookup/resolution
        // distribute module registry, file -> module names
        auto tokens = Lexer{stdlib_path_ + "io.clr"}.lex();

        // TODO(rihtwis-weard): module not found exception or save error and keep parsing
        return parse<ModuleDecl>(tokens);
      }
    }

    return module_root;
  }

} // namespace claire::parser
