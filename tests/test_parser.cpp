#include "fixtures.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

int main() {
  "hello_world"_test = []() {
    auto lexemes = claire::parser::Lexer{"../../examples/hello_world.clr"}.lex();
    auto ast     = claire::parser::Parser{stdlib_path}.parse(lexemes);
    Approvals::verify(ast->pretty_print());
  };
}
