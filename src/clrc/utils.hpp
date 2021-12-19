#pragma once

#include <fstream>
#include <string>
#include <type_traits>

namespace claire {
  inline auto loads(std::string const &fpath) {
    return std::string{std::istreambuf_iterator<char>(std::ifstream{fpath}.rdbuf()),
      std::istreambuf_iterator<char>()};
  }

  template <typename T>
  constexpr auto utype(T t) {
    return static_cast<std::underlying_type_t<T>>(t);
  }
} // namespace claire
