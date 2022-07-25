#include "lexer.hpp"

namespace claire::parser {

  Lexer::Lexer(std::string const &fpath)
    : state_{LexicalState::eNextChar}
    , source_{loads(fpath)} {
  }

  std::vector<Token> Lexer::tokenize() {
    Lexeme lex{};

    // Alias pointer to buffer to create register-friendly variable
    char *src_ptr = source_.data();

    do {
      unsigned char ch;
      auto          eqc = ch_to_eqc[(ch = *src_ptr++)];

      state_ = next_state(state_, eqc);
      lex.len += lex_inside[utype(state_)];

      switch (state_) {
      case LexicalState::eNextChar: {
        ++lex.col_num;
        break;
      }
      case LexicalState::eNewLine: {
        ++lex.line_num;
        lex.col_num = 1;
        break;
      }
      case LexicalState::eIdentifier: {
        lex.kind = TokenKind::eIdentifier;
        break;
      }
      case LexicalState::eOperatorMultiEnd:
      case LexicalState::eIdentifierEnd: {
        lex.update_repr(src_ptr, 1, 0);
        lex.to_hyponym();

        // End of token requires reevaluation
        src_ptr += ch_reeval[ch];
        // TODO(rihtwis-weard): update col pos according to token length if
        //                    reevaluation occurred

        lex.reset();
        tokens_.push_back(lex.as_token());
        break;
      }
      case LexicalState::eString: {
        lex.kind = TokenKind::eStringLiteral;
        break;
      }
      case LexicalState::eStringEnd: {
        lex.update_repr(src_ptr, 1, 1);
        src_ptr += ch_reeval[ch];
        lex.reset();
        tokens_.push_back(lex.as_token());
        break;
      }
      case LexicalState::eNumeral: {
        lex.kind = TokenKind::eNumeral;
        break;
      }
      case LexicalState::eNumeralEnd: {
        lex.update_repr(src_ptr, 1, 0);
        src_ptr += ch_reeval[ch];
        lex.reset();
        tokens_.push_back(lex.as_token());
        break;
      }
      case LexicalState::eSeparator: {
        lex.kind = TokenKind::eSeparator;
        lex.update_repr(src_ptr, 0, 0);

        lex.reset();
        tokens_.push_back(lex.as_token());
        break;
      }
      case LexicalState::eOperatorSingle: {
        lex.update_repr(src_ptr, 0, 0);
        lex.to_hyponym(TokenKind::eOperator);

        lex.reset();
        tokens_.push_back(lex.as_token());
        break;
      }
      case LexicalState::eEOF: {
        throw unexpected_eof{};
      }
      case LexicalState::eError: {
        throw invalid_lexeme{};
      }
      default:
        break;
      }

    } while (not should_exit(state_));

    return tokens_;
  }

} // namespace claire::parser
