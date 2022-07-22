#pragma once

#include <exception>

namespace claire {

  class unexpected_eof : public std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

  class invalid_lexeme : public std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

} // namespace claire
