#ifndef TOKEN_HH
#define TOKEN_HH

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

}  // namespace json

#endif  // TOKEN_HH