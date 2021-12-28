#define APPROVALS_UT
#include "ApprovalTests.hpp"

#include "codegen/ir_code_generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main() {
  using namespace boost::ut;
  using namespace ApprovalTests;

  auto directory = Approvals::useApprovalsSubdirectory("golden_files");

  "hello_world"_test = []() {
    constexpr auto source_fname = "../../examples/hello_world.clr";
    auto           lexemes      = claire::Lexer{source_fname}.lex();
    auto           ast          = claire::Parser{}.parse(lexemes);

    std::unique_ptr<claire::codegen::IRCodeGenerator> code_generator =
      std::make_unique<claire::codegen::IRCodeGenerator>(source_fname);
    ast->traverse(*code_generator);

    code_generator->finish_program();
    std::cout << code_generator->dumps() << "\n";
    code_generator->emit_object_code();

    Approvals::verify(
      code_generator->dumps(), Options().fileOptions().withFileExtension(".ll"));
  };
}
