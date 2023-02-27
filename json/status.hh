#ifndef STATUS_HH
#define STATUS_HH

#include <fmt/format.h>

#include <string_view>

#include "utility/status_or.hh"

namespace json {

enum class Status {
  Ok,
  UnexpectedCharacter,
  UnexpectedToken,
  Unimplemented,
};

template <typename T>
using StatusOr = StatusOrBase<Status, T>;

constexpr auto textForStatus(const Status& status) -> std::string_view {
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

}  // namespace json

template <>
struct fmt::formatter<json::Status> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::Status& status, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", json::textForStatus(status));
  }
};

#endif  // STATUS_HH