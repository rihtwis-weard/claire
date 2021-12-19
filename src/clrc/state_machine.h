#pragma once

#ifdef __cplusplus

#include <cstdint>

#include "utils.hpp"

namespace claire {

  // TODO(rihtwis-weard): rename to Glyph?
  enum class CharType {
    eLayout,
    eSpace,
    eCarriageReturn,
    eLineFeed,
    eLetter,
    eDigit,
    eSeparator,
    eOperator,
    eVerticalBar,
    eQuote,
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
    lex_trans[utype(LexicalState::eCount) * utype(CharType::eCount)];

  // True (1) if inside lexeme
  extern "C" std::uint8_t const lex_inside[utype(CharType::eCount)];

  // Apply state transition using current state and equivalence class
  auto next_state(LexicalState curr, std::uint16_t eqc) {
    return static_cast<LexicalState>(lex_trans[utype(curr) + eqc]);
  }

  auto should_exit(LexicalState curr) {
    // Since LexicalState::eFinal is 0, by default we break out when
    // state transitions are not explicitly handled
    return utype(curr) <= utype(LexicalState::eFinal);
  }

} // namespace claire

#else

#include <stdint.h>

typedef enum {
  eCharTypeLayout,
  eCharTypeSpace,
  eCharTypeCarriageReturn,
  eCharTypeLineFeed,
  eCharTypeLetter,
  eCharTypeDigit,
  eCharTypeSeparator,
  eCharTypeOperator,
  eCharTypeVerticalBar,
  eCharTypeQuote,
  eCharTypeEOF,
  eCharTypeCount
} CharType;

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
  eLexicalStateComment,
  eLexicalStateNewLine,
  eLexicalStateEOF,
  eLexicalStateError,
  eLexicalStateCount,
} LexicalState;

#endif
