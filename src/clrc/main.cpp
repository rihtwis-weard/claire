#include <iostream>

#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char const *argv[]) {
  auto source_fname = "../../examples/hello_world.clr";

  auto tokens = claire::Lexer{source_fname}.lex();
  for (auto const &tok : tokens) {
    std::cout << tok << "\n";
  }

  std::size_t i{};
  for (auto v : claire::parse_trans) {
    std::cout << "[" << i++ << "]: " << v << "\n";
  }

  auto ast = claire::Parser{}.parse(tokens);
  claire::CodeGenerator{source_fname}.codegen(ast.get());
  return EXIT_SUCCESS;
}
