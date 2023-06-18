#ifndef NODE_HH
#define NODE_HH

#include <string>

#include "json/value.hh"

namespace json::internal {

static constexpr auto Root = static_cast<size_t>(-1);

struct Node {
  Value value;
  size_t parent{};
  std::string name{};
  size_t children{};
};

}  // namespace json::internal

#endif  // NODE_HH