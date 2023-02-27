#ifndef TOKEN_HH
#define TOKEN_HH

#include <fmt/format.h>

#include <string_view>

#include "utility/one_of.hh"

namespace json {

//"
//" 4 JSON Text
//"
//" A JSON text is a sequence of tokens formed from Unicode code points that
//" conforms to the JSON value grammar. The set of tokens includes six
//" structural tokens, strings, numbers, and three literal name tokens.
//"

struct Token {
  enum class Type {
    String,
    Number,

    //" The six structural tokens:
    LeftSquareBracket,
    LeftCurlyBracket,
    RightSquareBracket,
    RightCurlyBracket,
    Colon,
    Comma,

    //" These are the three literal name tokens:
    True,
    False,
    Null
  };

  Type type;
  std::string_view value;

  [[nodiscard]] constexpr auto isDataType() const -> bool {
    return type == one_of(Type::String, Type::Number);
  }

  [[nodiscard]] constexpr auto isLiteral() const -> bool {
    return type == one_of(Type::True, Type::False, Type::Null);
  }

  [[nodiscard]] constexpr auto isOpening() const -> bool {
    return type == one_of(Type::LeftSquareBracket, Type::LeftCurlyBracket);
  }

  [[nodiscard]] constexpr auto startsAValue() const -> bool {
    return isDataType() || isLiteral() || isOpening();
  }
};

[[nodiscard]] constexpr auto tokenTypeToString(const Token::Type& type)
    -> std::string_view {
  switch (type) {
    case Token::Type::String:
      return "String";
    case Token::Type::Number:
      return "Number";
    case Token::Type::LeftSquareBracket:
      return "LeftSquareBracket";
    case Token::Type::LeftCurlyBracket:
      return "LeftCurlyBracket";
    case Token::Type::RightSquareBracket:
      return "RightSquareBracket";
    case Token::Type::RightCurlyBracket:
      return "RightCurlyBracket";
    case Token::Type::Colon:
      return "Colon";
    case Token::Type::Comma:
      return "Comma";
    case Token::Type::True:
      return "True";
    case Token::Type::False:
      return "False";
    case Token::Type::Null:
      return "Null";
  }
}

}  // namespace json

template <>
struct fmt::formatter<json::Token> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::Token& token, FormatContext& ctx) {
    fmt::format_to(ctx.out(), "{}", json::tokenTypeToString(token.type));
    if (token.type ==
        one_of(json::Token::Type::String, json::Token::Type::Number))
      fmt::format_to(ctx.out(), " ({})", token.value);
    return ctx.out();
  }
};

#endif  // TOKEN_HH