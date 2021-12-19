#define APPROVALS_UT
#include "ApprovalTests.hpp"

#include "lexer.hpp"

int main() {
  using namespace boost::ut;
  using namespace ApprovalTests;

  auto directory = Approvals::useApprovalsSubdirectory("golden_files");

  "hello_world"_test = []() {
    auto lexemes = claire::Lexer{"../../examples/hello_world.clr"}.lex();
    Approvals::verifyAll("hello_world.clr", lexemes);
  };

  "fib"_test = []() {
    auto lexemes = claire::Lexer{"../../examples/fib.clr"}.lex();
    Approvals::verifyAll("fib.clr", lexemes);
  };
}
