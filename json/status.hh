#ifndef STATUS_HH
#define STATUS_HH

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

}  // namespace json

#endif  // STATUS_HH