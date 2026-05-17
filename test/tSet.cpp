#include <gtest/gtest.h>

#include <set>

#include "Set.hpp"
#include "test_utils.hpp"
using mystd::set::Set;
using std::set;

TEST(Set, Construct) {
  {
    Set<int> st;
    EXPECT_EQ(0, st.size());
    EXPECT_EQ(true, st.empty());
    EXPECT_EQ(st.begin(), st.end());
  }
  {
    Set<int, mystd::compare::Greater<int>> st;
    st.insert(10);
    st.insert(20);
    st.insert(5);
    auto it = st.begin();
    EXPECT_EQ(20, *it);
    EXPECT_EQ(10, *(++it));
  }
  {
    Set<int> st1;
    st1.insert(1);
    st1.insert(2);
    Set<int> st2(st1);
    EXPECT_EQ(2, st2.size());
    st1.insert(3);
    EXPECT_EQ(3, st1.size());
    EXPECT_EQ(2, st2.size());
  }
  {
    Set<int> st1;
    st1.insert(1);
    st1.insert(2);
    Set<int> st2(std::move(st1));
    EXPECT_EQ(2, st2.size());
    EXPECT_EQ(true, st1.empty());
  }
  {
    Set<int> st1, st2;
    st1.insert(10);
    st2 = st1;
    EXPECT_EQ(1, st2.size());
    st1.insert(20);
    st2 = st2;
    EXPECT_EQ(1, st2.size());
    EXPECT_EQ(2, st1.size());
  }
  {
    Set<int> st1, st2;
    st1.insert(10);
    st2.insert(5);
    st2 = std::move(st1);
    EXPECT_EQ(1, st2.size());
    EXPECT_EQ(10, *st2.begin());
    EXPECT_EQ(true, st1.empty());
  }
}

TEST(Set, Basic) {
  Set<int> st;
  auto ret1 = st.insert(10);
  EXPECT_EQ(true, ret1.second);
  EXPECT_EQ(10, *ret1.first);
  EXPECT_EQ(1, st.size());
  auto ret2 = st.insert(10);
  EXPECT_EQ(false, ret2.second);
  EXPECT_EQ(10, *ret2.first);
  EXPECT_EQ(1, st.size());
  st.insert(20);
  st.insert(30);
  EXPECT_EQ(3, st.size());
  std::vector<int> vec;
  for (auto x : st) vec.push_back(x);
  EXPECT_EQ(10, vec[0]);
  EXPECT_EQ(20, vec[1]);
  EXPECT_EQ(30, vec[2]);
  auto it = st.find(20);
  auto next = st.erase(it);
  EXPECT_EQ(30, *next);
  EXPECT_EQ(2, st.size());
  size_t cnt = st.erase(10);
  EXPECT_EQ(1, cnt);
  EXPECT_EQ(1, st.size());
  cnt = st.erase(999);
  EXPECT_EQ(0, cnt);
  EXPECT_EQ(1, st.size());
  st.clear();
  EXPECT_EQ(true, st.empty());
  Set<int> a, b;
  a.insert(1);
  b.insert(2);
  b.insert(3);
  a.swap(b);
  EXPECT_EQ(2, a.size());
  EXPECT_EQ(1, b.size());
  EXPECT_EQ(2, *a.begin());
}

TEST(Set, Iterator) {
  Set<int> st;
  st.insert(10);
  st.insert(20);
  st.insert(30);
  auto it = st.begin();
  EXPECT_EQ(10, *it);
  EXPECT_EQ(20, *(++it));
  EXPECT_EQ(30, *(++it));
  EXPECT_EQ(st.end(), ++it);
  auto end_it = st.end();
  EXPECT_EQ(30, *(--end_it));
  EXPECT_EQ(20, *(--end_it));
  EXPECT_EQ(10, *(--end_it));
  EXPECT_EQ(st.begin(), end_it);
  const Set<int>& cst = st;
  auto cit = cst.end();
  EXPECT_EQ(30, *(--cit));
}

namespace {
struct MoveOnly {
  int val;
  explicit MoveOnly(int v) : val(v) {}
  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
  MoveOnly(const MoveOnly&) = delete;
  bool operator<(const MoveOnly& other) const { return val < other.val; }
};
}  // namespace

