#ifndef PARSER_HH
#define PARSER_HH

#include <charconv>
#include <stack>
#include <string_view>
#include <vector>

#include "json/node.hh"
#include "json/status.hh"
#include "json/token.hh"
#include "json/tokenizer.hh"

namespace json::internal {

class Parser {
  enum class State {
    ExpectValue,
    ExpectOptionalValue,
    ExpectEndOfObject,
    ExpectValueName,
    ExpectOptionalValueName,
    ExpectOptionalCommaInObject,
    ExpectColon,
    ExpectEndOfArray
  };

  std::vector<Node> nodes_;
  std::stack<size_t> parents_{{Root}};
  std::stack<State> states_{{State::ExpectValue}};
  std::string pending_value_name;

 public:
  [[nodiscard]] auto parse(std::string_view json_source) -> Status {
    internal::Parser parser;

    while (!json_source.empty()) {
      auto maybe_token = json::internal::Tokenizer::parse(json_source);
      if (maybe_token) {
        auto status = parseToken(*maybe_token);
        if (status != Status::Ok) return status;

      } else if (maybe_token.status() != Status::Ok) {
        return maybe_token.status();
      }
    }
    return Status::Ok;
  }

  [[nodiscard]] auto nodes() const -> std::vector<Node> { return nodes_; }

 private:
  [[nodiscard]] auto parseToken(const Token& token) -> Status {
    auto state = states_.top();
    states_.pop();

    bool optional = true;
    switch (state) {
      case State::ExpectOptionalValueName:
        if (token.type == Token::Type::String) return rememberValueName(token);
        break;

      case State::ExpectOptionalValue:
        if (token.startsAValue()) return startNewValue(token);
        break;

      case State::ExpectOptionalCommaInObject:
        if (token.type == Token::Type::Comma) return continueObject();
        break;

      default:
        optional = false;
        break;
    }

    if (optional) {
      state = states_.top();
      states_.pop();
    }

    switch (state) {
      case State::ExpectValue:
        return startNewValue(token);

      case State::ExpectValueName:
        return rememberValueName(token);

      case State::ExpectEndOfObject:
        return finalizeObject(token);

      case State::ExpectEndOfArray:
        if (token.type == Token::Type::Comma) return continueArray();
        return finalizeArray(token);

      case State::ExpectColon:
        if (token.type != Token::Type::Colon) return Status::UnexpectedToken;
        return Status::Ok;

      default:
        break;
    }

    return Status::Unimplemented;
  }

  [[nodiscard]] auto startNewValue(const Token& token) -> Status {
    if (!token.startsAValue()) return Status::UnexpectedToken;

    switch (token.type) {
      case Token::Type::LeftCurlyBracket:
        startObject();
        break;

      case Token::Type::LeftSquareBracket:
        startArray();
        break;

      case Token::Type::String:
        nodes_.emplace_back(
            Node{String{std::string{token.value}}, parents_.top()});
        break;

      case Token::Type::Number: {
        float value{};
        std::from_chars(token.value.begin(), token.value.end(), value);
        nodes_.emplace_back(Node{Number{value}, parents_.top()});
        break;
      }

      case Token::Type::True:
      case Token::Type::False: {
        const auto value = token.type == Token::Type::True;
        nodes_.emplace_back(Node{Boolean{value}, parents_.top()});
        break;
      }

      case Token::Type::Null:
        nodes_.emplace_back(Node{Null{}, parents_.top()});
        break;

      default:
        return Status::Unimplemented;
    }

    auto* node = &nodes_.back();
    if (!pending_value_name.empty())
      node->name = std::exchange(pending_value_name, "");

    while (node->parent != Root) {
      ++nodes_[node->parent].children;
      node = &nodes_[node->parent];
    }

    return Status::Ok;
  }

  void startObject() {
    auto parent = parents_.top();
    parents_.push(nodes_.size());
    nodes_.emplace_back(Node{Object{}, parent});
    states_.push(State::ExpectEndOfObject);
    states_.push(State::ExpectOptionalValueName);
  }

  [[nodiscard]] auto rememberValueName(const Token& token) -> Status {
    if (token.type != Token::Type::String) return Status::UnexpectedToken;
    if (!pending_value_name.empty()) return Status::UnexpectedToken;
    pending_value_name = std::string{token.value};
    states_.push(State::ExpectOptionalCommaInObject);
    states_.push(State::ExpectValue);
    states_.push(State::ExpectColon);
    return Status::Ok;
  }

  [[nodiscard]] auto continueObject() -> Status {
    states_.push(State::ExpectValueName);
    return Status::Ok;
  }

  [[nodiscard]] auto finalizeObject(const Token& token) -> Status {
    if (token.type != Token::Type::RightCurlyBracket)
      return Status::UnexpectedToken;
    parents_.pop();
    return Status::Ok;
  }

  void startArray() {
    auto parent = parents_.top();
    parents_.push(nodes_.size());
    nodes_.emplace_back(Node{Array{}, parent});
    states_.push(State::ExpectEndOfArray);
    states_.push(State::ExpectOptionalValue);
  }

  [[nodiscard]] auto continueArray() -> Status {
    // TODO(ae): Need to check that we didn't start with a comma (i.e. [,true])
    states_.push(State::ExpectEndOfArray);
    states_.push(State::ExpectValue);
    return Status::Ok;
  }

  [[nodiscard]] auto finalizeArray(const Token& token) -> Status {
    if (token.type != Token::Type::RightSquareBracket)
      return Status::UnexpectedToken;
    parents_.pop();
    return Status::Ok;
  }
};

}  // namespace json::internal

#endif  // PARSER_HH