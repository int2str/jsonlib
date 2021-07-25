#ifndef VALIDATABLE_H
#define VALIDATABLE_H

#include <string>

namespace json {

struct ValidationError {
  size_t offset;
  std::string message;
};

}  // namespace json

#endif