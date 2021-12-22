#define APPROVALS_UT
#include "ApprovalTests.hpp"

#include "lexer.hpp"
#include "parser.hpp"

int main() {
  using namespace boost::ut;
  using namespace ApprovalTests;

  auto directory = Approvals::useApprovalsSubdirectory("golden_files");

  "hello_world"_test = []() {
    auto lexemes = claire::Lexer{"../../examples/hello_world.clr"}.lex();
    auto ast     = claire::Parser{}.parse(lexemes);
    Approvals::verify(ast->pretty_print());
  };
}
