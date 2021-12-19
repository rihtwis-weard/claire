#include "token.hpp"

namespace claire {
  robin_hood::unordered_map<std::string, TokenKind> const token_map = {
    {"let", TokenKind::eReservedLet},
    {"if", TokenKind::eReservedIf},
    {"else", TokenKind::eReservedElse},
  };
}
