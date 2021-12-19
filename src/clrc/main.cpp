#include <iostream>

#include "lexer.hpp"

int main(int argc, char const *argv[]) {
  auto tokens = claire::Lexer{"../../examples/fib.clr"}.lex();
  for (auto const &tok : tokens) {
    std::cout << tok << "\n";
  }

  return EXIT_SUCCESS;
}
