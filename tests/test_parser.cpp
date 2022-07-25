#include "fixtures.hpp"
#include "parser/ast_pretty_printer.hpp"
#include "parser/parser.hpp"
#include "parser/token.h"

int main() {
  using namespace claire::parser;

  "simple_identifier"_test = []() {
    auto pp = ASTPrettyPrinter{};

    // my_variable
    std::vector<Token> const tokens{
      {TokenKind::eIdentifier, "my_variable"},
    };

    auto it   = tokens.begin();
    auto node = parse_simple_identifier_expression(it);
    Approvals::verify(pp.pretty_print(node.get()));
  };

  "identifier_sequence.access_namespace"_test = []() {
    auto pp = ASTPrettyPrinter{};

    // my_namespace::my_func
    std::vector<Token> const tokens{
      {TokenKind::eIdentifier, "my_namespace"},
      {TokenKind::eAccessNamespace, "::"},
      {TokenKind::eIdentifier, "my_func"},
    };

    auto it   = tokens.begin();
    auto node = parse_identifier_sequence(it);
    Approvals::verify(pp.pretty_print(node.get()));
  };

  "expression_sequence.all_identifiers"_test = []() {
    auto pp = ASTPrettyPrinter{};

    // a, b, c
    std::vector<Token> const tokens{
      {TokenKind::eIdentifier, "a"},
      {TokenKind::eSeparator, ","},
      {TokenKind::eIdentifier, "b"},
      {TokenKind::eSeparator, ","},
      {TokenKind::eIdentifier, "c"},
    };

    auto ctx  = parse_context{tokens};
    auto node = parse_expression_sequence(ctx);
    Approvals::verify(pp.pretty_print(node.get()));
  };

  "function_call_expression.no_args"_test = []() {
    auto pp = ASTPrettyPrinter{};

    // my_func()
    std::vector<Token> const tokens{
      {TokenKind::eLParens, "("},
      {TokenKind::eRParens, ")"},
    };

    auto it     = tokens.begin();
    auto callee = std::make_unique<IdentifierExpr>("my_func");
    auto node   = parse_function_call_expression(it, std::move(callee));
    Approvals::verify(pp.pretty_print(node.get()));
  };

  //  "function_call_expression.two_or_more_args"_test = []() {
  //    auto pp = ASTPrettyPrinter{};
  //
  //    // my_product(a, b)
  //    std::vector<Token> const tokens{
  //      {TokenKind::eLParens, "("},
  //      {TokenKind::eIdentifier, "a"},
  //      {TokenKind::eSeparator, ","},
  //      {TokenKind::eIdentifier, "b"},
  //      {TokenKind::eRParens, ")"},
  //    };
  //
  //    auto it     = tokens.begin();
  //    auto callee = std::make_unique<IdentifierExpr>("my_product");
  //    auto node   = parse_function_call_expression(it, std::move(callee));
  //    Approvals::verify(pp.pretty_print(node.get()));
  //  };
}
