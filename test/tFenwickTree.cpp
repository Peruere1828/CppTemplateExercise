#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "FenwickTree.hpp"
#include "common.h"
#include "test_utils.hpp"

namespace TestFenwickTree {
const int mod = 1e9 + 7;
auto e = []() { return 0; };
auto add = [](int x, int y) { return (x + y) % mod; };
auto sub = [](int x, int y) { return ((x - y) % mod + mod) % mod; };
}  // namespace TestFenwickTree
using namespace TestFenwickTree;

using namespace mystd::fenwick_tree;

TEST(FenwickTree, Default) {
  RandomGenerator gen;
  std::vector<int> arr;
  const size_t arr_len = 10000;
  const size_t query_times = 100000;
  const int num_range = 100000;
  for (size_t i = 0; i < arr_len; i++) {
    arr.push_back(gen.uniform_int(0, num_range));
  }
  FenwickTree<int, e, add, sub> tree(arr);
  std::vector<int> ref = arr;
  EXPECT_THROW(tree.query(static_cast<size_t>(100), static_cast<size_t>(1)),
               std::out_of_range);
  EXPECT_THROW(tree.query(static_cast<ptrdiff_t>(-2)), std::out_of_range);
  EXPECT_THROW(tree.apply(static_cast<size_t>(arr_len), 100),
               std::out_of_range);
  for (size_t q = 0; q < query_times; q++) {
    int opt = gen.uniform_int(0, 2);
    size_t l = static_cast<size_t>(gen.uniform_int(0, static_cast<int>(arr_len) - 1));
    size_t r = static_cast<size_t>(gen.uniform_int(0, static_cast<int>(arr_len) - 1));
    if (l > r) mystd::swap(l, r);
    switch (opt) {
      case 0: {
        int val = gen.uniform_int(0, num_range);
        tree.apply(l, val);
        ref[l] = add(ref[l], val);
        break;
      }
      case 1: {
        int ans1 = tree.query(static_cast<ptrdiff_t>(l));
        int ans2 = e();
        for (size_t i = 0; i <= l; i++) {
          ans2 = add(ans2, ref[i]);
        }
        EXPECT_EQ(ans2, ans1);
        break;
      }
      case 2: {
        int ans1 = tree.query(l, r);
        int ans2 = e();
        for (size_t i = l; i <= r; i++) {
          ans2 = add(ans2, ref[i]);
        }
        EXPECT_EQ(ans2, ans1);
        break;
      }
      default:
        break;
    }
  }
}
