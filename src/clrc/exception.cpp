#include "exception.hpp"

namespace claire {

  char const *unexpected_eof::what() const noexcept {
    return "Unexpected end-of-file";
  }

  char const *invalid_lexeme::what() const noexcept {
    return "Invalid lexeme";
  }

} // namespace claire
