#include "token.h"

namespace claire::parser {
  robin_hood::unordered_map<std::string, TokenKind> const token_map = {
    // Operators
    {".", TokenKind::eAccess},
    {"(", TokenKind::eLParens},
    // Special Multi Operators
    {"|>", TokenKind::ePipe},
    // Reserved Keywords
    {"let", TokenKind::eReservedLet},
    {"if", TokenKind::eReservedIf},
    {"else", TokenKind::eReservedElse},
  };
}
