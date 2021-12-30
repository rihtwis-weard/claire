#include "fixtures.hpp"
#include "parser/ast_pretty_printer.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

int main() {
  "hello_world"_test = []() {
    auto lexemes = claire::parser::Lexer{"../../examples/hello_world.clr"}.lex();
    auto ast     = claire::parser::Parser{stdlib_path}.parse(lexemes);

    auto ast_pp = claire::parser::ASTPrettyPrinter{};
    Approvals::verify(ast_pp.pretty_print(ast.get()));
  };
}
