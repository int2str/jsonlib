#include "jsonlib/node.h"

#include <algorithm>

namespace {

struct NodeIdMatches {
  explicit NodeIdMatches(std::string_view id) : id_{id} {}
  [[nodiscard]] bool operator()(const json::Node& node) const {
    return node.id() == id_;
  }

 private:
  const std::string_view id_;
};

}  // namespace

namespace json {

Node::Node(std::string_view id, Type type) : id_{id}, type_{type} {}
Node::Node(std::string_view id, std::string_view value)
    : id_{id}, type_{Type::STRING}, value_{std::string{value}} {}
Node::Node(std::string_view id, double value)
    : id_{id}, type_{Type::NUMBER}, value_{value} {}
Node::Node(std::string_view id, bool value)
    : id_{id}, type_{Type::BOOLEAN}, value_{value} {}

Node& Node::setChildren(const std::vector<Node>& children) {
  children_ = children;
  return *this;
}

Node::Type Node::type() const { return type_; }
std::string Node::id() const { return id_; }
const std::vector<Node>& Node::children() const { return children_; }
double Node::asNumber() const { return std::get<double>(value_); }
int Node::asInteger() const { return static_cast<int>(asNumber()); }
unsigned Node::asUnsigned() const { return static_cast<unsigned>(asNumber()); }
bool Node::asBool() const { return std::get<bool>(value_); }
const std::string& Node::asString() const {
  return std::get<std::string>(value_);
}

[[nodiscard]] bool Node::has(std::string_view key) const {
  if (key.empty()) return false;
  return std::any_of(children_.cbegin(), children_.cend(), NodeIdMatches{key});
}

[[nodiscard]] const Node& Node::get(std::string_view key) const {
  auto it =
      std::find_if(children_.cbegin(), children_.cend(), NodeIdMatches{key});
  if (it == children_.cend()) throw "Invalid node property";
  return *it;
}

const Node& Node::operator[](size_t idx) const {
  if (type() != Type::ARRAY)
    throw "Can't index into something that isn't an array...";
  return children_.at(idx);
}

const Node& Node::operator[](std::string_view property) const {
  if (type() != Type::OBJECT)
    throw "Can't access property of something that isn't an object...";
  return get(property);
}

}  // namespace json
