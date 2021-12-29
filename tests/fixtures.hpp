#define APPROVALS_UT
#include "ApprovalTests.hpp"

using namespace boost::ut;
using namespace ApprovalTests;

auto           directory   = Approvals::useApprovalsSubdirectory("golden_files");
constexpr auto stdlib_path = "../../src/stdlib/";
