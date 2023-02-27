#ifndef FLAT_MAP_HH
#define FLAT_MAP_HH

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iostream>
#include <iterator>

template <typename V, typename REF>
class forward_iterator {
  size_t idx_;
  REF* ref_;

 public:
  using value_type = V;
  using difference_type = size_t;
  using pointer = V*;
  using reference = V&;
  using iterator_category = std::forward_iterator_tag;

  [[nodiscard]] explicit forward_iterator(REF* ref, size_t idx) noexcept
      : idx_{idx}, ref_{ref} {}

  [[nodiscard]] const V& operator*() const noexcept {
    return ref_->value_at(idx_);
  }

  [[nodiscard]] V& operator*() noexcept { return ref_->value_at(idx_); }

  forward_iterator& operator++() {
    ++idx_;
    return *this;
  }

  auto operator<=>(const forward_iterator&) const = default;
};

template <typename K, typename V, size_t N>
class FlatMap {
 public:
  struct Entry {
    K key;
    V value;
  };

  using value_type = V;
  using const_iterator = forward_iterator<const Entry, const FlatMap>;
  using iterator = forward_iterator<Entry, FlatMap>;

  [[nodiscard]] constexpr FlatMap(
      const std::initializer_list<Entry>& init) noexcept {
    for (const auto& kv : init) {
      if (size_ != N) values_[size_++] = kv;
    }
  }

  [[nodiscard]] constexpr FlatMap(
      std::initializer_list<Entry>&& init) noexcept {
    for (const auto& kv : init) {
      if (size_ != N) values_[size_++] = std::move(kv);
    }
  }

  [[nodiscard]] constexpr const V& operator[](const K& key) const {
    return std::find_if(values_.cbegin(), values_.cend(),
                        [&](const auto& kv) { return kv.key == key; })
        ->value;
  }

  [[nodiscard]] constexpr V& operator[](const K& key) {
    return std::find_if(values_.begin(), values_.end(),
                        [&](const auto& kv) { return kv.key == key; })
        ->value;
  }

  [[nodiscard]] constexpr bool contains(const K& key) const {
    return std::find_if(values_.cbegin(), values_.cend(), [&](const auto& kv) {
             return kv.key == key;
           }) != values_.cend();
  }

  [[nodiscard]] iterator begin() { return iterator(this, 0); }
  [[nodiscard]] iterator end() { return iterator(this, size_); }

  [[nodiscard]] const_iterator begin() const { return const_iterator(this, 0); }
  [[nodiscard]] const_iterator end() const {
    return const_iterator(this, size_);
  }

 private:
  std::array<Entry, N> values_;
  size_t size_{};

  friend const_iterator;
  const Entry& value_at(size_t idx) const { return values_.at(idx); }
  friend iterator;
  Entry& value_at(size_t idx) { return values_.at(idx); }
};

#endif  // FLAT_MAP_HH
