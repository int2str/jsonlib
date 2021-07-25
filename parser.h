#ifndef PARSER_H
#define PARSER_H

#include <vector>

#include "jsonlib/node.h"
#include "token.h"
#include "validatable.h"

namespace json {

struct Parser {
  static std::vector<Node> parse(const std::vector<Token>& tokens);
};

}  // namespace json

#endif
