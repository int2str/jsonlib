#ifndef JSON_H
#define JSON_H

#include <string_view>
#include <vector>

#include "node.h"

namespace json {

class JSON {
 public:
  explicit JSON(std::string_view json);

  [[nodiscard]] const Node& root() const;

  [[nodiscard]] const Node& operator[](size_t idx) const;
  [[nodiscard]] const Node& operator[](std::string_view property) const;

 private:
  const std::vector<Node> nodes_;
};

}  // namespace json

#endif