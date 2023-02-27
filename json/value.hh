#ifndef VALUE_HH
#define VALUE_HH

#include <fmt/format.h>

#include <string>
#include <variant>

#include "utility/overloaded_helper.hh"

namespace json {

//"
//" 5 JSON Values
//"
//" A JSON value can be an object, array, number, string, true, false, or null.
//"

struct Object {};

struct Array {};

struct Number {
  float value{};
};

struct String {
  std::string value;
};

struct Boolean {
  bool value{false};
};

struct Null {};

using Value = std::variant<Object, Array, Number, String, Boolean, Null>;

}  // namespace json

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

#endif  // VALUE_HH