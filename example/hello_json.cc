#include <iostream>

#include "jsonlib/json.h"

int main() {
  json::JSON json{R"({ "hello": "jsonlib!" })"};
  std::cout << "Hello, " << json["hello"].asString() << "\n";
}