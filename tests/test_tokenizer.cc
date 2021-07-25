#include <sstream>
#include <string>
#include <string_view>

#include "testrunner/testrunner.h"
#include "tokenizer.h"

namespace {

std::vector<json::Token> parse(std::string_view json) {
  try {
    return json::Tokenizer::parse(json);

  } catch (json::ValidationError error) {
    std::ostringstream error_str;
    error_str << error.message << " at offset "
              << static_cast<int>(error.offset);
    throw std::string{error_str.str()};
  }
}

void ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER(std::string number_str) {
  auto tokens = parse(number_str);
  EXPECT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type(), json::Token::NUMBER);
  EXPECT_EQ(tokens[0].asNumber(), std::stof(number_str));
}

}  // namespace

namespace json {
namespace test {

TEST(TokenizerAcceptsEmptyString) {
  auto tokens = parse("");
  ASSERT_TRUE(tokens.empty());
}

TEST(TokenizerAcceptsWhitespace) {
  auto tokens = parse("    \n  \t\r  ");
  ASSERT_TRUE(tokens.empty());
}

TEST(TokenizerRejectsNonStringToken) { EXPECT_THROW(Tokenizer::parse("XXX")); }

TEST(TokenizerAcceptsSingleNull) {
  auto tokens = parse("null");
  EXPECT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type(), Token::NULLOPT);
}

TEST(TokeinzerParsesBraces) {
  auto tokens = parse("{}");
  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].type(), Token::OBJECT_BEGIN);
  EXPECT_EQ(tokens[1].type(), Token::OBJECT_END);
}

TEST(TokeinzerParsesArrayBrackets) {
  auto tokens = parse(" {[]} ");
  EXPECT_EQ(tokens.size(), 4);
  EXPECT_EQ(tokens[1].type(), Token::ARRAY_BEGIN);
  EXPECT_EQ(tokens[2].type(), Token::ARRAY_END);
}

TEST(TokenizerParsesPunctuation) {
  auto tokens = parse(" ,: ");
  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0].type(), Token::COMMA);
  EXPECT_EQ(tokens[1].type(), Token::COLON);
}

TEST(TokenizerAcceptsSingleBoolean) {
  auto t_true = parse("true");
  EXPECT_EQ(t_true[0].type(), Token::BOOLEAN);
  EXPECT_EQ(t_true[0].asBool(), true);

  auto t_false = parse("false");
  EXPECT_EQ(t_false[0].type(), Token::BOOLEAN);
  EXPECT_EQ(t_false[0].asBool(), false);
}

TEST(TokenizerRejectsInvalidBoolean) {
  EXPECT_THROW(Tokenizer::parse("tru"));
  EXPECT_THROW(Tokenizer::parse("TRUE"));
}

TEST(TokenizerAcceptsSingleNumber) {
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("0");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("123");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("-4");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("1.1");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("-1.23");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("3.54672E-9");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("1e3");
  ASSERT_VALID_AND_FIRST_TOKEN_IS_NUMBER("1e3");
}

TEST(TokenizerRejectsInvalidNumber) {
  EXPECT_THROW(Tokenizer::parse("+1"));
  EXPECT_THROW(Tokenizer::parse("-"));
  EXPECT_THROW(Tokenizer::parse(".123"));
  EXPECT_THROW(Tokenizer::parse("-."));
  EXPECT_THROW(Tokenizer::parse("-.123"));
  EXPECT_THROW(Tokenizer::parse("1.2.3"));
  EXPECT_THROW(Tokenizer::parse("-4.5.6"));
  EXPECT_THROW(Tokenizer::parse("e4"));
  EXPECT_THROW(Tokenizer::parse("1-3"));
}

TEST(TokenizerAcceptsSingleEmptyString) {
  auto t_str = parse("\"\"");
  EXPECT_EQ(t_str[0].type(), Token::STRING);
  ASSERT_TRUE(t_str[0].asString().empty());
}

TEST(TokenizerAcceptsSingleString) {
  auto t_str = parse("\"X\"");
  EXPECT_EQ(t_str[0].type(), Token::STRING);
  EXPECT_EQ(t_str[0].asString(), "X");

  auto t_hello = parse("\"Hello World!\"");
  EXPECT_EQ(t_hello[0].type(), Token::STRING);
  EXPECT_EQ(t_hello[0].asString(), "Hello World!");

  auto t_multi_line = parse("\"Multiple\nLines\nare\nfun!\"");
  EXPECT_EQ(t_multi_line[0].type(), Token::STRING);
  EXPECT_EQ(t_multi_line[0].asString(), "Multiple\nLines\nare\nfun!");
}

TEST(TokenizerRejectsInvalidString) {
  EXPECT_THROW(Tokenizer::parse("\"Hello World!"));
  EXPECT_THROW(Tokenizer::parse("\"\"\""));
}

TEST(TokenizerParsesMultilineJson) {
  auto tokens = parse("{\n  \"a\" : \"b\",\n  \"c\" : \"d\",\n  \"n\" : 5\n}");

  std::vector<Token::Type> expected_tokens{
      Token::OBJECT_BEGIN, Token::STRING, Token::COLON, Token::STRING,
      Token::COMMA,        Token::STRING, Token::COLON, Token::STRING,
      Token::COMMA,        Token::STRING, Token::COLON, Token::NUMBER,
      Token::OBJECT_END};

  EXPECT_EQ(tokens.size(), expected_tokens.size());

  auto mismatch = std::mismatch(
      tokens.begin(), tokens.end(), expected_tokens.begin(),
      [](const Token& a, const Token::Type& b) { return a.type() == b; });
  EXPECT_EQ(mismatch.first, tokens.end());
}

}  // namespace test
}  // namespace json
