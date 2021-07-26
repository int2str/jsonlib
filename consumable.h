#ifndef CONSUMABLE_H
#define CONSUMABLE_H

template <typename COLLECTION, typename ITEM>
class Consumable {
 public:
  explicit Consumable(const COLLECTION& collection) : collection_{collection} {}

  constexpr void reset() { consumed_ = 0; }

  [[nodiscard]] bool eol() const { return consumed_ >= collection_.size(); }

  [[nodiscard]] size_t size() const { return collection_.size(); }

  [[nodiscard]] const COLLECTION& data() const { return collection_; }

  [[nodiscard]] size_t idx() const { return consumed_; }

  [[nodiscard]] bool canPeek() const {
    return (consumed_ < collection_.size() - 1);
  }

  [[nodiscard]] const ITEM& peek() const {
    return collection_.at(consumed_ + 1);
  }

  [[nodiscard]] const ITEM& prev() const {
    return collection_.at(consumed_ - 1);
  }

  [[nodiscard]] const ITEM& current() const {
    return collection_.at(consumed_);
  }

  constexpr void skip(size_t items) { consumed_ += items; }

  [[nodiscard]] const ITEM& consume() { return collection_.at(consumed_++); }

  bool consume_if(const ITEM& compare) {
    bool consume_item = current() == compare;
    if (consume_item) ++consumed_;
    return consume_item;
  }

 private:
  size_t consumed_{0};
  COLLECTION collection_;
};

#endif