#include <gtest/gtest.h>

#include <map>
#include <string>

#include "Map.hpp"
#include "test_utils.hpp"
using mystd::map::Map;
using std::map;

TEST(Map, Construct) {
  {
    Map<int, int> mp;
    EXPECT_EQ(0, mp.size());
    EXPECT_EQ(true, mp.empty());
    EXPECT_EQ(mp.begin(), mp.end());
  }
  {
    Map<int, int, mystd::compare::Greater<int>> mp;
    mp[10] = 100;
    mp[20] = 200;
    mp[5] = 50;
    auto it = mp.begin();
    EXPECT_EQ(20, it->first);
    EXPECT_EQ(200, it->second);
    EXPECT_EQ(10, (++it)->first);
  }
  {
    Map<int, int> mp1;
    mp1[1] = 10;
    mp1[2] = 20;
    Map<int, int> mp2(mp1);
    EXPECT_EQ(2, mp2.size());
    mp1[3] = 30;
    EXPECT_EQ(3, mp1.size());
    EXPECT_EQ(2, mp2.size());
  }
  {
    Map<int, int> mp1;
    mp1[1] = 10;
    mp1[2] = 20;
    Map<int, int> mp2(std::move(mp1));
    EXPECT_EQ(2, mp2.size());
    EXPECT_EQ(true, mp1.empty());
  }
  {
    Map<int, int> mp1, mp2;
    mp1[10] = 100;
    mp2 = mp1;
    EXPECT_EQ(1, mp2.size());
    mp1[20] = 200;
    EXPECT_EQ(1, mp2.size());
    EXPECT_EQ(2, mp1.size());
  }
  {
    Map<int, int> mp1, mp2;
    mp1[10] = 100;
    mp2[5] = 50;
    mp2 = std::move(mp1);
    EXPECT_EQ(1, mp2.size());
    EXPECT_EQ(100, mp2[10]);
    EXPECT_EQ(true, mp1.empty());
  }
}

TEST(Map, Access) {
  Map<int, std::string> mp;
  mp[1] = "one";
  EXPECT_EQ("one", mp[1]);
  EXPECT_EQ(1, mp.size());
  mp[1] = "ONE";
  EXPECT_EQ("ONE", mp[1]);
  EXPECT_EQ(1, mp.size());
  Map<int, int> mp2;
  EXPECT_EQ(0, mp2[42]);
  EXPECT_EQ(1, mp2.size());
  EXPECT_EQ("ONE", mp.at(1));
  EXPECT_THROW(mp.at(999), std::out_of_range);
  const auto& cmp = mp;
  EXPECT_EQ("ONE", cmp.at(1));
  EXPECT_THROW(cmp.at(999), std::out_of_range);
}

TEST(Map, Insert) {
  Map<int, std::string> mp;
  auto val = std::make_pair(1, std::string("one"));
  auto ret1 = mp.insert(val);
  EXPECT_EQ(true, ret1.second);
  EXPECT_EQ(1, ret1.first->first);
  EXPECT_EQ("one", ret1.first->second);
  EXPECT_EQ(1, mp.size());
  auto val2 = std::make_pair(1, std::string("duplicate"));
  auto ret2 = mp.insert(val2);
  EXPECT_EQ(false, ret2.second);
  EXPECT_EQ("one", ret2.first->second);
  EXPECT_EQ(1, mp.size());
  auto ret3 = mp.insert({2, "two"});
  EXPECT_EQ(true, ret3.second);
  EXPECT_EQ("two", ret3.first->second);
  EXPECT_EQ(2, mp.size());
}

TEST(Map, Erase) {
  Map<int, std::string> mp;
  mp[1] = "one";
  mp[2] = "two";
  mp[3] = "three";
  EXPECT_EQ(3, mp.size());
  auto it = mp.find(2);
  auto next = mp.erase(it);
  EXPECT_EQ(3, next->first);
  EXPECT_EQ(2, mp.size());
  size_t cnt = mp.erase(1);
  EXPECT_EQ(1, cnt);
  EXPECT_EQ(1, mp.size());
  EXPECT_EQ("three", mp[3]);
  cnt = mp.erase(999);
  EXPECT_EQ(0, cnt);
  EXPECT_EQ(1, mp.size());
  auto end_ret = mp.erase(mp.end());
  EXPECT_EQ(mp.end(), end_ret);
  mp.clear();
  EXPECT_EQ(true, mp.empty());
  EXPECT_EQ(0, mp.size());
}

