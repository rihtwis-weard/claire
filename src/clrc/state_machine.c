#include "state_machine.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#define offset(e) (eCharType##e * eLexicalStateCount)

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
  // Special
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
  // Numbers
  [48 ... 57]  = offset(Digit),
  // Letters
  [65 ... 90]  = offset(Letter),
  ['_']        = offset(Letter),
  [97 ... 122] = offset(Letter),
};

// Reevaluates character after ending identifier, special sequence, or numeric literal
// by incrementing source (typically decrements by 1), akin to negative lookahead.
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
  // Digits
  [48 ... 57]  = -1,
  // Letters
  [65 ... 90]  = -1,
  ['_']        = -1,
  [97 ... 122] = -1,
};

#define state(s) (eLexicalState##s)

// Lexical analysis state transitions
uint8_t const lex_trans[offset(Count)] = {
  [state(NextChar) + offset(Layout)]    = state(NextChar),
  [state(NextChar) + offset(Space)]     = state(NextChar),
  [state(NextChar) + offset(LineFeed)]  = state(NewLine),
  [state(NextChar) + offset(Letter)]    = state(Identifier),
  [state(NextChar) + offset(Digit)]     = state(Numeral),
  [state(NextChar) + offset(Separator)] = state(Separator),
  [state(NextChar) + offset(Operator)]  = state(Operator),

  [state(NewLine) + offset(Layout)]    = state(NextChar),
  [state(NewLine) + offset(Space)]     = state(NextChar),
  [state(NewLine) + offset(LineFeed)]  = state(NewLine),
  [state(NewLine) + offset(Letter)]    = state(Identifier),
  [state(NewLine) + offset(Digit)]     = state(Numeral),
  [state(NewLine) + offset(Separator)] = state(Separator),
  [state(NewLine) + offset(Operator)]  = state(Operator),

  [state(Identifier) + offset(Layout)]    = state(IdentifierEnd),
  [state(Identifier) + offset(Space)]     = state(IdentifierEnd),
  [state(Identifier) + offset(LineFeed)]  = state(IdentifierEnd),
  [state(Identifier) + offset(Letter)]    = state(Identifier),
  [state(Identifier) + offset(Digit)]     = state(Identifier),
  [state(Identifier) + offset(Separator)] = state(IdentifierEnd),
  [state(Identifier) + offset(Operator)]  = state(IdentifierEnd),

  [state(IdentifierEnd) + offset(Layout)]    = state(NextChar),
  [state(IdentifierEnd) + offset(Space)]     = state(NextChar),
  [state(IdentifierEnd) + offset(LineFeed)]  = state(NewLine),
  [state(IdentifierEnd) + offset(Letter)]    = state(Identifier),
  [state(IdentifierEnd) + offset(Digit)]     = state(Numeral),
  [state(IdentifierEnd) + offset(Separator)] = state(Separator),
  [state(IdentifierEnd) + offset(Operator)]  = state(Operator),

  [state(Numeral) + offset(Layout)]    = state(NumeralEnd),
  [state(Numeral) + offset(Space)]     = state(NumeralEnd),
  [state(Numeral) + offset(LineFeed)]  = state(NumeralEnd),
  [state(Numeral) + offset(Letter)]    = state(NumeralEnd),
  [state(Numeral) + offset(Digit)]     = state(Numeral),
  [state(Numeral) + offset(Separator)] = state(NumeralEnd),
  [state(Numeral) + offset(Operator)]  = state(NumeralEnd),

  [state(NumeralEnd) + offset(Layout)]    = state(NextChar),
  [state(NumeralEnd) + offset(Space)]     = state(NextChar),
  [state(NumeralEnd) + offset(LineFeed)]  = state(NewLine),
  [state(NumeralEnd) + offset(Letter)]    = state(Identifier),
  [state(NumeralEnd) + offset(Digit)]     = state(Numeral),
  [state(NumeralEnd) + offset(Separator)] = state(Separator),
  [state(NumeralEnd) + offset(Operator)]  = state(Operator),

  [state(Separator) + offset(Layout)]    = state(NextChar),
  [state(Separator) + offset(Space)]     = state(NextChar),
  [state(Separator) + offset(LineFeed)]  = state(NewLine),
  [state(Separator) + offset(Letter)]    = state(Identifier),
  [state(Separator) + offset(Digit)]     = state(Numeral),
  [state(Separator) + offset(Separator)] = state(Separator),
  [state(Separator) + offset(Operator)]  = state(Operator),

  [state(Operator) + offset(Layout)]    = state(NextChar),
  [state(Operator) + offset(Space)]     = state(NextChar),
  [state(Operator) + offset(LineFeed)]  = state(NewLine),
  [state(Operator) + offset(Letter)]    = state(Identifier),
  [state(Operator) + offset(Digit)]     = state(Numeral),
  [state(Operator) + offset(Separator)] = state(Separator),
  [state(Operator) + offset(Operator)]  = state(Operator),
};

// True if inside of an identifier, value, or special multi-character operator
uint8_t const lex_inside[offset(Count)] = {
  [eLexicalStateIdentifier] = 1,
  [eLexicalStateNumeral]    = 1,
  [eLexicalStateSeparator]  = 1,
  [eLexicalStateOperator]   = 1,
};

#pragma GCC diagnostic pop
