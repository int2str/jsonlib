#ifndef JSON_FORMATTER_HH
#define JSON_FORMATTER_HH

#include <fmt/format.h>

#include <string_view>

#include "json/status.hh"
#include "json/token.hh"
#include "json/value.hh"

namespace json::formatter {

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

[[nodiscard]] constexpr auto textForStatus(const Status& status)
    -> std::string_view {
  switch (status) {
    case Status::Ok:
      return "OK";
    case Status::UnexpectedCharacter:
      return "Tokenizer found an invalid character";
    case Status::UnexpectedToken:
      return "Parser found an invalid token";
    case Status::Unimplemented:
      return "Unimplemented feature";
  }
}

}  // namespace json::formatter

template <>
struct fmt::formatter<json::Value> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::Value& value, FormatContext& ctx) {
    std::visit(
        overloaded{
            [&](const json::Object&) { fmt::format_to(ctx.out(), "Object"); },
            [&](const json::Array&) { fmt::format_to(ctx.out(), "Array"); },
            [&](const json::Number& number) {
              fmt::format_to(ctx.out(), "Number ({})", number.value);
            },
            [&](const json::String& string) {
              fmt::format_to(ctx.out(), "String ({})", string.value);
            },
            [&](const json::Boolean& boolean) {
              fmt::format_to(ctx.out(), "Boolean ({})", boolean.value);
            },
            [&](const json::Null&) { fmt::format_to(ctx.out(), "Null"); }},
        value);
    return ctx.out();
  }
};

template <>
struct fmt::formatter<json::Status> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::Status& status, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}",
                          json::formatter::textForStatus(status));
  }
};

template <>
struct fmt::formatter<json::Token> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::Token& token, FormatContext& ctx) {
    fmt::format_to(ctx.out(), "{}",
                   json::formatter::tokenTypeToString(token.type));
    if (token.type ==
        one_of(json::Token::Type::String, json::Token::Type::Number))
      fmt::format_to(ctx.out(), " ({})", token.value);
    return ctx.out();
  }
};

#endif  // JSON_FORMATTER_HH