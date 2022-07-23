#include "exception.hpp"

namespace claire {

  char const *unexpected_eof::what() const noexcept {
    return "Unexpected end-of-file";
  }

  char const *invalid_lexeme::what() const noexcept {
    return "Invalid lexeme";
  }

  syntax_error::syntax_error()
    : message_{"Syntax error"} {
  }

  syntax_error::syntax_error(std::string message)
    : message_{"Syntax error: " + std::move(message)} {
  }

  char const *syntax_error::what() const noexcept {
    return message_.c_str();
  }

} // namespace claire
