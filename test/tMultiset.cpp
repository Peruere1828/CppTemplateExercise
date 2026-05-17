#include <gtest/gtest.h>

#include <set>
#include <vector>

#include "Compare.hpp"
#include "Set.hpp"
#include "test_utils.hpp"
using mystd::set::Multiset;
using std::multiset;

TEST(Multiset, Construct) {
  {
    Multiset<int> mst;
    EXPECT_EQ(0, mst.size());
    EXPECT_EQ(true, mst.empty());
    EXPECT_EQ(mst.begin(), mst.end());
  }
  {
    Multiset<int, mystd::compare::Greater<int>> mst;
    mst.insert(3);
    mst.insert(1);
    mst.insert(2);
    EXPECT_EQ(3, *mst.begin());
    EXPECT_EQ(2, *(++mst.begin()));
  }
  {
    Multiset<int> mst1;
    mst1.insert(1);
    mst1.insert(2);
    Multiset<int> mst2 = mst1;
    mst1.insert(3);
    EXPECT_EQ(3, mst1.size());
    EXPECT_EQ(2, mst2.size());
    EXPECT_EQ(mst2.end(), mst2.find(3));
  }
  {
    Multiset<int> mst1;
    mst1.insert(1);
    mst1.insert(2);
    Multiset<int> mst2 = std::move(mst1);
    EXPECT_EQ(2, mst2.size());
    EXPECT_EQ(true, mst1.empty());
  }
  {
    Multiset<int> mst1, mst2;
    mst1.insert(1);
    mst1.insert(2);
    mst2 = mst1;
    mst1.insert(3);
    EXPECT_EQ(3, mst1.size());
    EXPECT_EQ(2, mst2.size());
    EXPECT_EQ(mst2.end(), mst2.find(3));
  }
  {
    Multiset<int> mst1, mst2;
    mst1.insert(1);
    mst1.insert(2);
    mst2 = std::move(mst1);
    EXPECT_EQ(2, mst2.size());
    EXPECT_EQ(true, mst1.empty());
  }
}

TEST(Multiset, Iterator) {
  Multiset<int> mst;
  mst.insert(1);
  mst.insert(2);
  mst.insert(3);
  auto it = mst.end();
  --it;
  EXPECT_EQ(3, *it);
  --it;
  EXPECT_EQ(2, *it);
  --it;
  EXPECT_EQ(1, *it);
  EXPECT_EQ(it, mst.begin());
  const Multiset<int>& cmst = mst;
  auto cit = cmst.end();
  --cit;
  EXPECT_EQ(3, *cit);
}

TEST(Multiset, StringType) {
  Multiset<std::string> mst;
  mst.insert("apple");
  mst.insert("banana");
  mst.insert("apple");
  EXPECT_EQ(3, mst.size());
  EXPECT_EQ("apple", *mst.begin());
  EXPECT_EQ(2, mst.erase("apple"));
  EXPECT_EQ(1, mst.size());
  EXPECT_EQ("banana", *mst.begin());
}

TEST(Multiset, Duplicates) {
  Multiset<int> mst;
  const int count = 10;
  for (int i = 0; i < count; ++i) {
    mst.insert(100);
  }
  mst.insert(50);
  mst.insert(150);
  EXPECT_EQ(count + 2, mst.size());
  auto range = mst.equalRange(100);
  int range_count = 0;
  for (auto it = range.first; it != range.second; ++it) {
    EXPECT_EQ(100, *it);
    range_count++;
  }
  EXPECT_EQ(count, range_count);
  EXPECT_EQ(50, *(--range.first));
  EXPECT_EQ(150, *(range.second));
  auto erased_count = mst.erase(100);
  EXPECT_EQ(count, erased_count);
  EXPECT_EQ(2, mst.size());
  EXPECT_EQ(mst.end(), mst.find(100));
}

TEST(Multiset, EraseEdge) {
  Multiset<int> mst;
  mst.insert(10);
  mst.insert(20);
  mst.insert(30);
  auto it = mst.find(20);
  auto next = mst.erase(it);
  EXPECT_EQ(30, *next);
  EXPECT_EQ(2, mst.size());
  it = mst.find(30);
  next = mst.erase(it);
  EXPECT_EQ(mst.end(), next);
  EXPECT_EQ(1, mst.size());
  it = mst.begin();
  EXPECT_EQ(10, *it);
  next = mst.erase(it);
  EXPECT_EQ(mst.end(), next);
  EXPECT_EQ(true, mst.empty());
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
struct AbsCompare {
  bool operator()(int a, int b) const { return std::abs(a) < std::abs(b); }
};
}  // namespace

