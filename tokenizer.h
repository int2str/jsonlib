#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string_view>
#include <vector>

#include "token.h"
#include "validatable.h"

namespace json {

struct Tokenizer {
  static std::vector<Token> parse(std::string_view json);
};

}  // namespace json

#endif
