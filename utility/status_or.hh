#ifndef STATUS_OR_HH
#define STATUS_OR_HH

#include <utility>

template <typename StatusType, typename ValueType>
class StatusOrBase {
  bool has_value_;

  union alignas(alignof(ValueType)) {
    StatusType status_;
    ValueType data_;
  };

 public:
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] StatusOrBase(StatusType status)
      : has_value_{false}, status_{status} {}

  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] StatusOrBase(const ValueType& value)
      : has_value_{true}, data_{value} {}

  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] StatusOrBase(ValueType&& value)
      : has_value_{true}, data_{std::move(value)} {}

  ~StatusOrBase() {
    if (has_value_) data_.~ValueType();
  }

  [[nodiscard]] explicit operator bool() const { return has_value_; }

  [[nodiscard]] auto status() const -> StatusType { return status_; }

  [[nodiscard]] auto operator*() -> ValueType& { return data_; }

  [[nodiscard]] auto operator->() -> ValueType* { return &data_; }
};

#endif  // STATUS_OR_HH
