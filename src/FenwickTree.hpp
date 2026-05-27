#ifndef FENWICKTREE_HPP
#define FENWICKTREE_HPP

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "BitOperation.hpp"

namespace mystd::fenwick_tree {

// op: T × T -> T，满足结合律，e 是单位元
// opInv: 两段前缀和做差
template <typename T, T (*e)(), T (*op)(T, T), T (*opInv)(T, T)>
class FenwickTree {
private:
  size_t arr_size_;
  std::vector<T> tree_;

  static auto lowbit(size_t n) -> size_t { return n & (-n); }

  // 内部查询：ind 为平移后的 1-indexed 位置（0 返回单位元）
  auto queryInternal(size_t ind) const -> T {
    T result = e();
    for (; ind > 0; ind -= lowbit(ind)) {
      result = op(result, tree_[ind]);
    }
    return result;
  }

public:
  explicit FenwickTree(size_t arr_size)
      : FenwickTree(std::vector<T>(arr_size, e())) {}
  // 数组下标从0开始，但是平移到1开始
  explicit FenwickTree(const std::vector<T> &arr)
      : arr_size_(arr.size()), tree_(arr_size_ + 1, e()) {
    for (size_t i = 1; i <= arr_size_; i++) {
      tree_[i] = op(tree_[i], arr[i - 1]);
      size_t j = i + lowbit(i);
      if (j <= arr_size_) {
        tree_[j] = op(tree_[j], tree_[i]);
      }
    }
  }

  void apply(size_t ind, T val) {
    if (ind >= arr_size_) {
      throw std::out_of_range("FenwickTree::apply invalid index");
    }
    for (ind++; ind <= arr_size_; ind += lowbit(ind)) {
      tree_[ind] = op(tree_[ind], val);
    }
  }
  // 此处求和得到的是前缀和
  auto query(ptrdiff_t ind) const -> T {
    if (ind < -1 || static_cast<size_t>(ind) >= arr_size_) {
      throw std::out_of_range("FenwickTree::query invalid index");
    }
    return queryInternal(static_cast<size_t>(ind + 1));
  }

  auto query(size_t left_bound, size_t right_bound) const -> T {
    if (left_bound > right_bound || right_bound >= arr_size_) {
      throw std::out_of_range("FenwickTree::query invalid interval");
    }
    return opInv(queryInternal(right_bound + 1),
                 queryInternal(left_bound));
  }
};
}  // namespace mystd::fenwick_tree

#endif  // FENWICKTREE_HPP