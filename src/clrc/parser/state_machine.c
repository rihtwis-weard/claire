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
  ['-']        = offset(Hyphen),
  ['.']        = offset(Operator),
  [':']        = offset(Colon),
  [';']        = offset(Separator),
  ['<']        = offset(Operator),
  ['=']        = offset(Operator),
  ['>']        = offset(Operator),
  ['\\']       = offset(Separator),
  ['{']        = offset(Operator),
  ['|']        = offset(VerticalBar),
  ['}']        = offset(Operator),
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
  [';']        = -1,
  ['<']        = -1,
  ['=']        = -1,
  ['>']        = -1,
  ['\\']       = -1,
  ['{']        = -1,
  ['|']        = -1,
  ['}']        = -1,
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
  [state(s) + offset(Colon)]       = state(OperatorMulti),     \
  [state(s) + offset(Hyphen)]      = state(OperatorMulti),     \
  [state(s) + offset(VerticalBar)] = state(OperatorMulti),     \
  [state(s) + offset(DoubleQuote)] = state(String)
// clang-format on

#define reduce(prev_state, transition, next_state)                                       \
  [state(prev_state) + offset(transition)] = state(next_state)

// TODO(rihtwis-weard): need to start treating NewLine/LineFeeds as tokens for breaking up expressions?

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
  reduce(Identifier, Colon, IdentifierEnd),
  reduce(Identifier, Hyphen, IdentifierEnd),
  reduce(Identifier, VerticalBar, IdentifierEnd),
  reduce(Identifier, DoubleQuote, IdentifierEnd),

  reduce(String, Layout, String),
  reduce(String, Space, String),
  reduce(String, LineFeed, Error),
  reduce(String, Letter, String),
  reduce(String, Digit, String),
  reduce(String, Separator, String),
  reduce(String, Operator, String),
  reduce(String, Colon, String),
  reduce(String, Hyphen, String),
  reduce(String, VerticalBar, String),
  reduce(String, DoubleQuote, StringEnd),
  reduce(String, EOF, EOF),

  reduce(Numeral, Layout, NumeralEnd),
  reduce(Numeral, Space, NumeralEnd),
  reduce(Numeral, LineFeed, NumeralEnd),
  reduce(Numeral, Letter, NumeralEnd),
  reduce(Numeral, Digit, Numeral),
  reduce(Numeral, Separator, NumeralEnd),
  reduce(Numeral, Operator, NumeralEnd),
  reduce(Numeral, Colon, NumeralEnd),
  reduce(Numeral, Hyphen, NumeralEnd),
  reduce(Numeral, VerticalBar, NumeralEnd),
  reduce(Numeral, DoubleQuote, EOF),

  reduce(OperatorMulti, Layout, OperatorMultiEnd),
  reduce(OperatorMulti, Space, OperatorMultiEnd),
  reduce(OperatorMulti, LineFeed, OperatorMultiEnd),
  reduce(OperatorMulti, Letter, OperatorMultiEnd),
  reduce(OperatorMulti, Digit, OperatorMultiEnd),
  reduce(OperatorMulti, Separator, OperatorMultiEnd),
  reduce(OperatorMulti, Operator, OperatorMulti),
  reduce(OperatorMulti, Colon, OperatorMulti),
  reduce(OperatorMulti, Hyphen, Error),
  reduce(OperatorMulti, VerticalBar, Error),
  reduce(OperatorMulti, DoubleQuote, OperatorMultiEnd),
  reduce(OperatorMulti, EOF, EOF),
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

#pragma GCC diagnostic pop
