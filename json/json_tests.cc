#include <variant>

#include "json/json.hh"
#include "json/value.hh"
#include "testrunner/testrunner.h"

TEST(Json_ParsesConstantValues) {
  auto json_null = json::Json::parse(" null ");
  ASSERT_TRUE(json_null);
  ASSERT_TRUE(std::holds_alternative<json::Null>(json_null->value()));

  auto json_true = json::Json::parse("   true");
  ASSERT_TRUE(json_true);
  ASSERT_TRUE(std::holds_alternative<json::Boolean>(json_true->value()));
  ASSERT_TRUE(json_true->begin().boolean());

  auto json_false = json::Json::parse("false   ");
  ASSERT_TRUE(json_false);
  ASSERT_TRUE(std::holds_alternative<json::Boolean>(json_false->value()));
  ASSERT_FALSE(json_false->begin().boolean());
}

TEST(Json_ParsesEmptyValues) {
  ASSERT_TRUE(json::Json::parse(""));

  auto empty_string = json::Json::parse(R"("")");
  ASSERT_TRUE(empty_string);
  ASSERT_TRUE(std::holds_alternative<json::String>(empty_string->value()));

  auto empty_array = json::Json::parse("[]");
  ASSERT_TRUE(empty_array);
  ASSERT_TRUE(std::holds_alternative<json::Array>(empty_array->value()));

  auto empty_object = json::Json::parse("{}");
  ASSERT_TRUE(empty_object);
  ASSERT_TRUE(std::holds_alternative<json::Object>(empty_object->value()));
}

TEST(Json_ParsesSimpleObject) {
  auto simple = json::Json::parse(R"({"Hello": "Object"})");
  ASSERT_TRUE(simple);
  ASSERT_TRUE(std::holds_alternative<json::Object>(simple->value()));
  ASSERT_TRUE(std::holds_alternative<json::String>((*simple)["Hello"].value()));
  EXPECT_EQ((*simple)["Hello"].string(), "Object");
}