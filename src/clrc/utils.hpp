#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>

namespace claire {
  inline auto loads(std::string const &fpath) {
    return std::string{std::istreambuf_iterator<char>(std::ifstream{fpath}.rdbuf()),
      std::istreambuf_iterator<char>()};
  }

  template <typename InputIt>
  auto join(InputIt first, InputIt last, std::string const &separator = "") {
    std::ostringstream res{};
    if (first != last) {
      res << *first;
      while (++first != last) {
        res << separator << *first;
      }
    }
    return res.str();
  }

  template <typename T>
  constexpr auto utype(T t) {
    return static_cast<std::underlying_type_t<T>>(t);
  }
} // namespace claire