TEST(Map, Iterator) {
  Map<int, int> mp;
  mp[10] = 100;
  mp[20] = 200;
  mp[30] = 300;
  auto it = mp.begin();
  EXPECT_EQ(10, it->first);
  EXPECT_EQ(100, it->second);
  EXPECT_EQ(20, (++it)->first);
  EXPECT_EQ(30, (++it)->first);
  EXPECT_EQ(mp.end(), ++it);
  auto end_it = mp.end();
  EXPECT_EQ(30, (--end_it)->first);
  EXPECT_EQ(20, (--end_it)->first);
  EXPECT_EQ(10, (--end_it)->first);
  EXPECT_EQ(mp.begin(), end_it);
  auto it2 = mp.begin();
  auto prev = it2++;
  EXPECT_EQ(10, prev->first);
  EXPECT_EQ(20, it2->first);
  auto it3 = --mp.end();
  auto prev2 = it3--;
  EXPECT_EQ(30, prev2->first);
  EXPECT_EQ(20, it3->first);
  mp[10] = 100;
  mp[20] = 200;
  auto it4 = mp.begin();
  it4->second = 999;
  EXPECT_EQ(999, mp[10]);
  EXPECT_EQ(10, mp.begin()->first);
  for (auto iter = mp.begin(); iter != mp.end(); ++iter) {
    iter->second *= 2;
  }
  EXPECT_EQ(1998, mp[10]);
  EXPECT_EQ(400, mp[20]);
  EXPECT_EQ(600, mp[30]);
  auto it5 = mp.begin();
  (*it5).second = 42;
  EXPECT_EQ(42, mp[10]);
  {
    auto it6 = mp.begin();
    const auto& cmp_ref = mp;
    auto cit6 = cmp_ref.begin();
    EXPECT_EQ(true, it6 == cit6);
    EXPECT_EQ(false, it6 != cit6);
    Map<int, int>::ConstIterator cit_from_it = it6;
    EXPECT_EQ(true, it6 == cit_from_it);
  }
  for (auto& p : mp) {
    p.second = 42;
  }
  for (const auto& p : mp) {
    EXPECT_EQ(42, p.second);
  }
  {
    Map<int, int> single;
    single[1] = 100;
    auto sit = single.begin();
    EXPECT_EQ(false, sit == single.end());
    EXPECT_EQ(1, sit->first);
    ++sit;
    EXPECT_EQ(true, sit == single.end());
    --sit;
    EXPECT_EQ(1, sit->first);
    EXPECT_EQ(100, sit->second);
    sit->second = 200;
    EXPECT_EQ(200, single[1]);
  }
  {
    Map<int, int> empty;
    EXPECT_EQ(empty.begin(), empty.end());
  }
  {
    Map<int, int> mp2;
    mp2[1] = 10;
    Map<int, int>::Iterator it = mp2.begin();
    Map<int, int>::ConstIterator cit = it;
    EXPECT_EQ(it, cit);
    EXPECT_EQ(10, cit->second);
  }
  {
    Map<int, int> mp2;
    mp2[10] = 100;
    mp2[20] = 200;
    mp2[30] = 300;
    auto it = mp2.begin();
    ++it;
    --it;
    EXPECT_EQ(10, it->first);
    ++it;
    ++it;
    --it;
    EXPECT_EQ(20, it->first);
  }
  {
    Map<int, int> mp2;
    mp2[1] = 10;
    mp2[2] = 20;
    auto it = mp2.begin();
    auto next = it++;
    EXPECT_EQ(10, next->second);
    EXPECT_EQ(20, it->second);
    auto prev = it--;
    EXPECT_EQ(20, prev->second);
    EXPECT_EQ(10, it->second);
  }
}

TEST(Map, Lookup) {
  Map<int, std::string> mp;
  mp[1] = "one";
  mp[2] = "two";
  mp[3] = "three";
  auto found = mp.find(2);
  EXPECT_EQ(false, found == mp.end());
  EXPECT_EQ("two", found->second);
  auto not_found = mp.find(99);
  EXPECT_EQ(true, not_found == mp.end());
  EXPECT_EQ(1, mp.count(2));
  EXPECT_EQ(0, mp.count(99));
  EXPECT_EQ(2, mp.lowerBound(2)->first);
  EXPECT_EQ(3, mp.lowerBound(2).operator++()->first);
  EXPECT_EQ(3, mp.upperBound(2)->first);
  EXPECT_EQ(mp.end(), mp.upperBound(3));
  auto range = mp.equalRange(2);
  EXPECT_EQ(2, range.first->first);
  EXPECT_EQ(3, range.second->first);
}

TEST(Map, Swap) {
  Map<int, int> a, b;
  a[1] = 10;
  b[2] = 20;
  b[3] = 30;
  a.swap(b);
  EXPECT_EQ(2, a.size());
  EXPECT_EQ(1, b.size());
  EXPECT_EQ(20, a[2]);
  EXPECT_EQ(30, a[3]);
  EXPECT_EQ(10, b[1]);
}

namespace {
struct MoveOnlyValue {
  int val;
  explicit MoveOnlyValue(int v) : val(v) {}
  MoveOnlyValue(MoveOnlyValue&&) = default;
  MoveOnlyValue& operator=(MoveOnlyValue&&) = default;
  MoveOnlyValue(const MoveOnlyValue&) = delete;
};
}  // namespace

