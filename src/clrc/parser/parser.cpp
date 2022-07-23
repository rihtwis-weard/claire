#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

namespace claire::parser {

  auto Parser::next_state(ParseState prev, Token const &token) {
    return static_cast<ParseState>(
      parse_trans[utype(prev) + (utype(token.kind) * utype(ParseState::eCount))]);
  }

  template <typename RootNodeType>
  std::unique_ptr<ASTNode> Parser::parse(
    std::vector<Token> const &tokens, std::string const &id) {
    auto root  = std::make_unique<RootNodeType>(id);
    auto token = tokens.begin();

    switch (token->kind) {
    case TokenKind::eReservedFunc:
      root->add(parse_function_def(token));
      break;
    default:
      break;
    }

    return root;
  }

  std::unique_ptr<FunctionDef> Parser::parse_function_def(
    std::vector<Token>::const_iterator &tok) {
    tok              = std::next(tok);
    auto id          = tok->repr;
    auto symbol_name = std::make_unique<IdentifierExpr>(tok->repr);

    tok       = std::next(tok);
    auto args = parse_function_decl_args(tok);

    // TODO(rw): parse return type decl

    tok = std::next(tok);
    if (tok->kind == TokenKind::eScopeBegin) {
      auto body = parse_function_body(tok);
      return std::make_unique<FunctionDef>(
        id, std::move(symbol_name), std::move(args), std::move(body));
    } else {
      // TODO(rw): parse semicolon for func decl
      throw syntax_error{};
    }
  }

  std::unique_ptr<FunctionBody> Parser::parse_function_body(
    std::vector<Token>::const_iterator &tok) {
    tok       = std::next(tok);
    auto body = std::make_unique<FunctionBody>();

    for (; tok->kind != TokenKind::eScopeEnd;) {
      switch (tok->kind) {
      case TokenKind::eIdentifier:
        body->add(parse_identifier_expr(tok));
        break;
      case TokenKind::eLParens:
        body->add(parse_function_call_expr(tok));
        break;
      default:
        throw syntax_error{};
      }
    }

    return body;
  }

  std::unique_ptr<IdentifierExpr> Parser::parse_identifier_expr(
    std::vector<Token>::const_iterator &tok) {
    // TODO(rw): handle fully qualified names and field/method access

    auto ident = std::make_unique<IdentifierExpr>(tok->repr);
    tok        = std::next(tok);

    // TODO(rw): clean up breaking out of loop
    auto is_identifier_seq = [](auto kind) -> bool {
      return kind == TokenKind::eAccessNamespace or kind == TokenKind::eIdentifier;
    };

    for (; is_identifier_seq(tok->kind); ++tok) {
      switch (tok->kind) {
      case TokenKind::eAccessNamespace:
        tok = std::next(tok);
        ident->add(parse_identifier_expr(tok));
        break;
      default:
        break;
      }
    }
    return ident;
  }

  //  template <typename ASTNodeType>
  //  std::unique_ptr<ASTNode> Parser::parse(
  //    std::vector<Token> const &tokens, std::string const &id) {
  //    auto root  = std::make_unique<ASTNodeType>(id);
  //    // TODO(rihtwis-weard): `NewScope` synonymous with an additional `Program` state?
  //    auto state = ParseState::eNewScope;
  //
  //    for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {
  //      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
  //        return root;
  //      }
  //
  //      switch (state) {
  //      case ParseState::eIdentifierExpr: {
  //        root->add(parse_identifier_expr(state, tok));
  //        break;
  //      }
  //      case ParseState::eModuleOpenStmt: {
  //        // TODO(rihtwis-weard): elide `open ModuleName` declaration for stdlib modules
  //        root->add(parse_module_open_stmt(state, tok, tokens));
  //        state = ParseState::eNewScope;
  //        break;
  //      }
  //      case ParseState::eExternDecl: {
  //        root->add(parse_extern_decl(state, tok, tokens));
  //        break;
  //      }
  //      default:
  //        break;
  //      }
  //    }
  //
  //    return root;
  //  }

  template std::unique_ptr<ASTNode> Parser::parse<ProgramDecl>(
    std::vector<Token> const &tokens, std::string const &id);

  template std::unique_ptr<ASTNode> Parser::parse<ModuleDecl>(
    std::vector<Token> const &tokens, std::string const &id);

  //  std::unique_ptr<Expr> Parser::parse_identifier_expr(
  //    ParseState &state, std::vector<Token>::const_iterator tok) {
  //    std::unique_ptr<IdentifierExpr> node = std::make_unique<IdentifierExpr>(tok->repr);
  //
  //    for (; state > ParseState::eFinal;) {
  //      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
  //        return node;
  //      }
  //
  //      switch (state) {
  //      case ParseState::eNewAccessExpr: {
  //        return parse_module_access_expr(tok, *node);
  //      }
  //      default:
  //        break;
  //      }
  //    }
  //
  //    return node;
  //  }

