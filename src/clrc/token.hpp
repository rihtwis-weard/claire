#pragma once

#include <iomanip>

#include <robin_hood.h>

#include "utils.hpp"

namespace claire {
  enum class TokenKind {
    // General Tokens
    eIdentifier,
    eNumeral,
    eSeparator,
    eOperator,

    // Special Operators
    ePipe,

    // Reserved Keywords
    eReservedLet,
    eReservedIf,
    eReservedElse,
    eCount,
  };

  extern robin_hood::unordered_map<std::string, TokenKind> const token_map;

  struct Token {
    TokenKind   kind;
    std::string repr;
    std::size_t len;

    // Debugging metadata
    std::string name;
    std::size_t line_num = 1;
    std::size_t col_num  = 1;

  public:
    auto to_hyponym() {
      if (auto it = token_map.find(repr); it != token_map.end()) {
        auto [_, token_kind] = *it;
        kind                 = token_kind;
      }
    }

    auto update_repr(char *src_ptr, int start_offset, int end_offset) {
      auto start = src_ptr - (len + start_offset);
      auto end   = start + (len + end_offset);
      repr       = std::string{start, end};
    }

    auto reset(std::size_t offset = 0) {
      // Reset trackers for next token
      col_num += (len + offset);
      len = 0;
    }

    friend std::ostream &operator<<(std::ostream &os, Token const &tok);
  };

  inline std::ostream &operator<<(std::ostream &os, Token const &tok) {
    static constexpr std::string_view names[]{
      "Identifier",
      "Numeral",
      "Separator",
      "Operator",
      "Operator",
      "Reserved",
      "Reserved",
      "Reserved",
    };
    return os << std::setw(12) << names[utype(tok.kind)] << " | " << tok.repr << " |";
  }

} // namespace claire
