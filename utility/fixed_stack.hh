#ifndef FIXED_STACK_HH
#define FIXED_STACK_HH

#include <array>
#include <cstddef>
#include <initializer_list>

template <typename T, size_t N>
class FixedStack {
  std::array<T, N> data_;
  size_t size_{};

 public:
  FixedStack() = default;

  FixedStack(const std::initializer_list<T>& init) {
    for (const auto& value : init) push(value);
  }

  [[nodiscard]] auto size() const -> size_t { return size_; }

  [[nodiscard]] auto empty() const -> bool { return size_ == 0; }

  void push(T value) { data_[size_++] = value; }

  auto pop() -> T { return data_[--size_]; }
};

#endif  // FIXED_STACK_HH