#include "parser.h"

#include <algorithm>
#include <functional>
#include <string_view>
#include <vector>

#include "consumable.h"

namespace {

template <typename T>
constexpr bool is_in(const T& needle, const std::vector<T>& haystack) {
  return std::any_of(haystack.cbegin(), haystack.cend(),
                     [&](const auto& item) { return item == needle; });
}

}  // namespace

namespace json {

using TOKENS = Consumable<const std::vector<Token>&, Token>;
using NODES = std::vector<Node>;

bool consumeIfType(TOKENS& tokens, Token::Type type) {
  if (tokens.current().type() == type) {
    tokens.skip(1);
    return true;
  }
  return false;
}

bool hasObjectId(const TOKENS& tokens) {
  if (tokens.current().type() != Token::STRING) return false;
  if (!tokens.canPeek() || tokens.peek().type() != Token::COLON) return false;
  return true;
}

void parseValues(TOKENS& tokens, NODES& nodes, std::string_view id) {
  if (consumeIfType(tokens, Token::NULLOPT)) {
    nodes.push_back(Node{id, Node::Type::NULLOPT});

  } else if (tokens.current().type() == Token::BOOLEAN) {
    nodes.push_back(Node{id, tokens.consume().asBool()});

  } else if (tokens.current().type() == Token::NUMBER) {
    nodes.push_back(Node{id, tokens.consume().asNumber()});

  } else if (tokens.current().type() == Token::STRING) {
    nodes.push_back(Node{id, tokens.consume().asString()});

  } else {
    throw ValidationError{
        tokens.idx(), "Value must be a number, a string, null, true or false"};
  }
}

// Forward declared for recursion ...
void parseObject(TOKENS& tokens, NODES& nodes, std::string_view id);
void parseArray(TOKENS& tokens, NODES& nodes, std::string_view id);

void parseObjectProperty(bool id_required, TOKENS& tokens, NODES& nodes) {
  if (is_in(tokens.current().type(), {Token::ARRAY_END, Token::OBJECT_END}))
    return;

  if (!nodes.empty()) {
    if (!consumeIfType(tokens, Token::COMMA))
      throw ValidationError{tokens.idx(),
                            "Expected COMMA after object property"};
  }

  if (id_required && !hasObjectId(tokens))
    throw ValidationError{tokens.idx(), "Expected STRING for property id"};

  std::string id;
  if (hasObjectId(tokens)) {
    id = tokens.consume().asString();

    if (!consumeIfType(tokens, Token::COLON))
      throw ValidationError{tokens.idx(), "Expected COLON after property id"};
    if (tokens.eol())
      throw ValidationError{tokens.idx(), "Unexpected EOF after property id"};
  }

  switch (tokens.current().type()) {
    case Token::NUMBER:
    case Token::STRING:
    case Token::BOOLEAN:
    case Token::NULLOPT:
      parseValues(tokens, nodes, id);
      break;

    case Token::OBJECT_BEGIN:
      parseObject(tokens, nodes, id);
      break;

    case Token::ARRAY_BEGIN:
      parseArray(tokens, nodes, id);
      break;

    default:
      throw ValidationError{tokens.idx(), "Unexpected object property type"};
      break;
  }
}

void parseObject(TOKENS& tokens, NODES& nodes, std::string_view id) {
  if (!consumeIfType(tokens, Token::OBJECT_BEGIN))
    throw ValidationError{tokens.idx(), "Not an OBJECT_BEGIN"};

  if (tokens.eol())
    throw ValidationError{tokens.idx(), "Unexpected EOF after OBJECT_BEGIN"};

  if (!is_in(tokens.current().type(), {Token::OBJECT_END, Token::STRING}))
    throw ValidationError{tokens.idx(),
                          "Expected OBJECT_END or STRING after OBJECT_BEGIN"};

  NODES children;
  while (!tokens.eol()) {
    size_t last_idx = tokens.idx();
    parseObjectProperty(true, tokens, children);

    if (consumeIfType(tokens, Token::OBJECT_END)) {
      nodes.push_back(Node{id, Node::Type::OBJECT}.setChildren(children));
      break;
    }

    if (tokens.idx() == last_idx)
      throw ValidationError{last_idx, "Parser got stuck in object definition"};
  }
}

void parseArray(TOKENS& tokens, NODES& nodes, std::string_view id) {
  if (!consumeIfType(tokens, Token::ARRAY_BEGIN))
    throw ValidationError{tokens.idx(), "Not an ARRAY_BEGIN"};
  if (tokens.eol())
    throw ValidationError{tokens.idx(), "Unexpected EOF after ARRAY_BEGIN"};
  if (hasObjectId(tokens))
    throw ValidationError{tokens.idx(),
                          "Unexpected property id after ARRAY_BEGIN"};

  NODES children;
  while (!tokens.eol()) {
    size_t last_idx = tokens.idx();
    parseObjectProperty(false, tokens, children);

    if (consumeIfType(tokens, Token::ARRAY_END)) {
      nodes.push_back(Node{id, Node::Type::ARRAY}.setChildren(children));
      break;
    }

    if (tokens.idx() == last_idx)
      throw ValidationError{last_idx, "Parser got stuck in array definition"};
  }
}

std::vector<Node> Parser::parse(const std::vector<Token>& tokens) {
  TOKENS js{tokens};
  if (js.eol()) return {};

  if (!is_in(js.current().type(),
             {Token::NUMBER, Token::NULLOPT, Token::BOOLEAN, Token::STRING,
              Token::OBJECT_BEGIN, Token::ARRAY_BEGIN}))
    throw ValidationError{js.idx(), "First element must be OBJECT or VALUE"};

  NODES nodes;
  parseObjectProperty(false, js, nodes);

  if (!js.eol())
    throw ValidationError{js.idx(), "Unexpected token after ROOT object"};

  return nodes;
}

}  // namespace json
