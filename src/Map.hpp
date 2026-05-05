#ifndef MAP_HPP
#define MAP_HPP

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "Compare.hpp"
#include "RBTree.hpp"

namespace mystd::map {
template <typename Key, typename T,
          typename Compare = mystd::compare::Less<Key>>
class Map {
public:
  using KeyType = Key;
  using MappedType = T;
  using ValueType = std::pair<const Key, T>;

private:
  struct SelectFirst {
    auto operator()(const ValueType& val) const -> const Key& {
      return val.first;
    }
  };
  using RBTree = mystd::rbtree::RBTree<ValueType, Key, SelectFirst, Compare>;
  RBTree tree_;

public:
  // 放弃实现精准语义的Iterator和ConstIterator
  using Iterator = typename RBTree::ConstIterator;
  using ConstIterator = typename RBTree::ConstIterator;
  Map() = default;
  explicit Map(Compare comp) : tree_(comp) {}
  Map(const Map& other) : tree_(other.tree_) {}
  Map(Map&& other) noexcept : tree_(std::move(other.tree_)) {}
  auto operator=(const Map& other) -> Map& {
    tree_ = other.tree_;
    return *this;
  }
  auto operator=(Map&& other) noexcept -> Map& {
    tree_ = std::move(other.tree_);
    return *this;
  }
  ~Map() = default;

  // clang-format off
  auto begin() noexcept -> Iterator { return Iterator(tree_.begin()); }
  auto begin() const noexcept -> ConstIterator { return ConstIterator(tree_.begin()); }
  auto end() noexcept -> Iterator { return Iterator(tree_.end()); }
  auto end() const noexcept -> ConstIterator { return ConstIterator(tree_.end()); }
  [[nodiscard]] auto size() const noexcept -> size_t { return tree_.size(); }
  [[nodiscard]] auto empty() const noexcept -> bool { return tree_.empty(); }
  // clang-format on

  auto operator[](const Key& key) -> T& {
    auto tmp = tree_.insertUnique(ValueType(key, T()));
    return tmp.first->second;
  }
  auto at(const Key& key) -> T& {
    auto iter = tree_.find(key);
    if (iter == tree_.end()) {
      throw std::out_of_range("mystd::map::at: key not found");
    }
    return iter->second;
  }
  auto at(const Key& key) const -> const T& {
    auto iter = tree_.find(key);
    if (iter == tree_.end()) {
      throw std::out_of_range("mystd::map::at: key not found");
    }
    return iter->second;
  }

  auto insert(const ValueType& val) -> std::pair<Iterator, bool> {
    auto tmp = tree_.insertUnique(val);
    return {Iterator(tmp.first), tmp.second};
  }
  auto insert(ValueType&& val) -> std::pair<Iterator, bool> {
    auto tmp = tree_.insertUnique(std::move(val));
    return {Iterator(tmp.first), tmp.second};
  }

  auto erase(Iterator iter) {
    typename RBTree::Iterator tmp(iter.base());
    typename RBTree::Iterator res = tree_.erase(tmp);
    return Iterator(res);
  }
  auto erase(const Key& val) { return tree_.eraseUnique(val); }

  // clang-format off
  auto find(const Key& key) { return Iterator(tree_.find(key)); }
  auto find(const Key& key) const { return ConstIterator(tree_.find(key)); }
  auto lowerBound(const Key& key) { return Iterator(tree_.lowerBound(key)); }
  auto lowerBound(const Key& key) const { return ConstIterator(tree_.lowerBound(key)); }
  auto upperBound(const Key& key) { return Iterator(tree_.upperBound(key)); }
  auto upperBound(const Key& key) const { return ConstIterator(tree_.upperBound(key)); }
  auto equalRange(const Key& key) -> std::pair<Iterator, Iterator> {
    auto tmp = tree_.equalRange(key);
    return {Iterator(tmp.first), Iterator(tmp.second)};
  }
  auto equalRange(const Key& key) const -> std::pair<ConstIterator, ConstIterator> {
    auto tmp = tree_.equalRange(key);
    return {ConstIterator(tmp.first), ConstIterator(tmp.second)};
  }
  auto count(const Key& key) const -> size_t { return find(key) != end(); }
  // clang-format on

  void clear() { tree_.clear(); }
  void swap(Map& other) noexcept { tree_.swap(other.tree_); }
};
}  // namespace mystd::map

#endif