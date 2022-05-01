#include "tokenizer.h"

#include <functional>
#include <string>

#include "consumable.h"

namespace {

constexpr bool isWhitespace(char ch) {
  return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t';
}

constexpr bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }

constexpr bool isLowerAlha(char ch) { return ch >= 'a' && ch <= 'z'; }

std::string extractLowerCaseString(std::string_view in, size_t offset) {
  size_t len = 0;
  while (offset + len != in.size() && isLowerAlha(in[offset + len])) ++len;
  return std::string{in.substr(offset, len)};
}

}  // namespace

namespace json {

using JSON = Consumable<std::string_view, char>;
using Tokens = std::vector<Token>;
using Parser = std::function<void(JSON& json, Tokens& tokens)>;

void skipWhitespace(JSON& json, Tokens& /*unused*/) {
  while (!json.eol() && isWhitespace(json.current())) json.skip(1);
}

void parsePunctuation(JSON& json, Tokens& tokens) {
  if (json.consume_if('{')) {
    tokens.push_back(Token{Token::OBJECT_BEGIN});

  } else if (json.consume_if('}')) {
    tokens.push_back(Token{Token::OBJECT_END});

  } else if (json.consume_if('[')) {
    tokens.push_back(Token{Token::ARRAY_BEGIN});

  } else if (json.consume_if(']')) {
    tokens.push_back(Token{Token::ARRAY_END});

  } else if (json.consume_if(',')) {
    tokens.push_back(Token{Token::COMMA});

  } else if (json.consume_if(':')) {
    tokens.push_back(Token{Token::COLON});
  }
}

void parseNull(JSON& json, Tokens& tokens) {
  if (json.eol() || !isLowerAlha(json.current())) return;

  auto s = extractLowerCaseString(json.data(), json.idx());
  if ("null" == s) {
    tokens.push_back(Token{Token::NULLOPT});
    json.skip(s.size());
  }
}

void parseBoolean(JSON& json, Tokens& tokens) {
  if (json.eol() || !isLowerAlha(json.current())) return;

  auto s = extractLowerCaseString(json.data(), json.idx());
  if ("true" == s) {
    tokens.push_back(Token{true});
    json.skip(s.size());

  } else if ("false" == s) {
    tokens.push_back(Token{false});
    json.skip(s.size());
  }
}

void parseNumber(JSON& json, Tokens& tokens) {
  if (json.current() != '-' && !isDigit(json.current())) return;

  if (json.current() == '-' && !json.canPeek())
    throw ValidationError{json.idx(), "Invalid negative number (trailing -)"};

  if (json.current() == '-' && !isDigit(json.peek()))
    throw ValidationError{json.idx() + 1,
                          "Invalid negative number (. after -)"};

  std::string number_str{json.consume_if('-') ? "-" : ""};

  bool has_decimal = false;
  bool has_exponent = false;
  while (!json.eol() && (isDigit(json.current()) || json.current() == '.' ||
                         json.current() == 'E' || json.current() == 'e' ||
                         json.current() == '-')) {
    if (json.current() == '.') {
      if (has_decimal)
        throw ValidationError{json.idx(), "Multiple decimal points"};
      has_decimal = true;

    } else if (json.current() == 'E' || json.current() == 'e') {
      if (has_exponent) throw ValidationError{json.idx(), "Multiple exponents"};
      has_exponent = true;

    } else if (json.current() == '-') {
      if (json.prev() != 'E' && json.prev() != 'e')
        throw ValidationError{json.idx(), "'-' without exponent declaration"};
    }

    number_str += json.consume();
  }

  tokens.push_back(Token{std::stof(number_str)});
}

void parseString(JSON& json, Tokens& tokens) {
  if (json.eol() || !json.canPeek() || !json.consume_if('"')) return;

  const size_t initial_offset = json.idx();
  std::string token{};

  while (!json.eol()) {
    switch (json.current()) {
      case '"':
        json.skip(1);
        tokens.push_back(Token{token});
        return;
      case '\\':
        if (!json.canPeek())
          throw ValidationError{initial_offset, "Unterminated escape sequence"};
        switch (json.peek()) {
          case '"':
            token += '"';
            break;
          case '\\':
            token += '\\';
            break;
          default:
            throw ValidationError{json.idx(),
                                  "Invalid escape sequence in string"};
        }
        json.skip(2);
        break;
      default:
        token += json.consume();
    }
  }

  throw ValidationError{initial_offset,
                        "Unterminated string/string parsing error"};
}

std::vector<Token> Tokenizer::parse(std::string_view json) {
  std::vector<Token> tokens;
  Consumable<std::string_view, char> jstream{json};

  static const std::vector<Parser> Parsers{skipWhitespace, parsePunctuation,
                                           parseNull,      parseBoolean,
                                           parseNumber,    parseString};

  while (!jstream.eol()) {
    auto last_idx = jstream.idx();

    for (const auto& parser : Parsers) {
      if (!jstream.eol()) parser(jstream, tokens);
    }

    if (jstream.idx() == last_idx)
      throw ValidationError{jstream.idx(), "Tokenizer got stuck"};
  }

  return tokens;
}

}  // namespace json
