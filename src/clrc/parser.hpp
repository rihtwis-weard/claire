#pragma once

#include "ast.hpp"
#include "token.hpp"

namespace claire {
  class Parser {
  public:
    std::unique_ptr<ASTNode> parse(std::vector<Token> const &tokens);

  private:
    std::unique_ptr<ProgramDecl>      parse_program_decl(std::vector<Token> const &tokens);
    std::unique_ptr<FunctionDecl> parse_function_decl();
    std::unique_ptr<VariableDecl> parse_variable_decl();

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_call_expr(std::unique_ptr<Expr> &&fn);

    std::unique_ptr<IfStmt>     parse_if_stmt();
    std::unique_ptr<ReturnStmt> parse_return_stmt();
  };
} // namespace claire
