#pragma once

#ifdef __cplusplus

#include <cstdint>

#include "../utils.hpp"

namespace claire::parser {

  enum class Glyph {
    eLayout,
    eSpace,
    eCarriageReturn,
    eLineFeed,
    eLetter,
    eDigit,
    eSeparator,
    eOperator,
    eColon,
    eHyphen,
    eVerticalBar,
    eDoubleQuote,
    eEOF,
    eCount
  };

  enum class LexicalState {
    eFinal,
    eNextChar,
    eSeparator,
    eOperatorSingle,
    eOperatorMulti,
    eOperatorMultiEnd,
    eIdentifier,
    eIdentifierEnd,
    eNumeral,
    eNumeralEnd,
    eString,
    eStringEnd,
    eComment,
    eNewLine,
    eEOF,
    eError,
    eCount,
  };

  // Character equivalence classes
  extern "C" std::uint16_t const ch_to_eqc[256];
  // Character re-evaluations
  extern "C" int const           ch_reeval[256];

  // Lexical state transitions
  extern "C" std::uint8_t const
    lex_trans[utype(LexicalState::eCount) * utype(Glyph::eCount)];

  // True (1) if inside lexeme
  extern "C" std::uint8_t const lex_inside[utype(Glyph::eCount)];

  // Apply state transition using current state and equivalence class
  inline auto next_state(LexicalState curr, std::uint16_t eqc) {
    return static_cast<LexicalState>(lex_trans[utype(curr) + eqc]);
  }

  inline auto should_exit(LexicalState curr) {
    // Since LexicalState::eFinal is 0, by default we break out when
    // state transitions are not explicitly handled
    return utype(curr) <= utype(LexicalState::eFinal);
  }

} // namespace claire::parser

#else

#include <stdint.h>

typedef enum {
  eGlyphLayout,
  eGlyphSpace,
  eGlyphCarriageReturn,
  eGlyphLineFeed,
  eGlyphLetter,
  eGlyphDigit,
  eGlyphSeparator,
  eGlyphOperator,
  eGlyphColon,
  eGlyphHyphen,
  eGlyphVerticalBar,
  eGlyphDoubleQuote,
  eGlyphEOF,
  eGlyphCount
} Glyph;

typedef enum {
  eLexicalStateFinal,
  eLexicalStateNextChar,
  eLexicalStateSeparator,
  eLexicalStateOperatorSingle,
  eLexicalStateOperatorMulti,
  eLexicalStateOperatorMultiEnd,
  eLexicalStateIdentifier,
  eLexicalStateIdentifierEnd,
  eLexicalStateNumeral,
  eLexicalStateNumeralEnd,
  eLexicalStateString,
  eLexicalStateStringEnd,
  eLexicalStateComment,
  eLexicalStateNewLine,
  eLexicalStateEOF,
  eLexicalStateError,
  eLexicalStateCount,
} LexicalState;

#endif
