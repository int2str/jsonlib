#ifndef JSON_HH
#define JSON_HH

#include <algorithm>
#include <optional>
#include <variant>

#include "json/parser.hh"
#include "json/status.hh"

//
// Document references in this file refer to:
//
//    ECMA International Standard ECMA-404
//    2nd Edition / December 2012
//
//    The JSON Data Interchange Syntax
//
// Note:
// Codepoints are unicode. For simplicity sake, initial implementation of this
// parser uses char's instead.
//
// TODO(ae): Switch to Unicode code points eventually ...

namespace json {

template <typename T>
class ValueIterator {
 public:
  [[nodiscard]] constexpr auto operator<=>(
      const ValueIterator&) const noexcept = default;

  [[nodiscard]] auto string() const -> std::optional<std::string_view> {
    return this->accessAs<json::String, std::string_view>();
  }

  [[nodiscard]] auto number() const -> std::optional<double> {
    return this->accessAs<json::Number, double>();
  }

  [[nodiscard]] auto boolean() const -> std::optional<bool> {
    return this->accessAs<json::Boolean, bool>();
  }

  [[nodiscard]] auto value() const -> const Value& {
    return container_->at(idx_)->value;
  }

  [[nodiscard]] auto name() const -> std::optional<std::string_view> {
    if (*this == container_->end()) return std::nullopt;
    return container_->at(idx_)->name;
  }

  [[nodiscard]] auto operator*() const -> const ValueIterator& { return *this; }

  [[nodiscard]] auto operator[](std::string_view key) const -> ValueIterator {
    for (const auto& child : *this) {
      if (child.node()->name == key) return child;
    }
    return container_->end();
  }

  [[nodiscard]] auto operator[](size_t offset) const -> ValueIterator {
    for (const auto& child : *this) {
      if (0 == offset--) return child;
    }
    return container_->end();
  }

  [[nodiscard]] auto has(std::string_view key) const -> bool {
    return (*this)[key] != container_->end();
  }

  [[nodiscard]] auto begin() const -> ValueIterator {
    if (*this == container_->end()) return *this;
    return ValueIterator{container_, idx_ + 1};
  }

  [[nodiscard]] auto end() const -> ValueIterator {
    if (*this == container_->end()) return *this;
    return ValueIterator{container_, idx_ + 1 + container_->at(idx_)->children};
  }

  auto operator++() -> ValueIterator& {
    if (*this != container_->end()) idx_ += container_->at(idx_)->children + 1;
    return *this;
  }

 private:
  friend T;
  ValueIterator(const T* container, size_t idx)
      : container_{container}, idx_{idx} {}

  [[nodiscard]] auto node() const -> const internal::Node* {
    return container_->at(idx_);
  }

  template <typename Alternative, typename Returns>
  [[nodiscard]] auto accessAs() const -> std::optional<Returns> {
    if (node() == nullptr) return std::nullopt;
    if (!std::holds_alternative<Alternative>(node()->value))
      return std::nullopt;
    return std::get<Alternative>(node()->value).value;
  }

  const T* container_;
  size_t idx_;
};

class Json {
  std::vector<internal::Node> nodes_;

  explicit Json(std::vector<internal::Node>&& nodes)
      : nodes_{std::move(nodes)} {};

 public:
  [[nodiscard]] static auto parse(std::string_view json_source)
      -> StatusOr<Json> {
    internal::Parser parser;

    auto status = parser.parse(json_source);
    if (status != Status::Ok) return status;

    return Json{parser.nodes()};
  }

  [[nodiscard]] auto operator[](std::string_view key) const
      -> ValueIterator<Json> {
    if (nodes_.empty()) return end();
    return begin()[key];
  }

  [[nodiscard]] auto has(std::string_view key) const -> bool {
    return (*this)[key] != end();
  }

  [[nodiscard]] auto begin() const -> ValueIterator<Json> {
    return ValueIterator{this, 0};
  }

  [[nodiscard]] auto end() const -> ValueIterator<Json> {
    return ValueIterator{this, nodes_.size()};
  }

  [[nodiscard]] auto value() const -> const Value& {
    return nodes_.at(0).value;
  }

 private:
  friend ValueIterator<Json>;
  [[nodiscard]] auto at(size_t idx) const -> const internal::Node* {
    if (idx >= nodes_.size()) return nullptr;
    return &nodes_.at(idx);
  }
};

}  // namespace json

#endif  // JSON_HH