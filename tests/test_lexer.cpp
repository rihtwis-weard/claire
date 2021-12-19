#define APPROVALS_UT
#include "ApprovalTests.hpp"

#include "lexer.hpp"

int main() {
  using namespace boost::ut;
  using namespace ApprovalTests;

  auto directory = Approvals::useApprovalsSubdirectory("golden_files");
}
