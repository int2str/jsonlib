#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>

namespace json {

class Token {
 public:
  enum Type {
    OBJECT_BEGIN,
    OBJECT_END,
    ARRAY_BEGIN,
    ARRAY_END,
    COLON,
    COMMA,
    NUMBER,
    STRING,
    BOOLEAN,
    NULLOPT
  };

  explicit Token(Type type);
  explicit Token(std::string_view value);
  explicit Token(double value);
  explicit Token(bool value);

  [[nodiscard]] Type type() const;
  [[nodiscard]] double asNumber() const;
  [[nodiscard]] bool asBool() const;
  [[nodiscard]] const std::string& asString() const;

 private:
  const Type type_;
  const std::variant<bool, double, std::string> value_;
};

}  // namespace json

#endif