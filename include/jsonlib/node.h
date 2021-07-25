#ifndef NODE_H
#define NODE_H

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace json {

class Node {
 public:
  enum class Type : uint8_t { OBJECT, ARRAY, NUMBER, STRING, BOOLEAN, NULLOPT };

  explicit Node(std::string_view id, Type type);
  explicit Node(std::string_view id, std::string_view value);
  explicit Node(std::string_view id, double value);
  explicit Node(std::string_view id, bool value);

  Node& setChildren(const std::vector<Node>& children);

  [[nodiscard]] Type type() const;
  [[nodiscard]] std::string id() const;
  [[nodiscard]] const std::vector<Node>& children() const;
  [[nodiscard]] double asNumber() const;
  [[nodiscard]] int asInteger() const;
  [[nodiscard]] bool asBool() const;
  [[nodiscard]] const std::string& asString() const;

  [[nodiscard]] bool has(std::string_view key) const;
  [[nodiscard]] const Node& get(std::string_view key) const;

  [[nodiscard]] const Node& operator[](size_t idx) const;
  [[nodiscard]] const Node& operator[](std::string_view property) const;

 private:
  std::string id_;
  Type type_;
  std::variant<bool, double, std::string> value_{};
  std::vector<Node> children_{};
};

}  // namespace json

#endif