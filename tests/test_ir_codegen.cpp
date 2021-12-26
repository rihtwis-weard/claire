#define APPROVALS_UT
#include "ApprovalTests.hpp"

#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main() {
  using namespace boost::ut;
  using namespace ApprovalTests;

  auto directory = Approvals::useApprovalsSubdirectory("golden_files");

  "hello_world"_test = []() {
    constexpr auto source_fname   = "../../examples/hello_world.clr";
    auto           lexemes        = claire::Lexer{source_fname}.lex();
    auto           ast            = claire::Parser{}.parse(lexemes);
    auto           code_generator = claire::CodeGenerator{source_fname};
    code_generator.codegen(ast.get());
    code_generator.fin();

    Approvals::verify(
      code_generator.dumps(), Options().fileOptions().withFileExtension(".ll"));
  };
}
