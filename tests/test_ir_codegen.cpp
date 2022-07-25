#include "codegen/ir_code_generator.hpp"
#include "fixtures.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

int main() {
  "hello_world"_test = []() {
    constexpr auto source_fname = "../../examples/hello_world.clr";
    auto           lexemes      = claire::parser::Lexer{source_fname}.tokenize();
    auto           ast          = claire::parser::Parser{stdlib_path}.parse(lexemes);

    std::unique_ptr<claire::codegen::IRCodeGenerator> code_generator =
      std::make_unique<claire::codegen::IRCodeGenerator>(source_fname);

    std::visit(*code_generator, ast->as_variant());

    code_generator->emit_object_code();

    Approvals::verify(
      code_generator->dumps(), Options().fileOptions().withFileExtension(".ll"));
  };
}
