#ifndef SEGMENTTREE_HPP
#define SEGMENTTREE_HPP

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "BitOperation.hpp"

namespace mystd::segment_tree {

template <typename T, T (*op)(T, T), T (*e)(), typename F, T (*mapping)(F, T),
          F (*composition)(F, F), F (*id)()>
class SegmentTree {
private:
  size_t arr_size_, tree_size_, height_log_;
  // 为了方便在算法竞赛环境中使用，使用 std::vector 而不是 mystd::Vector
  std::vector<T> tree_;
  std::vector<F> lazy_;
  void pushUp(size_t ind) {
    tree_[ind] = op(tree_[ind * 2], tree_[ind * 2 + 1]);
  }
  void applyNode(size_t ind, F func) {
    tree_[ind] = mapping(func, tree_[ind]);
    if (ind < tree_size_) {
      lazy_[ind] = composition(func, lazy_[ind]);
    }
  }
  void pushDown(size_t ind) {
    applyNode(ind * 2, lazy_[ind]);
    applyNode(ind * 2 + 1, lazy_[ind]);
    lazy_[ind] = id();
  }

public:
  explicit SegmentTree(size_t arr_size)
      : SegmentTree(std::vector<T>(arr_size, e())) {}
  // 数组下标从 0 开始
  explicit SegmentTree(const std::vector<T> &arr)
      : arr_size_(arr.size()),
        tree_size_(bitop::bitCeil(arr_size_)),
        height_log_(bitop::countrZero(tree_size_)) {
    tree_.resize(tree_size_ * 2, e());
    lazy_.resize(tree_size_, id());
    for (size_t i = 0; i < arr_size_; i++) {
      tree_[tree_size_ + i] = arr[i];
    }
    for (size_t i = tree_size_ - 1; i > 0; i--) {
      pushUp(i);
    }
  }
  void assign(size_t ind, T val) {
    if (ind >= arr_size_) {
      throw std::out_of_range("SegmentTree::assign index out of range");
    }
    ind += tree_size_;
    for (size_t i = height_log_; i >= 1; i--) {
      pushDown(ind >> i);
    }
    tree_[ind] = val;
    for (size_t i = 1; i <= height_log_; i++) {
      pushUp(ind >> i);
    }
  }
  void apply(size_t ind, F func) {
    if (ind >= arr_size_) {
      throw std::out_of_range("SegmentTree::apply index out of range");
    }
    ind += tree_size_;
    for (size_t i = height_log_; i >= 1; i--) {
      pushDown(ind >> i);
    }
    tree_[ind] = mapping(func, tree_[ind]);
    for (size_t i = 1; i <= height_log_; i++) {
      pushUp(ind >> i);
    }
  }
  // 区间为闭区间 [L, R]，但内部实现的时候使用半开区间 [L, R+1)
  void apply(size_t left_bound, size_t right_bound, F func) {
    if (left_bound > right_bound || right_bound >= arr_size_) {
      throw std::out_of_range("SegmentTree::apply invalid interval");
    }
    left_bound += tree_size_;
    right_bound += tree_size_ + 1;
    for (size_t i = height_log_; i >= 1; i--) {
      if (((left_bound >> i) << i) != left_bound) {
        pushDown(left_bound >> i);
      }
      if (((right_bound >> i) << i) != right_bound) {
        pushDown((right_bound - 1) >> i);
      }
    }
    size_t orig_left = left_bound;
    size_t orig_right = right_bound;
    while (left_bound < right_bound) {
      if ((left_bound & 1) != 0) {
        applyNode(left_bound++, func);
      }
      if ((right_bound & 1) != 0) {
        applyNode(--right_bound, func);
      }
      left_bound >>= 1;
      right_bound >>= 1;
    }
    for (size_t i = 1; i <= height_log_; i++) {
      if (((orig_left >> i) << i) != orig_left) {
        pushUp(orig_left >> i);
      }
      if (((orig_right >> i) << i) != orig_right) {
        pushUp((orig_right - 1) >> i);
      }
    }
  }
  auto query(size_t ind) -> T {
    if (ind >= arr_size_) {
      throw std::out_of_range("SegmentTree::query index out of range");
    }
    ind += tree_size_;
    for (size_t i = height_log_; i >= 1; i--) {
      pushDown(ind >> i);
    }
    return tree_[ind];
  }
  // 区间为闭区间 [L, R]，但内部实现的时候使用半开区间 [L, R+1)
  auto query(size_t left_bound, size_t right_bound) -> T {
    if (left_bound > right_bound || right_bound >= arr_size_) {
      throw std::out_of_range("SegmentTree::query invalid interval");
    }
    left_bound += tree_size_;
    right_bound += tree_size_ + 1;
    for (size_t i = height_log_; i >= 1; i--) {
      if (((left_bound >> i) << i) != left_bound) {
        pushDown(left_bound >> i);
      }
      if (((right_bound >> i) << i) != right_bound) {
        pushDown((right_bound - 1) >> i);
      }
    }
    T lans = e();
    T rans = e();
    while (left_bound < right_bound) {
      if ((left_bound & 1) != 0) {
        lans = op(lans, tree_[left_bound++]);
      }
      if ((right_bound & 1) != 0) {
        rans = op(tree_[--right_bound], rans);
      }
      left_bound >>= 1;
      right_bound >>= 1;
    }
    return op(lans, rans);
  }
};
}  // namespace mystd::segment_tree

#endif  // SEGMENTTREE_HPP