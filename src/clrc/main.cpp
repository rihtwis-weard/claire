#include <iostream>

#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char const *argv[]) {
  constexpr auto source_fname = "../../examples/hello_world.clr";

  auto tokens = claire::Lexer{source_fname}.lex();
  for (auto const &tok : tokens) {
    std::cout << tok << "\n";
  }

  std::size_t i{};
  for (auto v : claire::parse_trans) {
    std::cout << "[" << i++ << "]: " << v << "\n";
  }

  auto ast        = claire::Parser{}.parse(tokens);
  auto ir_codegen = claire::CodeGenerator{source_fname};
  ir_codegen.codegen(ast.get());
  ir_codegen.fin();
  std::cout << ir_codegen.dumps() << "\n";

  ir_codegen.emit_object_code();
  return EXIT_SUCCESS;
}