TEST(Multiset, MoveOnly) {
  Multiset<MoveOnly> mst;
  mst.insert(MoveOnly(10));
  mst.insert(MoveOnly(5));
  mst.insert(MoveOnly(10));
  EXPECT_EQ(3, mst.size());
  auto it = mst.begin();
  EXPECT_EQ(5, it->val);
  ++it;
  EXPECT_EQ(10, it->val);
  mst.erase(it);
  EXPECT_EQ(2, mst.size());
  EXPECT_EQ(5, mst.begin()->val);
}

TEST(Multiset, CustomCompare) {
  Multiset<int, AbsCompare> mst;
  mst.insert(-10);
  mst.insert(5);
  mst.insert(10);
  EXPECT_EQ(3, mst.size());
  EXPECT_EQ(5, *mst.begin());
  EXPECT_EQ(true, mst.find(10) != mst.end());
  EXPECT_EQ(true, mst.find(-10) != mst.end());
  auto range = mst.equalRange(10);
  int count = 0;
  for (auto it = range.first; it != range.second; ++it) {
    EXPECT_EQ(10, std::abs(*it));
    count++;
  }
  EXPECT_EQ(2, count);
}

TEST(Multiset, SelfAssign) {
  Multiset<int> mst;
  mst.insert(1);
  mst.insert(2);
  mst = mst;
  EXPECT_EQ(2, mst.size());
  EXPECT_EQ(1, *mst.begin());
  mst = std::move(mst);
  EXPECT_EQ(2, mst.size());
}

TEST(Multiset, ConstSupport) {
  Multiset<int> mst;
  mst.insert(10);
  mst.insert(20);
  mst.insert(20);
  mst.insert(30);
  const Multiset<int>& cmst = mst;
  EXPECT_EQ(4, cmst.size());
  EXPECT_EQ(false, cmst.empty());
  auto it = cmst.begin();
  EXPECT_EQ(10, *it);
  ++it;
  EXPECT_EQ(20, *it);
  auto fit = cmst.find(30);
  EXPECT_EQ(false, fit == cmst.end());
  EXPECT_EQ(30, *fit);
  auto fit_fail = cmst.find(999);
  EXPECT_EQ(true, fit_fail == cmst.end());
  auto range = cmst.equalRange(20);
  EXPECT_EQ(20, *range.first);
  EXPECT_EQ(30, *range.second);
  int count = 0;
  for (auto i = range.first; i != range.second; ++i) {
    count++;
  }
  EXPECT_EQ(2, count);
}

TEST(Multiset, Basics) {
  Multiset<int> a;
  a.insert(1);
  a.insert(2);
  a.insert(2);
  a.insert(3);
  EXPECT_EQ(4, a.size());
  std::vector<int> vals;
  for (auto it = a.begin(); it != a.end(); ++it) vals.push_back(*it);
  EXPECT_EQ(1, vals[0]);
  EXPECT_EQ(2, vals[1]);
  EXPECT_EQ(2, vals[2]);
  EXPECT_EQ(3, vals[3]);
  auto it2 = a.find(2);
  EXPECT_EQ(2, *it2);
  auto next_it = a.erase(it2);
  EXPECT_EQ(3, *next_it);
  EXPECT_EQ(3, a.size());
  a.insert(2);
  auto removed = a.erase(2);
  EXPECT_EQ(2, removed);
  EXPECT_EQ(2, a.size());
  a.insert(2);
  a.insert(2);
  auto lb = a.lowerBound(2);
  auto ub = a.upperBound(2);
  EXPECT_EQ(2, *lb);
  int count2 = 0;
  for (auto it = lb; it != ub; ++it) ++count2;
  EXPECT_EQ(2, count2);
  auto eq = a.equalRange(2);
  EXPECT_EQ(*eq.first, 2);
  auto f = a.find(999);
  EXPECT_EQ(a.end(), f);
  a.clear();
  EXPECT_EQ(true, a.empty());
  EXPECT_EQ(a.begin(), a.end());
  Multiset<int> b;
  b.insert(10);
  b.insert(20);
  a.insert(1);
  a.insert(2);
  a.swap(b);
  EXPECT_EQ(2, b.size());
  EXPECT_EQ(2, a.size());
  vals.clear();
  for (auto it = a.begin(); it != a.end(); ++it) vals.push_back(*it);
  EXPECT_EQ(10, vals[0]);
  EXPECT_EQ(20, vals[1]);
}

TEST(Multiset, Fuzzy) {
  RandomGenerator gen;
  const int query_times = 1000000;
  const int num_range = 100;
  Multiset<int> st;
  multiset<int> ref;
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
        EXPECT_EQ(*ref_ans, *my_ans);
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
