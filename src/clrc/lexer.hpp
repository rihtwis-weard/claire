#pragma once

#include <vector>

#include "exception.hpp"
#include "state_machine.h"
#include "token.h"

namespace claire {

  class Lexer {
    LexicalState       state_;
    std::string        source_;
    std::vector<Token> tokens_;

  public:
    explicit Lexer(std::string const &fpath);

    std::vector<Token> lex();
  };

} // namespace claire
