#include <sstream>
#include <string_view>

#include "parser.h"
#include "testrunner/testrunner.h"
#include "tokenizer.h"

namespace {

std::vector<json::Node> parse(std::string_view json) {
  try {
    return json::Parser::parse(json::Tokenizer::parse(json));

  } catch (json::ValidationError error) {
    std::ostringstream error_str;
    error_str << error.message << " at offset "
              << static_cast<int>(error.offset);
    throw std::string{error_str.str()};
  }
}

json::Node getFirstChild(std::string_view json) {
  auto nodes = parse(json);
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), json::Node::Type::OBJECT);
  EXPECT_EQ(nodes[0].children().size(), 1);
  return nodes[0].children()[0];
}

json::Node getFirstArrayElement(std::string_view json) {
  auto nodes = parse(json);
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), json::Node::Type::ARRAY);
  EXPECT_EQ(nodes[0].children().size(), 1);
  return nodes[0].children()[0];
}

}  // namespace

namespace json {
namespace test {

TEST(ParserHandlesEmptyTokenList) {
  auto nodes = parse("");
  ASSERT_TRUE(nodes.empty());
}

TEST(ParserRejectsInvalidBeginning) {
  EXPECT_THROW(parse("]"));
  EXPECT_THROW(parse("}"));
}

TEST(ParserAcceptsNullStart) {
  auto t_null = parse("null");
  EXPECT_EQ(t_null.size(), 1);
  EXPECT_EQ(t_null[0].type(), Node::Type::NULLOPT);
}

TEST(ParserAcceptsBooleanStart) {
  auto t_true = parse("true");
  EXPECT_EQ(t_true.size(), 1);
  EXPECT_EQ(t_true[0].type(), Node::Type::BOOLEAN);
  EXPECT_EQ(t_true[0].asBool(), true);

  auto t_false = parse("false");
  EXPECT_EQ(t_false.size(), 1);
  EXPECT_EQ(t_false[0].type(), Node::Type::BOOLEAN);
  EXPECT_EQ(t_false[0].asBool(), false);
}

TEST(ParserAcceptsStringStart) {
  auto t_string = parse("\"Hello World!\"");
  EXPECT_EQ(t_string.size(), 1);
  EXPECT_EQ(t_string[0].type(), Node::Type::STRING);
  EXPECT_EQ(t_string[0].asString(), "Hello World!");
}

TEST(ParserAcceptsNumberStart) {
  auto t_number = parse("42");
  EXPECT_EQ(t_number.size(), 1);
  EXPECT_EQ(t_number[0].type(), Node::Type::NUMBER);
  ASSERT_TRUE(t_number[0].asNumber() > 41.99);
  ASSERT_TRUE(t_number[0].asNumber() < 42.01);
}

TEST(ParserRejectsIncompleteObject) {
  EXPECT_THROW(parse("{"));
  EXPECT_THROW(parse("{\"a\""));
  EXPECT_THROW(parse("{\"a\","));
  EXPECT_THROW(parse("{\"a\":"));
  EXPECT_THROW(parse("{\"a\":}"));
  EXPECT_THROW(parse("{\"a\"}"));
  EXPECT_THROW(parse("{\"a\": 5"));
  EXPECT_THROW(parse("{\"a\": {\"b\": \"c\"}"));
}

TEST(ParserAcceptsEmptyObject) {
  auto nodes = parse("{}");
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), Node::Type::OBJECT);
}

TEST(ParserRejectsEmptyNestedObjects) { EXPECT_THROW(parse("{{}}")); }

TEST(ParserRejectsTokensAfterRootObject) {
  EXPECT_THROW(parse("{}{}"));
  EXPECT_THROW(parse("{}, 5"));
  EXPECT_THROW(parse("{} null"));
}

TEST(ParserAcceptsNumericPropertyObject) {
  auto child = getFirstChild("{\"number\": 5}");
  EXPECT_EQ(child.id(), "number");
  EXPECT_EQ(child.type(), Node::Type::NUMBER);
  ASSERT_TRUE(child.asNumber() > 4.99);
  ASSERT_TRUE(child.asNumber() < 5.01);
}

TEST(ParserAcceptsNullPropertyObject) {
  auto child = getFirstChild("{\"nullopt\": null}");
  EXPECT_EQ(child.id(), "nullopt");
  EXPECT_EQ(child.type(), Node::Type::NULLOPT);
}

