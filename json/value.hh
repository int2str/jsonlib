#ifndef VALUE_HH
#define VALUE_HH

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
  double value{};
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

#endif  // VALUE_HH