#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

#include "codegen/ir_code_generator.hpp"

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

  auto ast = claire::Parser{}.parse(tokens);

  std::unique_ptr<claire::codegen::IRCodeGenerator> code_generator =
    std::make_unique<claire::codegen::IRCodeGenerator>(source_fname);
  ast->traverse(*code_generator);

  code_generator->finish_program();
  std::cout << code_generator->dumps() << "\n";
  code_generator->emit_object_code();
  return EXIT_SUCCESS;
}
