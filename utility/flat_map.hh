#ifndef FLAT_MAP_HH
#define FLAT_MAP_HH

#include <algorithm>
#include <array>
#include <initializer_list>
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

  [[nodiscard]] auto operator*() const noexcept -> const V& {
    return ref_->value_at(idx_);
  }

  [[nodiscard]] auto operator*() noexcept -> V& { return ref_->value_at(idx_); }

  auto operator++() -> forward_iterator& {
    ++idx_;
    return *this;
  }

  auto operator<=>(const forward_iterator&) const noexcept = default;
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

  [[nodiscard]] constexpr auto operator[](const K& key) const -> const V& {
    return std::find_if(values_.cbegin(), values_.cend(),
                        [&](const auto& kv) { return kv.key == key; })
        ->value;
  }

  [[nodiscard]] constexpr auto operator[](const K& key) -> V& {
    return std::find_if(values_.begin(), values_.end(),
                        [&](const auto& kv) { return kv.key == key; })
        ->value;
  }

  [[nodiscard]] constexpr auto contains(const K& key) const -> bool {
    return std::find_if(values_.cbegin(), values_.cend(), [&](const auto& kv) {
             return kv.key == key;
           }) != values_.cend();
  }

  [[nodiscard]] auto begin() -> iterator { return iterator(this, 0); }
  [[nodiscard]] auto end() -> iterator { return iterator(this, size_); }

  [[nodiscard]] auto begin() const -> const_iterator {
    return const_iterator(this, 0);
  }
  [[nodiscard]] auto end() const -> const_iterator {
    return const_iterator(this, size_);
  }

 private:
  std::array<Entry, N> values_;
  size_t size_{};

  friend const_iterator;
  auto value_at(size_t idx) const -> const Entry& { return values_.at(idx); }
  friend iterator;
  auto value_at(size_t idx) -> Entry& { return values_.at(idx); }
};

#endif  // FLAT_MAP_HH