  //  std::unique_ptr<Expr> Parser::parse_module_access_expr(
  //    std::vector<Token>::const_iterator tok, IdentifierExpr const &expr) {
  //
  //    auto node = std::make_unique<ModuleAccessExpr>(expr);
  //
  //    for (auto state = ParseState::eNewAccessExpr; state > ParseState::eFinal;) {
  //      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
  //        return node;
  //      }
  //
  //      if (state == ParseState::eGrowAccessExpr) {
  //        node->grow(tok->repr);
  //      } else if (state == ParseState::eFunctionCallExpr) {
  //        return parse_function_call_expr(tok, std::move(node));
  //      }
  //    }
  //    return node;
  //  }

  std::unique_ptr<Expr> Parser::parse_function_call_expr(
    std::vector<Token>::const_iterator &tok) {
    auto node = std::make_unique<FunctionCallExpr>();

    // TODO(rw): distinguish right parens and separator
    for (; tok->kind != TokenKind::eSeparator; ++tok) {
      switch (tok->kind) {
        // TODO(rihwis-weard): don't hardcode type / values
      case TokenKind::eStringLiteral:
        node->add(std::make_unique<StringExpr>(tok->repr));
        break;
      default:
        break;
      }
    }

    return node;
  }

  //  std::unique_ptr<Expr> Parser::parse_function_call_expr(
  //    std::vector<Token>::const_iterator tok, std::unique_ptr<Expr> &&callee) {
  //    auto node = std::make_unique<FunctionCallExpr>(std::move(callee));
  //
  //    for (auto state = ParseState::eFunctionCallExpr; state > ParseState::eFinal;) {
  //      if (state = next_state(state, *(++tok)); state <= ParseState::eFinal) {
  //        return node;
  //      }
  //
  //      // TODO(rihwis-weard): don't hardcode type / values
  //      if (state == ParseState::eFunctionCallArgs) {
  //        node->add(std::make_unique<StringExpr>(tok->repr));
  //      }
  //    }
  //
  //    return node;
  //  }

  std::unique_ptr<ASTNode> Parser::parse_module_open_stmt(ParseState &state,
    std::vector<Token>::const_iterator &tok, std::vector<Token> const &tokens) {

    auto module_root = std::make_unique<ASTNode>();

    for (; state > ParseState::eFinal;) {
      if (++tok == tokens.end()) {
        return module_root;
      }

      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
        return module_root;
      }

      if (state == ParseState::eIdentifierExpr) {
        // TODO(rihwis-weard): correct way to cache repeated ASTNode/Module resolutions and returning
        //                     unique_ptr to ASTNode? May need to use shared_ptr/weak_ptrs all around
        //        if (auto mod = mod_map_.find(tok->repr); mod != mod_map_.end()) {
        //          return std::unique_ptr<ASTNode>{mod->second.get()};
        //        }

        // TODO(rihtwis-weard): return `OpenModuleStmt` if `Module` is already loaded

        // TODO(rihwis-weard): don't hard-code module lookup/resolution
        // distribute module registry, file -> module names
        auto toks = Lexer{stdlib_path_ + "io.clr"}.lex();

        // TODO(rihtwis-weard): module not found exception or save error and keep parsing
        return parse<ModuleDecl>(toks, tok->repr);
      }
    }

    return module_root;
  }

  std::unique_ptr<ExternDecl> Parser::parse_extern_decl(ParseState &state,
    std::vector<Token>::const_iterator tok, std::vector<Token> const &tokens) {

    std::string              name{};
    std::vector<FunctionArg> args{};
    std::string              return_type{};
    std::string              linkage_name{};

    for (; state > ParseState::eFinal;) {
      if (++tok == tokens.end()) {
        return std::make_unique<ExternDecl>(
          name, std::move(args), return_type, std::make_unique<StringExpr>(linkage_name));
      }

      if (state = next_state(state, *tok); state <= ParseState::eFinal) {
        return std::make_unique<ExternDecl>(
          name, std::move(args), return_type, std::make_unique<StringExpr>(linkage_name));
      }

      if (state == ParseState::eExternDeclName) {
        name = tok->repr;
      } else if (state == ParseState::eFunctionDeclArgs) {
        args = parse_function_decl_args(tok);
        --tok;
      } else if (state == ParseState::eFunctionDeclReturnType) {
        tok++;
        return_type = tok->repr;
        state       = ParseState::eExternDecl;
      } else if (state == ParseState::eExternDeclLinkageName) {
        linkage_name = tok->repr;
      }
    }

    // TODO(rihtwis-weard): need to record errors
    return nullptr;
  }

  std::vector<FunctionArg> Parser::parse_function_decl_args(
    std::vector<Token>::const_iterator &tok) {
    if (tok->kind != TokenKind::eLParens) {
      throw syntax_error{};
    }

    auto args = std::vector<FunctionArg>{};

    FunctionArg curr_arg{};
    tok = std::next(tok);

    // TODO(rw): distinguish right parens and operator
    for (; tok->kind != TokenKind::eOperator; ++tok) {
      tok           = std::next(tok);
      curr_arg.name = tok->repr;

      // TODO(rw): parse colon separator, type identifier, and comma separator

      // args.push_back(curr_arg);
    }
    return args;
  }

} // namespace claire::parser
