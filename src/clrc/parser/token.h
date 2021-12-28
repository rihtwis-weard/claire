#pragma once

#ifdef __cplusplus

#include <iomanip>

#include <robin_hood.h>

#include "../utils.hpp"

namespace claire::parser {

  enum class TokenKind {
    // General Tokens
    eIdentifier,
    eStringLiteral,
    eNumeral,
    eSeparator,
    eOperator,

    // Special Operators
    eAccess,
    eLParens,
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

    auto to_hyponym(TokenKind fallback) {
      if (auto it = token_map.find(repr); it != token_map.end()) {
        auto [_, token_kind] = *it;
        kind                 = token_kind;
      } else {
        kind = fallback;
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

#define TOKEN_DESC(kind, desc)                                                           \
  case (kind):                                                                           \
    os << (desc);                                                                        \
    break

  inline std::ostream &operator<<(std::ostream &os, Token const &tok) {
    os << std::setw(12);

    switch (tok.kind) {
      TOKEN_DESC(TokenKind::eIdentifier, "Identifier");
      TOKEN_DESC(TokenKind::eStringLiteral, "String");
      TOKEN_DESC(TokenKind::eNumeral, "Numeral");
      TOKEN_DESC(TokenKind::eSeparator, "Separator");
      TOKEN_DESC(TokenKind::eOperator, "Operator");
      TOKEN_DESC(TokenKind::eAccess, "Operator");
      TOKEN_DESC(TokenKind::eLParens, "Operator");
      TOKEN_DESC(TokenKind::ePipe, "Operator");
      TOKEN_DESC(TokenKind::eReservedLet, "Keyword");
      TOKEN_DESC(TokenKind::eReservedIf, "Keyword");
      TOKEN_DESC(TokenKind::eReservedElse, "Keyword");
    default:
      os << "Unknown";
      break;
    }

    return os << " | " << tok.repr << " |";
  }

} // namespace claire::parser

#else

typedef enum {
  eTokenKindIdentifier,
  eTokenKindStringLiteral,
  eTokenKindNumeral,
  eTokenKindSeparator,
  eTokenKindOperator,
  eTokenKindAccess,
  eTokenKindLParens,
  eTokenKindPipe,
  eTokenKindReservedLet,
  eTokenKindReservedIf,
  eTokenKindReservedElse,
  eTokenKindCount,
} TokenKind;

#endif
