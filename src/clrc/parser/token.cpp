#include "token.h"

namespace claire::parser {
  robin_hood::unordered_map<std::string, TokenKind> const token_map = {
    // Operators
    {".", TokenKind::eAccess},
    {"(", TokenKind::eLParens},
    {")", TokenKind::eRParens},
    {"-", TokenKind::eMinus},
    {"{", TokenKind::eScopeBegin},
    {"}", TokenKind::eScopeEnd},
    // Special Multi Operators
    {"::", TokenKind::eAccessNamespace},
    {"->", TokenKind::eArrow},
    {"|>", TokenKind::ePipe},
    // Reserved Keywords
    {"func", TokenKind::eReservedFunc},
    //    {"let", TokenKind::eReservedLet},
    //    {"if", TokenKind::eReservedIf},
    //    {"else", TokenKind::eReservedElse},
    //    {"open", TokenKind::eReservedOpen},
    //    {"module", TokenKind::eReservedModule},
    //    {"export", TokenKind::eReservedExport},
    //    {"extern", TokenKind::eReservedExtern},
    // Builtin Types
    {"binary", TokenKind::eTypeBinary},
    {"u32", TokenKind::eTypeU32},
  };
}
