# JSONlib
A simple JSON parsing library.
Once again, don't use this library ... There are much JSON parsers out there.

# Why?
This library was created mainly for research/fun purposes to see how it could be done.

# How to use
Simple example (can be found in example/ folder):

    #include <iostream>
    #include "jsonlib/json.h"

    int main() {
      json::JSON json{"{ \"hello\": \"jsonlib!\" }"};
      std::cout << "Hello, " << json["hello"].asString() << "\n";
    }
