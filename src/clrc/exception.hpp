#pragma once

#include <exception>

namespace claire {

  class unexpected_eof : std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

  class invalid_lexeme : std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

} // namespace claire
