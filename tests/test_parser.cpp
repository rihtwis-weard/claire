#include "fixtures.hpp"
#include "parser/ast_pretty_printer.hpp"
#include "parser/parser.hpp"
#include "parser/token.h"

int main() {

  "simple_identifier"_test = []() {
    auto pp = claire::parser::ASTPrettyPrinter{};

    std::vector<claire::parser::Token> const tokens{
      {
        .kind = claire::parser::TokenKind::eIdentifier,
        .repr = "my_variable",
        .len  = 11,
      },
    };

    auto it   = tokens.begin();
    auto node = claire::parser::parse_simple_identifier_expression(it);
    Approvals::verify(pp.pretty_print(node.get()));
  };

  "identifier_sequence.access_namespace"_test = []() {
    auto pp = claire::parser::ASTPrettyPrinter{};

    std::vector<claire::parser::Token> const tokens{
      {
        .kind = claire::parser::TokenKind::eIdentifier,
        .repr = "my_namespace",
        .len  = 12,
      },
      {
        .kind = claire::parser::TokenKind::eAccessNamespace,
        .repr = "::",
        .len  = 2,
      },

      {
        .kind = claire::parser::TokenKind::eIdentifier,
        .repr = "my_func",
        .len  = 7,
      },
    };

    auto it   = tokens.begin();
    auto node = claire::parser::parse_identifier_sequence(it);
    Approvals::verify(pp.pretty_print(node.get()));
  };
}
