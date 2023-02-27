#ifndef TOKENIZER_HH
#define TOKENIZER_HH

#include <array>
#include <functional>
#include <string_view>

#include "json/character_utils.hh"
#include "json/status.hh"
#include "json/token.hh"
#include "utility/flat_map.hh"

namespace json::internal {

class Tokenizer {
 public:
  static auto parse(std::string_view& json) -> StatusOr<Token> {
    using Parser = std::function<StatusOr<Token>(std::string_view&)>;
    static const std::array<Parser, 6> Parsers{
        Tokenizer::skipWhitespace,     Tokenizer::parseStructuralTokens,
        Tokenizer::parseLiteralTokens, Tokenizer::parseNumberToken,
        Tokenizer::parseStringToken,   Tokenizer::abortIfNotEmpty};

    for (const auto& parser : Parsers) {
      auto maybe_token = parser(json);
      if (maybe_token) return *maybe_token;
      if (maybe_token.status() != Status::Ok) return maybe_token.status();
    }

    return Status::Ok;
  }

 private:
  static auto skipWhitespace(std::string_view& json) -> StatusOr<Token> {
    //"
    //" Insignificant whitespace is allowed before or after any token.
    //" Whitespace is any sequence of one or more of the following code points:
    //" character tabulation (U+0009), line feed (U+000A), carriage return
    //" (U+000D), and space (U+0020). Whitespace is not allowed within any
    //" token, except that space is allowed in strings.
    //"
    json.remove_prefix(
        std::min(json.find_first_not_of("\t\n\r "), json.size()));
    return Status::Ok;
  }

  [[nodiscard]] static auto parseStructuralTokens(std::string_view& json)
      -> StatusOr<Token> {
    //" The six structural tokens:
    static constexpr auto StructuralTokens =
        FlatMap<char, Token::Type, 6>{{'[', Token::Type::LeftSquareBracket},
                                      {'{', Token::Type::LeftCurlyBracket},
                                      {']', Token::Type::RightSquareBracket},
                                      {'}', Token::Type::RightCurlyBracket},
                                      {':', Token::Type::Colon},
                                      {',', Token::Type::Comma}};

    if (json.empty() || !StructuralTokens.contains(json[0])) return Status::Ok;
    auto token = Token{StructuralTokens[json[0]], ""};
    json.remove_prefix(1);
    return token;
  }

  [[nodiscard]] static auto parseLiteralTokens(std::string_view& json)
      -> StatusOr<Token> {
    //"
    //" These are the three literal name tokens:
    //"   true   U+0074 U+0072 U+0075 U+0065
    //"   false  U+0066 U+0061 U+006C U+0073 U+0065
    //"   null   U+006E U+0075 U+006C U+006C
    //"

    // Note: The code points given indicate these literal tokens must be
    // lowercase.
    static constexpr auto LiteralTokens =
        FlatMap<std::string_view, Token::Type, 3>{{"true", Token::Type::True},
                                                  {"false", Token::Type::False},
                                                  {"null", Token::Type::Null}};
    auto starts_with = json::internal::extractLowerCaseAlpha(json);
    if (starts_with.empty() || !LiteralTokens.contains(starts_with))
      return Status::Ok;
    json.remove_prefix(starts_with.size());
    return Token{LiteralTokens[starts_with], ""};
  }

  [[nodiscard]] static auto parseNumberToken(std::string_view& json)
      -> StatusOr<Token> {
    if (json.empty() || (json[0] != '-' && !json::internal::isDigit(json[0])))
      return Status::Ok;

    auto number_slice = internal::extractNumber(json);
    if (number_slice.empty()) return Status::UnexpectedCharacter;

    auto token = Token{Token::Type::Number, number_slice};
    json.remove_prefix(number_slice.length());
    return token;
  }

  [[nodiscard]] static auto parseStringToken(std::string_view& json)
      -> StatusOr<Token> {
    if (json.empty() || json[0] != '"') return Status::Ok;

    //"
    //" 9 String
    //"
    //" A string is a sequence of Unicode code points wrapped with quotation
    //" marks (U+0022). All code points may be placed within the quotation marks
    //" except for the code points that must be escaped: quotation mark
    //" (U+0022), " reverse solidus (U+005C), and the control characters U+0000
    //" to U+001F.
    //"

    // TODO(ae): Escaped characters ...
    // TODO(ae): Four digit hex codes

    auto closing_quote = json.find('"', 1);
    if (closing_quote == std::string_view::npos)
      return Status::UnexpectedCharacter;

    auto token = Token{Token::Type::String, json.substr(1, closing_quote - 1)};
    json.remove_prefix(closing_quote + 1);
    return token;
  }

  [[nodiscard]] static auto abortIfNotEmpty(std::string_view& json)
      -> StatusOr<Token> {
    skipWhitespace(json);
    return json.empty() ? Status::Ok : Status::UnexpectedCharacter;
  }
};

}  // namespace json::internal

#endif  // TOKENIZER_HH