TEST(Map, MoveOnly) {
  Map<int, MoveOnlyValue> mp;
  auto ret = mp.insert({1, MoveOnlyValue(10)});
  EXPECT_EQ(true, ret.second);
  EXPECT_EQ(10, ret.first->second.val);
  ret = mp.insert({1, MoveOnlyValue(99)});
  EXPECT_EQ(false, ret.second);
  EXPECT_EQ(10, ret.first->second.val);
  mp.insert({2, MoveOnlyValue(20)});
  mp.insert({3, MoveOnlyValue(30)});
  auto it = mp.find(2);
  auto next = mp.erase(it);
  EXPECT_EQ(3, next->first);
  EXPECT_EQ(2, mp.size());
  size_t cnt = mp.erase(1);
  EXPECT_EQ(1, cnt);
  EXPECT_EQ(1, mp.size());
  EXPECT_EQ(3, mp.begin()->first);
  Map<int, MoveOnlyValue> mp2;
  mp2.insert({99, MoveOnlyValue(999)});
  mp.swap(mp2);
  EXPECT_EQ(1, mp.size());
  EXPECT_EQ(99, mp.begin()->first);
}

TEST(Map, ConstCorrectness) {
  Map<int, int> mp;
  mp[10] = 100;
  mp[20] = 200;
  mp[30] = 300;
  const Map<int, int>& cmp = mp;
  EXPECT_EQ(false, cmp.empty());
  EXPECT_EQ(3, cmp.size());
  auto cit = cmp.begin();
  EXPECT_EQ(10, cit->first);
  auto found = cmp.find(20);
  EXPECT_EQ(false, found == cmp.end());
  EXPECT_EQ(200, found->second);
  auto not_found = cmp.find(99);
  EXPECT_EQ(true, not_found == cmp.end());
  EXPECT_EQ(1, cmp.count(20));
  EXPECT_EQ(0, cmp.count(99));
  EXPECT_EQ(20, cmp.lowerBound(20)->first);
  EXPECT_EQ(30, cmp.upperBound(20)->first);
  auto range = cmp.equalRange(20);
  EXPECT_EQ(20, range.first->first);
  EXPECT_EQ(30, range.second->first);
  EXPECT_EQ(200, cmp.at(20));
  EXPECT_THROW(cmp.at(99), std::out_of_range);
  int sum = 0;
  for (const auto& p : cmp) {
    sum += p.second;
  }
  EXPECT_EQ(600, sum);
}

TEST(Map, StringType) {
  Map<std::string, int> mp;
  mp["banana"] = 3;
  mp["apple"] = 1;
  mp["cherry"] = 5;
  EXPECT_EQ(3, mp.size());
  auto it = mp.begin();
  EXPECT_EQ("apple", it->first);
  EXPECT_EQ(1, it->second);
  EXPECT_EQ("banana", (++it)->first);
  EXPECT_EQ("cherry", (++it)->first);
  mp["apple"] = 100;
  EXPECT_EQ(100, mp["apple"]);
  EXPECT_EQ(3, mp.at("banana"));
  mp.erase("banana");
  EXPECT_EQ(2, mp.size());
  EXPECT_EQ(0, mp.count("banana"));
  std::string key = "date";
  mp[key] = 42;
  EXPECT_EQ(42, mp["date"]);
}

TEST(Map, Fuzzy) {
  RandomGenerator gen;
  const int query_times = 1000000;
  const int num_range = 1000;
  Map<int, int> mp;
  map<int, int> ref;
  for (int t = 0; t < query_times; t++) {
    int opt = gen.uniform_int(0, 7);
    int key = gen.uniform_int(0, num_range);
    int value = gen.uniform_int(0, 10000);
    switch (opt) {
      case 0:
        EXPECT_EQ(ref.empty(), mp.empty());
        break;
      case 1:
        EXPECT_EQ(ref.size(), mp.size());
        break;
      case 2:
        mp[key] = value;
        ref[key] = value;
        EXPECT_EQ(ref[key], mp[key]);
        break;
      case 3: {
        auto my_ans = mp.insert({key, value});
        auto ref_ans = ref.insert({key, value});
        EXPECT_EQ(ref_ans.second, my_ans.second);
        break;
      }
      case 4: {
        auto my_ans = mp.erase(key);
        auto ref_ans = ref.erase(key);
        EXPECT_EQ(ref_ans, my_ans);
        break;
      }
      case 5: {
        auto my_ans = (mp.find(key) == mp.end());
        auto ref_ans = (ref.find(key) == ref.end());
        EXPECT_EQ(ref_ans, my_ans);
        break;
      }
      case 6:
        EXPECT_EQ(ref.count(key), mp.count(key));
        break;
      case 7: {
        auto my_range = mp.equalRange(key);
        auto ref_range = ref.equal_range(key);
        if (ref_range.first == ref.end()) {
          EXPECT_EQ(true, my_range.first == mp.end());
        } else {
          EXPECT_EQ(ref_range.first->first, my_range.first->first);
        }
        if (ref_range.second == ref.end()) {
          EXPECT_EQ(true, my_range.second == mp.end());
        } else {
          EXPECT_EQ(ref_range.second->first, my_range.second->first);
        }
        break;
      }
      default:
        break;
    }
  }
}
