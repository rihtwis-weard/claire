#include <iostream>

#include "codegen/ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

int main(int argc, char const *argv[]) {
  constexpr auto source_fname = "../../examples/hello_world.clr";

  auto tokens = claire::parser::Lexer{source_fname}.lex();
  for (auto const &tok : tokens) {
    std::cout << tok << "\n";
  }

  std::size_t i{};
  for (auto v : claire::parser::parse_trans) {
    std::cout << "[" << i++ << "]: " << static_cast<int>(v) << "\n";
  }

  constexpr auto stdlib_path = "../../src/stdlib/";

  auto ast = claire::parser::Parser{stdlib_path}.parse(tokens);

  std::unique_ptr<claire::codegen::IRCodeGenerator> code_generator =
    std::make_unique<claire::codegen::IRCodeGenerator>(source_fname);

  std::visit(*code_generator, ast->as_variant());

  std::cout << code_generator->dumps() << "\n";
  code_generator->emit_object_code();
  return EXIT_SUCCESS;
}