TEST(Set, MoveOnly) {
  Set<MoveOnly> st;
  auto ret = st.insert(MoveOnly(10));
  EXPECT_EQ(true, ret.second);
  EXPECT_EQ(10, ret.first->val);
  ret = st.insert(MoveOnly(10));
  EXPECT_EQ(false, ret.second);
  EXPECT_EQ(10, ret.first->val);
  EXPECT_EQ(1, st.size());
  st.insert(MoveOnly(5));
  st.insert(MoveOnly(15));
  auto it = st.find(MoveOnly(10));
  auto next = st.erase(it);
  EXPECT_EQ(15, next->val);
  EXPECT_EQ(2, st.size());
  size_t cnt = st.erase(MoveOnly(5));
  EXPECT_EQ(1, cnt);
  EXPECT_EQ(1, st.size());
  EXPECT_EQ(15, st.begin()->val);
  Set<MoveOnly> st2;
  st2.insert(MoveOnly(99));
  st.swap(st2);
  EXPECT_EQ(99, st.begin()->val);
  EXPECT_EQ(15, st2.begin()->val);
}

TEST(Set, ConstCorrectness) {
  Set<int> st;
  st.insert(10);
  st.insert(20);
  st.insert(30);
  const Set<int>& cst = st;
  EXPECT_EQ(false, cst.empty());
  EXPECT_EQ(3, cst.size());
  auto cit = cst.begin();
  EXPECT_EQ(10, *cit);
  auto found = cst.find(20);
  EXPECT_EQ(false, found == cst.end());
  EXPECT_EQ(20, *found);
  auto not_found = cst.find(99);
  EXPECT_EQ(true, not_found == cst.end());
  auto lb = cst.lowerBound(20);
  EXPECT_EQ(20, *lb);
  auto ub = cst.upperBound(20);
  EXPECT_EQ(30, *ub);
  auto range = cst.equalRange(20);
  EXPECT_EQ(20, *range.first);
  EXPECT_EQ(30, *range.second);
}

TEST(Set, StringType) {
  Set<std::string> st;
  st.insert("banana");
  st.insert("apple");
  st.insert("cherry");
  EXPECT_EQ(3, st.size());
  auto it = st.begin();
  EXPECT_EQ("apple", *it);
  EXPECT_EQ("banana", *(++it));
  EXPECT_EQ("cherry", *(++it));
  st.erase("banana");
  EXPECT_EQ(2, st.size());
  EXPECT_EQ("cherry", *st.begin().operator++());
  std::string s = "date";
  st.insert(std::move(s));
  EXPECT_EQ(3, st.size());
  EXPECT_EQ(true, st.find("date") != st.end());
}

TEST(Set, Fuzzy) {
  RandomGenerator gen;
  const int query_times = 1000000;
  const int num_range = 1000;
  Set<int> st;
  set<int> ref;
  for (int t = 0; t < query_times; t++) {
    int opt = gen.uniform_int(0, 5);
    int rndint = gen.uniform_int(0, num_range);
    switch (opt) {
      case 0:
        EXPECT_EQ(ref.empty(), st.empty());
        break;
      case 1:
        EXPECT_EQ(ref.size(), st.size());
        break;
      case 2: {
        auto my_ans = st.insert(rndint);
        auto ref_ans = ref.insert(rndint);
        EXPECT_EQ(*ref_ans.first, *my_ans.first);
        break;
      }
      case 3: {
        auto my_ans = st.erase(rndint);
        auto ref_ans = ref.erase(rndint);
        EXPECT_EQ(ref_ans, my_ans);
        break;
      }
      case 4: {
        auto my_ans = (st.find(rndint) == st.end());
        auto ref_ans = (ref.find(rndint) == ref.end());
        EXPECT_EQ(ref_ans, my_ans);
        break;
      }
      case 5: {
        auto my_ans = st.equalRange(rndint);
        auto ref_ans = ref.equal_range(rndint);
        if (ref_ans.first == ref.end()) {
          EXPECT_EQ(true, (my_ans.first == st.end()));
        } else {
          EXPECT_EQ((*ref_ans.first), (*my_ans.first));
        }
        if (ref_ans.second == ref.end()) {
          EXPECT_EQ(true, (my_ans.second == st.end()));
        } else {
          EXPECT_EQ((*ref_ans.second), (*my_ans.second));
        }
        break;
      }
      default:
        break;
    }
  }
}
