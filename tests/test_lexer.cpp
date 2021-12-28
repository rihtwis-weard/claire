#include "fixtures.hpp"
#include "parser/lexer.hpp"

int main() {
  "hello_world"_test = []() {
    auto lexemes = claire::parser::Lexer{"../../examples/hello_world.clr"}.lex();
    Approvals::verifyAll("hello_world.clr", lexemes);
  };

  "fib"_test = []() {
    auto lexemes = claire::parser::Lexer{"../../examples/fib.clr"}.lex();
    Approvals::verifyAll("fib.clr", lexemes);
  };

  "exception"_test = []() {
    std::string sources[]{
      "unsupported_multi_operator.clr",
    };

    for (auto const &src : sources) {
      auto lexer = claire::parser::Lexer{"../../tests/data/" + src};
      expect(throws([&]() { lexer.lex(); }));
    }
  };
}
