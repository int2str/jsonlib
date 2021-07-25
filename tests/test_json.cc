
#include "jsonlib/json.h"
#include "testrunner/testrunner.h"

namespace json {
namespace test {

TEST(JsonAcceptsEmptyString) { JSON json{""}; }

TEST(JsonParsesSimpleValues) {
  JSON j_null{"null"};
  EXPECT_EQ(j_null.root().type(), Node::Type::NULLOPT);

  JSON j_bool{"true"};
  EXPECT_EQ(j_bool.root().type(), Node::Type::BOOLEAN);
  EXPECT_EQ(j_bool.root().asBool(), true);
}

TEST(JsonParsesSimpleObject) {
  JSON j_obj{"{\"Hello\": \"Object\"}"};
  EXPECT_EQ(j_obj["Hello"].type(), Node::Type::STRING);
  EXPECT_EQ(j_obj["Hello"].asString(), "Object");
}

TEST(JsonRejectsUnknownProperty) {
  JSON json{"{\"a\":\"b\"}"};
  [[maybe_unused]] Node::Type type;
  EXPECT_THROW(type = json["Invalid"].type());
}

TEST(JsonParsesNestedObject) {
  JSON j_nested{"{\"Hello\": {\"Nested\": \"Object\"}}"};
  EXPECT_EQ(j_nested["Hello"].type(), Node::Type::OBJECT);
  EXPECT_EQ(j_nested["Hello"]["Nested"].type(), Node::Type::STRING);
  EXPECT_EQ(j_nested["Hello"]["Nested"].asString(), "Object");
}

TEST(JsonParsesNestedArray) {
  JSON j_nested{"[1, [2, 3], 4]"};
  EXPECT_EQ(j_nested[0].type(), Node::Type::NUMBER);
  EXPECT_EQ(j_nested[0].asInteger(), 1);
  EXPECT_EQ(j_nested[1].type(), Node::Type::ARRAY);
  EXPECT_EQ(j_nested[1][0].type(), Node::Type::NUMBER);
  EXPECT_EQ(j_nested[1][0].asInteger(), 2);
  EXPECT_EQ(j_nested[1][1].type(), Node::Type::NUMBER);
  EXPECT_EQ(j_nested[1][1].asInteger(), 3);
  EXPECT_EQ(j_nested[2].type(), Node::Type::NUMBER);
  EXPECT_EQ(j_nested[2].asInteger(), 4);
}

TEST(JsonRejectsOutOfBoundsArrayAccess) {
  JSON j_array{"[1, [2, 3], 4]"};
  [[maybe_unused]] Node::Type type;
  EXPECT_THROW(type = j_array[5].type());
  EXPECT_THROW(type = j_array[1][3].type());
}

}  // namespace test
}  // namespace json
