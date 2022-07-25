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
    eAccessMember,
    eLParens,
    eRParens,
    eMinus,
    eScopeBegin,
    eScopeEnd,

    // Multi
    eAccessNamespace,
    eArrow,
    ePipe,

    // Reserved Keywords
    eReservedFunc,
    //    eReservedLet,
    //    eReservedIf,
    //    eReservedElse,
    //    eReservedOpen,
    //    eReservedModule,
    //    eReservedExport,
    //    eReservedExtern,

    // Builtin Types
    eTypeBinary,
    eTypeU32,

    eCount,
  };

  extern robin_hood::unordered_map<std::string, TokenKind> const token_map;

  struct Token {
    TokenKind   kind;
    std::string repr;
    std::size_t len;

    // Debugging metadata
//    std::string name;
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
      TOKEN_DESC(TokenKind::eAccessMember, "Operator");
      TOKEN_DESC(TokenKind::eLParens, "Operator");
      TOKEN_DESC(TokenKind::eRParens, "Operator");
      TOKEN_DESC(TokenKind::eMinus, "Operator");
      TOKEN_DESC(TokenKind::eScopeBegin, "Separator");
      TOKEN_DESC(TokenKind::eScopeEnd, "Separator");
      TOKEN_DESC(TokenKind::eAccessNamespace, "Operator");
      TOKEN_DESC(TokenKind::eArrow, "Operator");
      TOKEN_DESC(TokenKind::ePipe, "Operator");

      TOKEN_DESC(TokenKind::eReservedFunc, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedLet, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedIf, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedElse, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedOpen, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedModule, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedExport, "Keyword");
      //      TOKEN_DESC(TokenKind::eReservedExtern, "Keyword");
      TOKEN_DESC(TokenKind::eTypeBinary, "Type");
      TOKEN_DESC(TokenKind::eTypeU32, "Type");
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
  eTokenKindAccessMember,
  eTokenKindLParens,
  eTokenKindRParens,
  eTokenKindMinus,
  eTokenKindScopeBegin,
  eTokenKindScopeEnd,
  eTokenKindAccessNamespace,
  eTokenKindArrow,
  eTokenKindPipe,
  eTokenKindReservedFunc,
  //  eTokenKindReservedLet,
  //  eTokenKindReservedIf,
  //  eTokenKindReservedElse,
  //  eTokenKindReservedOpen,
  //  eTokenKindReservedModule,
  //  eTokenKindReservedExport,
  //  eTokenKindReservedExtern,
  eTokenKindTypeBinary,
  eTokenKindTypeU32,
  eTokenKindCount,
} TokenKind;

#endif
