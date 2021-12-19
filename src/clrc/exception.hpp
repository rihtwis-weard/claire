#pragma once

#include <exception>

namespace claire {

  class unexpected_eof : std::exception {
  public:
    virtual char const *what() const noexcept;
  };

  class invalid_lexeme : std::exception {

  public:
    virtual char const *what() const noexcept;
  };

} // namespace claire
