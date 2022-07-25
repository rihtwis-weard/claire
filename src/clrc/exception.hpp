#pragma once

#include <exception>
#include <string>

namespace claire {

  class unexpected_eof : public std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

  class invalid_lexeme : public std::exception {
  public:
    [[nodiscard]] char const *what() const noexcept override;
  };

  class syntax_error : public std::exception {
    std::string message_;

  public:
    explicit syntax_error();
    explicit syntax_error(std::string message);

    [[nodiscard]] char const *what() const noexcept override;
  };

} // namespace claire
