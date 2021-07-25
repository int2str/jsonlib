#include "token.h"

namespace json {

Token::Token(Type type) : type_{type}, value_{} {}
Token::Token(std::string_view value)
    : type_{Token::STRING}, value_{std::string{value}} {}
Token::Token(double value) : type_{Token::NUMBER}, value_{value} {}
Token::Token(bool value) : type_{Token::BOOLEAN}, value_{value} {}

Token::Type Token::type() const { return type_; }
double Token::asNumber() const { return std::get<double>(value_); }
bool Token::asBool() const { return std::get<bool>(value_); }
const std::string& Token::asString() const {
  return std::get<std::string>(value_);
}

}  // namespace json