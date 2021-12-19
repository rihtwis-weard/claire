#include "token.hpp"

namespace claire {
  robin_hood::unordered_map<std::string, TokenKind> const token_map = {
    // Special Multi Operators
    {"|>", TokenKind::ePipe},
    // Reserved Keywords
    {"let", TokenKind::eReservedLet},
    {"if", TokenKind::eReservedIf},
    {"else", TokenKind::eReservedElse},
  };
}
