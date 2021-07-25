#ifndef CONSUMABLE_H
#define CONSUMABLE_H

template <typename COLLECTION, typename ITEM>
class Consumable {
 public:
  explicit Consumable(const COLLECTION& collection)
      : consumed_{}, collection_{collection} {}

  constexpr void reset() { consumed_ = 0; }

  [[nodiscard]] constexpr bool eol() const {
    return consumed_ >= collection_.size();
  }

  [[nodiscard]] constexpr size_t size() const { return collection_.size(); }

  [[nodiscard]] constexpr const COLLECTION& data() const { return collection_; }

  [[nodiscard]] constexpr size_t idx() const { return consumed_; }

  [[nodiscard]] constexpr bool canPeek() const {
    return (consumed_ < collection_.size() - 1);
  }

  [[nodiscard]] constexpr const ITEM& peek() const {
    return collection_.at(consumed_ + 1);
  }

  [[nodiscard]] constexpr const ITEM& prev() const {
    return collection_.at(consumed_ - 1);
  }

  [[nodiscard]] constexpr const ITEM& current() const {
    return collection_.at(consumed_);
  }

  constexpr void skip(size_t items) { consumed_ += items; }

  [[nodiscard]] constexpr const ITEM& consume() {
    return collection_.at(consumed_++);
  }

  constexpr bool consume_if(const ITEM& compare) {
    if (current() == compare) {
      ++consumed_;
      return true;
    }
    return false;
  }

 private:
  size_t consumed_;
  COLLECTION collection_;
};

#endif