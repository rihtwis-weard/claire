#pragma once

#include <vector>

#include "state_machine.h"
#include "token.hpp"

namespace claire {
  class Lexer {
    LexicalState       state_;
    std::string        source_;
    std::vector<Token> tokens_;

  public:
    explicit Lexer(std::string const &fpath)
      : state_{LexicalState::eNextChar}
      , source_{loads(fpath)} {
    }

    auto lex() {
      Token tok{};

      // Alias pointer to buffer to create register-friendly variable
      char *src_ptr = source_.data();

      do {
        int  ch;
        auto eqc = ch_to_eqc[(ch = *src_ptr++)];

        state_ = next_state(state_, eqc);
        tok.len += lex_inside[utype(state_)];

        switch (state_) {
        case LexicalState::eNextChar: {
          ++tok.col_num;
          break;
        }
        case LexicalState::eNewLine: {
          ++tok.line_num;
          tok.col_num = 1;
          break;
        }
        case LexicalState::eIdentifier: {
          tok.kind = TokenKind::eIdentifier;
          break;
        }
        case LexicalState::eOperatorMultiEnd:
        case LexicalState::eIdentifierEnd: {
          tok.update_repr(src_ptr, 1, 0);
          tok.to_hyponym();

          // End of token requires reevaluation
          src_ptr += ch_reeval[ch];
          // TODO(rihtwis-weard): update col pos according to token length if
          //                    reevaluation occurred

          tok.reset();
          tokens_.push_back(tok);
          break;
        }
        case LexicalState::eNumeral: {
          tok.kind = TokenKind::eNumeral;
          break;
        }
        case LexicalState::eNumeralEnd: {
          tok.update_repr(src_ptr, 1, 0);
          src_ptr += ch_reeval[ch];
          tok.reset();
          tokens_.push_back(tok);
          break;
        }
        case LexicalState::eSeparator: {
          tok.kind = TokenKind::eSeparator;
          tok.update_repr(src_ptr, 0, 0);

          tok.reset();
          tokens_.push_back(tok);
          break;
        }
        case LexicalState::eOperatorSingle: {
          tok.kind = TokenKind::eOperator;
          tok.update_repr(src_ptr, 0, 0);

          tok.reset();
          tokens_.push_back(tok);
          break;
        }
        default:
          break;
        }

      } while (not should_exit(state_));

      return tokens_;
    }
  };

} // namespace claire
