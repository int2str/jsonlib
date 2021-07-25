#include <iostream>

#include "jsonlib/json.h"

int main() {
  json::JSON json{"{ \"hello\": \"jsonlib!\" }"};
  std::cout << "Hello, " << json["hello"].asString() << "\n";
}