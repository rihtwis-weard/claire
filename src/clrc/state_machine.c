#include "state_machine.h"
#include "token.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#define offset(e) (eGlyph##e * eLexicalStateCount)

// Character equivalence classes are used to index into a
// lexical transition state machine. Offsets are premultiplied to
// elide an additional indirection.
uint16_t const ch_to_eqc[256] = {
  // Layout
  [0]          = offset(EOF),
  [1 ... 9]    = offset(Layout),
  ['\n']       = offset(LineFeed),
  [11 ... 31]  = offset(Layout),
  [' ']        = offset(Space),
  // String Literals
  ['"']        = offset(DoubleQuote),
  // Special
  ['!']        = offset(Operator),
  ['(']        = offset(Operator),
  [')']        = offset(Operator),
  ['+']        = offset(Operator),
  ['-']        = offset(Operator),
  ['.']        = offset(Operator),
  [':']        = offset(Separator),
  ['<']        = offset(Operator),
  ['=']        = offset(Operator),
  ['>']        = offset(Operator),
  ['\\']       = offset(Separator),
  ['|']        = offset(VerticalBar),
  // Numbers
  [48 ... 57]  = offset(Digit),
  // Letters
  [65 ... 90]  = offset(Letter),
  ['_']        = offset(Letter),
  [97 ... 122] = offset(Letter),
};

// Reevaluates character after ending identifier, special sequence, or numeric literal
// by incrementing source (typically decrements by 1), usually following negative lookahead to terminate token.
int const ch_reeval[256] = {
  // Line Feed - New Line
  [10]         = -1,
  // Special Characters
  ['(']        = -1,
  [')']        = -1,
  ['+']        = -1,
  ['-']        = -1,
  ['.']        = -1,
  [':']        = -1,
  ['<']        = -1,
  ['=']        = -1,
  ['>']        = -1,
  ['\\']       = -1,
  ['|']        = -1,
  // Digits
  [48 ... 57]  = -1,
  // Letters
  [65 ... 90]  = -1,
  ['_']        = -1,
  [97 ... 122] = -1,
};

#define state(s) (eLexicalState##s)

// state transitions that always serve to initiate a new lexeme after terminating an existing one
// clang-format off
#define initiating_states(s)                                   \
  [state(s) + offset(Layout)]      = state(NextChar),          \
  [state(s) + offset(Space)]       = state(NextChar),          \
  [state(s) + offset(LineFeed)]    = state(NewLine),           \
  [state(s) + offset(Letter)]      = state(Identifier),        \
  [state(s) + offset(Digit)]       = state(Numeral),           \
  [state(s) + offset(Separator)]   = state(Separator),         \
  [state(s) + offset(Operator)]    = state(OperatorSingle),    \
  [state(s) + offset(VerticalBar)] = state(OperatorMulti),     \
  [state(s) + offset(DoubleQuote)] = state(String)
// clang-format on

#define reduce(prev_state, transition, next_state)                                       \
  [state(prev_state) + offset(transition)] = state(next_state)

// Lexical analysis state transitions
uint8_t const lex_trans[offset(Count)] = {
  initiating_states(NextChar),
  initiating_states(NewLine),
  initiating_states(IdentifierEnd),
  initiating_states(StringEnd),
  initiating_states(NumeralEnd),
  initiating_states(Separator),
  initiating_states(OperatorSingle),
  initiating_states(OperatorMultiEnd),

  reduce(Identifier, Layout, IdentifierEnd),
  reduce(Identifier, Space, IdentifierEnd),
  reduce(Identifier, LineFeed, IdentifierEnd),
  reduce(Identifier, Letter, Identifier),
  reduce(Identifier, Digit, Identifier),
  reduce(Identifier, Separator, IdentifierEnd),
  reduce(Identifier, Operator, IdentifierEnd),
  reduce(Identifier, VerticalBar, IdentifierEnd),
  reduce(Identifier, DoubleQuote, IdentifierEnd),

  reduce(String, Layout, String),
  reduce(String, Space, String),
  reduce(String, LineFeed, Error),
  reduce(String, Letter, String),
  reduce(String, Digit, String),
  reduce(String, Separator, String),
  reduce(String, Operator, String),
  reduce(String, VerticalBar, String),
  reduce(String, DoubleQuote, StringEnd),
  reduce(String, EOF, EOF),

  [state(Numeral) + offset(Layout)]      = state(NumeralEnd),
  [state(Numeral) + offset(Space)]       = state(NumeralEnd),
  [state(Numeral) + offset(LineFeed)]    = state(NumeralEnd),
  [state(Numeral) + offset(Letter)]      = state(NumeralEnd),
  [state(Numeral) + offset(Digit)]       = state(Numeral),
  [state(Numeral) + offset(Separator)]   = state(NumeralEnd),
  [state(Numeral) + offset(Operator)]    = state(NumeralEnd),
  [state(Numeral) + offset(VerticalBar)] = state(NumeralEnd),
  [state(Numeral) + offset(DoubleQuote)] = state(NumeralEnd),
  [state(Numeral) + offset(EOF)]         = state(EOF),

  [state(OperatorMulti) + offset(Layout)]      = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(Space)]       = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(LineFeed)]    = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(Letter)]      = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(Digit)]       = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(Separator)]   = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(Operator)]    = state(OperatorMulti),
  [state(OperatorMulti) + offset(VerticalBar)] = state(Error),
  [state(OperatorMulti) + offset(DoubleQuote)] = state(OperatorMultiEnd),
  [state(OperatorMulti) + offset(EOF)]         = state(EOF),
};

// True if inside of an identifier, value, or special multi-character operator
uint8_t const lex_inside[offset(Count)] = {
  [eLexicalStateSeparator]      = 1,
  [eLexicalStateOperatorSingle] = 1,
  [eLexicalStateOperatorMulti]  = 1,
  [eLexicalStateIdentifier]     = 1,
  [eLexicalStateNumeral]        = 1,
  [eLexicalStateString]         = 1,
};

#undef state
#define state(s) (eParseState##s)

#undef offset
#define offset(e) (eTokenKind##e * eParseStateCount)

#undef initiating_states

uint8_t const parse_trans[offset(Count)] = {
  reduce(NewScope, Identifier, Expr),
};

#pragma GCC diagnostic pop