TEST(ParserAcceptsBooleanPropertyObject) {
  auto c_true = getFirstChild("{\"yes\": true}");
  EXPECT_EQ(c_true.id(), "yes");
  EXPECT_EQ(c_true.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(c_true.asBool(), true);

  auto c_false = getFirstChild("{\"no\": false}");
  EXPECT_EQ(c_false.id(), "no");
  EXPECT_EQ(c_false.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(c_false.asBool(), false);
}

TEST(ParserAcceptsStringPropertyObject) {
  auto c_true = getFirstChild("{\"Hello\": \"World\"}");
  EXPECT_EQ(c_true.id(), "Hello");
  EXPECT_EQ(c_true.type(), Node::Type::STRING);
  EXPECT_EQ(c_true.asString(), "World");
}

TEST(ParserAcceptsNestedObjects) {
  auto c_nested = getFirstChild("{\"a\": {\"b\": \"c\"}}");
  EXPECT_EQ(c_nested.id(), "a");
  EXPECT_EQ(c_nested.type(), Node::Type::OBJECT);
  EXPECT_EQ(c_nested.children()[0].type(), Node::Type::STRING);
  EXPECT_EQ(c_nested.children()[0].asString(), "c");
}

TEST(ParserRejectsInvalidPropertyId) {
  EXPECT_THROW(parse("{null: 5}"));
  EXPECT_THROW(parse("{42: 6}"));
  EXPECT_THROW(parse("{:5}"));
}

TEST(ParserAcceptsMultiPropertyObject) {
  auto nodes = parse("{\"a\": true, \"b\": false}");
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), Node::Type::OBJECT);
  EXPECT_EQ(nodes[0].children().size(), 2);

  auto c_a = nodes[0].children()[0];
  EXPECT_EQ(c_a.id(), "a");
  EXPECT_EQ(c_a.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(c_a.asBool(), true);

  auto c_b = nodes[0].children()[1];
  EXPECT_EQ(c_b.id(), "b");
  EXPECT_EQ(c_b.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(c_b.asBool(), false);
}

TEST(ParserAcceptsEmptyArray) {
  auto nodes = parse("[]");
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), Node::Type::ARRAY);
  ASSERT_TRUE(nodes[0].children().empty());
}

TEST(ParserRejectsIncompleteArray) {
  EXPECT_THROW(parse("["));
  EXPECT_THROW(parse("[\"a\""));
  EXPECT_THROW(parse("[\"a\","));
  EXPECT_THROW(parse("[\"a\":"));
  EXPECT_THROW(parse("[\"a\":]"));
  EXPECT_THROW(parse("[\"a\": 5"));
  EXPECT_THROW(parse("[{\"b\": \"c\"}"));
}

TEST(ParserAcceptsSingleNullvalueArray) {
  auto child = getFirstArrayElement("[null]");
  EXPECT_EQ(child.type(), Node::Type::NULLOPT);
}

TEST(ParserAcceptsSingleNumberArray) {
  auto child = getFirstArrayElement("[ 1e2 ]");
  EXPECT_EQ(child.type(), Node::Type::NUMBER);
  ASSERT_TRUE(child.asNumber() > 99.99);
  ASSERT_TRUE(child.asNumber() < 100.01);
}

TEST(ParserAcceptsSingleStringArray) {
  auto child = getFirstArrayElement("[ \"Hello Array!\" ]");
  EXPECT_EQ(child.type(), Node::Type::STRING);
  EXPECT_EQ(child.asString(), "Hello Array!");
}

TEST(ParserAcceptsSingleObjectArray) {
  auto child = getFirstArrayElement("[{\"prop\": true}]");
  EXPECT_EQ(child.type(), Node::Type::OBJECT);
  EXPECT_EQ(child.children().size(), 1);

  auto prop = child.children()[0];
  EXPECT_EQ(prop.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(prop.id(), "prop");
  EXPECT_EQ(prop.asBool(), true);
}

TEST(ParserAcceptsNestedArrays) {
  auto inner = getFirstArrayElement("[[false]]");
  EXPECT_EQ(inner.type(), Node::Type::ARRAY);
  EXPECT_EQ(inner.children().size(), 1);

  auto value = inner.children()[0];
  EXPECT_EQ(value.type(), Node::Type::BOOLEAN);
  EXPECT_EQ(value.asBool(), false);
}

TEST(ParserAcceptsMultiItemArrays) {
  auto nodes = parse("[0, null, true, \"str\", {\"prop\": false}]");
  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0].type(), Node::Type::ARRAY);

  auto elements = nodes[0].children();
  EXPECT_EQ(elements.size(), 5);
  EXPECT_EQ(elements[0].type(), Node::Type::NUMBER);
  EXPECT_EQ(elements[0].asNumber(), 0.0f);
  EXPECT_EQ(elements[1].type(), Node::Type::NULLOPT);
  EXPECT_EQ(elements[2].type(), Node::Type::BOOLEAN);
  EXPECT_EQ(elements[2].asBool(), true);
  EXPECT_EQ(elements[3].type(), Node::Type::STRING);
  EXPECT_EQ(elements[3].asString(), "str");
  EXPECT_EQ(elements[4].type(), Node::Type::OBJECT);
  EXPECT_EQ(elements[4].children().size(), 1);

  auto object = elements[4].children();
  EXPECT_EQ(object[0].type(), Node::Type::BOOLEAN);
  EXPECT_EQ(object[0].id(), "prop");
  EXPECT_EQ(object[0].asBool(), false);
}

TEST(ParserRejectsPropertyValueArray) { EXPECT_THROW(parse("[\"a\": 5]")); }

}  // namespace test
}  // namespace json