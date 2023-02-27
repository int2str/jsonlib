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

template <>
struct fmt::formatter<json::internal::Node> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const json::internal::Node& node, FormatContext& ctx) {
    fmt::format_to(ctx.out(), "Parent: {} Child nodes: {} - '{}': {}",
                   node.parent, node.children, node.name, node.value);
    return ctx.out();
  }
};

#endif  // NODE_HH