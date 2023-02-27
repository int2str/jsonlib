#ifndef CHARACTER_UTILS_HH
#define CHARACTER_UTILS_HH

#include <string_view>

#include "utility/fixed_stack.hh"
#include "utility/one_of.hh"

namespace json::internal {

[[nodiscard]] constexpr auto isDigit(char chr) -> bool {
  //" The digits are the code points U+0030 through U+0039.
  return chr >= '0' && chr <= '9';
}

[[nodiscard]] constexpr auto isLowerCaseAlpha(char chr) -> bool {
  return chr >= 'a' && chr <= 'z';
}

[[nodiscard]] constexpr auto extractLowerCaseAlpha(std::string_view str)
    -> std::string_view {
  size_t idx = 0;
  while (idx < str.length() && isLowerCaseAlpha(str[idx])) ++idx;
  return str.substr(0, idx);
}

enum class NumberParserState {
  ExpectOptionalMinus,
  ExpectLeadingZeroOrDigit,
  ExpectOptionalPeriod,
  ExpectOptionalExponent,
  ExpectOptionalDigit,
  ExpectDigit,
  ExpectOptionalPlusOrMinus
};

using Stack = FixedStack<NumberParserState, 10>;

inline auto hasMandatoryItems(Stack& states) -> bool {
  while (!states.empty()) {
    if (states.pop() == one_of(NumberParserState::ExpectLeadingZeroOrDigit,
                               NumberParserState::ExpectDigit))
      return true;
  }
  return false;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity): I tried ...
inline auto extractNumber(std::string_view str) -> std::string_view {
  Stack states{{NumberParserState::ExpectOptionalExponent,
                NumberParserState::ExpectOptionalPeriod,
                NumberParserState::ExpectLeadingZeroOrDigit,
                NumberParserState::ExpectOptionalMinus}};

  size_t matched = 0;
  while (matched != str.length() && !states.empty()) {
    auto ch = str[matched];

    switch (states.pop()) {
      case NumberParserState::ExpectOptionalMinus:
        matched += static_cast<size_t>(ch == '-');
        break;

      case NumberParserState::ExpectLeadingZeroOrDigit:
        if (!isDigit(ch)) return "";
        if (ch != '0') states.push(NumberParserState::ExpectOptionalDigit);
        ++matched;
        break;

      case NumberParserState::ExpectOptionalDigit:
        if (!isDigit(ch)) continue;
        states.push(NumberParserState::ExpectOptionalDigit);
        ++matched;
        break;

      case NumberParserState::ExpectDigit:
        if (!isDigit(ch)) return "";
        ++matched;
        break;

      case NumberParserState::ExpectOptionalPeriod:
        if (isDigit(ch)) return "";
        if (ch != '.') continue;
        states.push(NumberParserState::ExpectOptionalDigit);
        states.push(NumberParserState::ExpectDigit);
        ++matched;
        break;

      case NumberParserState::ExpectOptionalExponent:
        if (ch != one_of('e', 'E')) continue;
        states.push(NumberParserState::ExpectOptionalDigit);
        states.push(NumberParserState::ExpectDigit);
        states.push(NumberParserState::ExpectOptionalPlusOrMinus);
        ++matched;
        break;

      case NumberParserState::ExpectOptionalPlusOrMinus:
        matched += static_cast<size_t>(ch == one_of('+', '-'));
        break;
    }
  }

  if (hasMandatoryItems(states)) return "";
  return str.substr(0, matched);
}

}  // namespace json::internal

#endif  // CHARACTER_UTILS_HH