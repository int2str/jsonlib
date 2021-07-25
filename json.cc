#include "jsonlib/json.h"

#include "parser.h"
#include "tokenizer.h"

namespace json {

JSON::JSON(std::string_view json)
    : nodes_{Parser::parse(Tokenizer::parse(json))} {}

const Node& JSON::root() const { return nodes_.at(0); }

const Node& JSON::operator[](size_t idx) const { return root()[idx]; }

const Node& JSON::operator[](std::string_view property) const {
  return root()[property];
}

}  // namespace json
