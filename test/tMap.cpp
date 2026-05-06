#include <map>
#include <string>

#include "Map.hpp"
#include "test.h"
using mystd::map::Map;
using std::map;

static void map_construct_impl() {
  // 默认构造
  {
    Map<int, int> mp;
    CHECK_EQ(0, mp.size());
    CHECK_EQ(true, mp.empty());
    CHECK_EQ(mp.begin(), mp.end());
  }

  // 自定义比较器构造
  {
    Map<int, int, mystd::compare::Greater<int>> mp;
    mp[10] = 100;
    mp[20] = 200;
    mp[5] = 50;
    auto it = mp.begin();
    CHECK_EQ(20, it->first);
    CHECK_EQ(200, it->second);
    CHECK_EQ(10, (++it)->first);
  }

  // 拷贝构造
  {
    Map<int, int> mp1;
    mp1[1] = 10;
    mp1[2] = 20;
    Map<int, int> mp2(mp1);

    CHECK_EQ(2, mp2.size());
    mp1[3] = 30;
    CHECK_EQ(3, mp1.size());
    CHECK_EQ(2, mp2.size());
  }

  // 移动构造
  {
    Map<int, int> mp1;
    mp1[1] = 10;
    mp1[2] = 20;
    Map<int, int> mp2(std::move(mp1));

    CHECK_EQ(2, mp2.size());
    CHECK_EQ(true, mp1.empty());
  }

  // 拷贝赋值与自赋值
  {
    Map<int, int> mp1, mp2;
    mp1[10] = 100;
    mp2 = mp1;
    CHECK_EQ(1, mp2.size());

    mp1[20] = 200;
    CHECK_EQ(1, mp2.size());
    CHECK_EQ(2, mp1.size());
  }

  // 移动赋值
  {
    Map<int, int> mp1, mp2;
    mp1[10] = 100;
    mp2[5] = 50;
    mp2 = std::move(mp1);

    CHECK_EQ(1, mp2.size());
    CHECK_EQ(100, mp2[10]);
    CHECK_EQ(true, mp1.empty());
  }
}

static void map_access_impl() {
  Map<int, std::string> mp;

  // operator[]: 插入新元素
  mp[1] = "one";
  CHECK_EQ("one", mp[1]);
  CHECK_EQ(1, mp.size());

  // operator[]: 覆盖已有元素
  mp[1] = "ONE";
  CHECK_EQ("ONE", mp[1]);
  CHECK_EQ(1, mp.size());

  // operator[]: 默认值（int 类型）
  Map<int, int> mp2;
  CHECK_EQ(0, mp2[42]);
  CHECK_EQ(1, mp2.size());

  // at(): 查找已有元素
  CHECK_EQ("ONE", mp.at(1));

  // at(): 不存在的 key 抛出异常
  EXPECT_THROW(mp.at(999), std::out_of_range);

  // at() const
  const auto& cmp = mp;
  CHECK_EQ("ONE", cmp.at(1));
  EXPECT_THROW(cmp.at(999), std::out_of_range);
}

static void map_insert_impl() {
  Map<int, std::string> mp;

  // 插入新元素 (lvalue)
  auto val = std::make_pair(1, std::string("one"));
  auto ret1 = mp.insert(val);
  CHECK_EQ(true, ret1.second);
  CHECK_EQ(1, ret1.first->first);
  CHECK_EQ("one", ret1.first->second);
  CHECK_EQ(1, mp.size());

  // 插入重复 key
  auto val2 = std::make_pair(1, std::string("duplicate"));
  auto ret2 = mp.insert(val2);
  CHECK_EQ(false, ret2.second);
  CHECK_EQ("one", ret2.first->second);
  CHECK_EQ(1, mp.size());

  // 插入新元素 (rvalue)
  auto ret3 = mp.insert({2, "two"});
  CHECK_EQ(true, ret3.second);
  CHECK_EQ("two", ret3.first->second);
  CHECK_EQ(2, mp.size());
}

static void map_erase_impl() {
  Map<int, std::string> mp;
  mp[1] = "one";
  mp[2] = "two";
  mp[3] = "three";
  CHECK_EQ(3, mp.size());

  // erase by iterator
  auto it = mp.find(2);
  auto next = mp.erase(it);
  CHECK_EQ(3, next->first);
  CHECK_EQ(2, mp.size());

  // erase by key
  size_t cnt = mp.erase(1);
  CHECK_EQ(1, cnt);
  CHECK_EQ(1, mp.size());
  CHECK_EQ("three", mp[3]);

  // erase non-existent key
  cnt = mp.erase(999);
  CHECK_EQ(0, cnt);
  CHECK_EQ(1, mp.size());

  // erase end()
  auto end_ret = mp.erase(mp.end());
  CHECK_EQ(mp.end(), end_ret);

  // clear
  mp.clear();
  CHECK_EQ(true, mp.empty());
  CHECK_EQ(0, mp.size());
}

static void map_iterator_impl() {
  Map<int, int> mp;
  mp[10] = 100;
  mp[20] = 200;
  mp[30] = 300;

  // 1. operator++ 正向遍历
  auto it = mp.begin();
  CHECK_EQ(10, it->first);
  CHECK_EQ(100, it->second);
  CHECK_EQ(20, (++it)->first);
  CHECK_EQ(30, (++it)->first);
  CHECK_EQ(mp.end(), ++it);

  // 2. operator-- 反向遍历
  auto end_it = mp.end();
  CHECK_EQ(30, (--end_it)->first);
  CHECK_EQ(20, (--end_it)->first);
  CHECK_EQ(10, (--end_it)->first);
  CHECK_EQ(mp.begin(), end_it);

  // 3. 后置++
  auto it2 = mp.begin();
  auto prev = it2++;
  CHECK_EQ(10, prev->first);
  CHECK_EQ(20, it2->first);

  // 4. 后置--
  auto it3 = --mp.end();
  auto prev2 = it3--;
  CHECK_EQ(30, prev2->first);
  CHECK_EQ(20, it3->first);

  // 5. Iterator 修改 value（非 const 版本的关键能力）
  mp[10] = 100;
  mp[20] = 200;
  auto it4 = mp.begin();
  it4->second = 999;
  CHECK_EQ(999, mp[10]);
  // 验证不会意外修改 key（key 是 const 的）
  CHECK_EQ(10, mp.begin()->first);

  // 6. 通过 Iterator 批量修改所有元素
  for (auto iter = mp.begin(); iter != mp.end(); ++iter) {
    iter->second *= 2;
  }
  CHECK_EQ(1998, mp[10]);
  CHECK_EQ(400, mp[20]);
  CHECK_EQ(600, mp[30]);

  // 7. 通过 operator* 修改 value
  auto it5 = mp.begin();
  (*it5).second = 42;
  CHECK_EQ(42, mp[10]);

  // 8. Iterator 与 ConstIterator 的转换和比较
  {
    auto it6 = mp.begin();
    // const 引用获得 ConstIterator
    const auto& cmp_ref = mp;
    auto cit6 = cmp_ref.begin();
    CHECK_EQ(true, it6 == cit6);
    CHECK_EQ(false, it6 != cit6);

    // Iterator 可隐式转换为 ConstIterator
    Map<int, int>::ConstIterator cit_from_it = it6;
    CHECK_EQ(true, it6 == cit_from_it);
  }

  // 9. 通过 range-based for 修改所有元素
  for (auto& p : mp) {
    p.second = 42;
  }
  for (const auto& p : mp) {
    CHECK_EQ(42, p.second);
  }

  // 10. 单元素迭代器边界
  {
    Map<int, int> single;
    single[1] = 100;
    auto sit = single.begin();
    CHECK_EQ(false, sit == single.end());
    CHECK_EQ(1, sit->first);
    ++sit;
    CHECK_EQ(true, sit == single.end());
    --sit;
    CHECK_EQ(1, sit->first);
    CHECK_EQ(100, sit->second);

    // 修改单元素
    sit->second = 200;
    CHECK_EQ(200, single[1]);
  }

  // 11. 空 map 迭代器
  {
    Map<int, int> empty;
    CHECK_EQ(empty.begin(), empty.end());
  }

  // 12. Iterator 隐式转换为 ConstIterator
  {
    Map<int, int> mp2;
    mp2[1] = 10;
    Map<int, int>::Iterator it = mp2.begin();
    Map<int, int>::ConstIterator cit = it;  // 隐式转换
    CHECK_EQ(it, cit);
    CHECK_EQ(10, cit->second);
  }

  // 13. 混合前进后退
  {
    Map<int, int> mp2;
    mp2[10] = 100;
    mp2[20] = 200;
    mp2[30] = 300;
    auto it = mp2.begin();
    ++it;  // -> 20
    --it;  // -> 10
    CHECK_EQ(10, it->first);
    ++it;  // -> 20
    ++it;  // -> 30
    --it;  // -> 20
    CHECK_EQ(20, it->first);
  }

  // 14. 后置 ++ 和 -- 返回的值可解引用
  {
    Map<int, int> mp2;
    mp2[1] = 10;
    mp2[2] = 20;
    auto it = mp2.begin();
    auto next = it++;
    CHECK_EQ(10, next->second);
    CHECK_EQ(20, it->second);
    auto prev = it--;
    CHECK_EQ(20, prev->second);
    CHECK_EQ(10, it->second);
  }
}

static void map_lookup_impl() {
  Map<int, std::string> mp;
  mp[1] = "one";
  mp[2] = "two";
  mp[3] = "three";

  // find: 存在的 key
  auto found = mp.find(2);
  CHECK_EQ(false, found == mp.end());
  CHECK_EQ("two", found->second);

  // find: 不存在的 key
  auto not_found = mp.find(99);
  CHECK_EQ(true, not_found == mp.end());

  // count
  CHECK_EQ(1, mp.count(2));
  CHECK_EQ(0, mp.count(99));

  // lowerBound
  CHECK_EQ(2, mp.lowerBound(2)->first);
  CHECK_EQ(3, mp.lowerBound(2).operator++()->first);

  // upperBound
  CHECK_EQ(3, mp.upperBound(2)->first);
  CHECK_EQ(mp.end(), mp.upperBound(3));

  // equalRange
  auto range = mp.equalRange(2);
  CHECK_EQ(2, range.first->first);
  CHECK_EQ(3, range.second->first);
}

static void map_swap_impl() {
  Map<int, int> a, b;
  a[1] = 10;
  b[2] = 20;
  b[3] = 30;

  a.swap(b);
  CHECK_EQ(2, a.size());
  CHECK_EQ(1, b.size());
  CHECK_EQ(20, a[2]);
  CHECK_EQ(30, a[3]);
  CHECK_EQ(10, b[1]);
}

namespace {
struct MoveOnlyValue {
  int val;
  explicit MoveOnlyValue(int v) : val(v) {}
  MoveOnlyValue(MoveOnlyValue&&) = default;
  MoveOnlyValue& operator=(MoveOnlyValue&&) = default;
  MoveOnlyValue(const MoveOnlyValue&) = delete;
  MoveOnlyValue& operator=(const MoveOnlyValue&) = delete;
};
}  // namespace

static void map_move_only_impl() {
  Map<int, MoveOnlyValue> mp;

  // 1. insert(T&&)
  auto ret = mp.insert({1, MoveOnlyValue(10)});
  CHECK_EQ(true, ret.second);
  CHECK_EQ(10, ret.first->second.val);

  // 2. 重复 insert
  ret = mp.insert({1, MoveOnlyValue(99)});
  CHECK_EQ(false, ret.second);
  CHECK_EQ(10, ret.first->second.val);

  mp.insert({2, MoveOnlyValue(20)});
  mp.insert({3, MoveOnlyValue(30)});

  // 3. erase by iterator
  auto it = mp.find(2);
  auto next = mp.erase(it);
  CHECK_EQ(3, next->first);
  CHECK_EQ(2, mp.size());

  // 4. erase by key
  size_t cnt = mp.erase(1);
  CHECK_EQ(1, cnt);
  CHECK_EQ(1, mp.size());
  CHECK_EQ(3, mp.begin()->first);

  // 5. swap
  Map<int, MoveOnlyValue> mp2;
  mp2.insert({99, MoveOnlyValue(999)});
  mp.swap(mp2);
  CHECK_EQ(1, mp.size());
  CHECK_EQ(99, mp.begin()->first);
}

static void map_const_impl() {
  Map<int, int> mp;
  mp[10] = 100;
  mp[20] = 200;
  mp[30] = 300;

  const Map<int, int>& cmp = mp;

  // 1. empty/size
  CHECK_EQ(false, cmp.empty());
  CHECK_EQ(3, cmp.size());

  // 2. begin/end (const)
  auto cit = cmp.begin();
  CHECK_EQ(10, cit->first);

  // 3. find const
  auto found = cmp.find(20);
  CHECK_EQ(false, found == cmp.end());
  CHECK_EQ(200, found->second);

  auto not_found = cmp.find(99);
  CHECK_EQ(true, not_found == cmp.end());

  // 4. count const
  CHECK_EQ(1, cmp.count(20));
  CHECK_EQ(0, cmp.count(99));

  // 5. lowerBound/upperBound const
  CHECK_EQ(20, cmp.lowerBound(20)->first);
  CHECK_EQ(30, cmp.upperBound(20)->first);

  // 6. equalRange const
  auto range = cmp.equalRange(20);
  CHECK_EQ(20, range.first->first);
  CHECK_EQ(30, range.second->first);

  // 7. at const
  CHECK_EQ(200, cmp.at(20));
  EXPECT_THROW(cmp.at(99), std::out_of_range);

  // 8. 范围 for（const map 应遍历所有元素）
  int sum = 0;
  for (const auto& p : cmp) {
    sum += p.second;
  }
  CHECK_EQ(600, sum);
}

static void map_string_impl() {
  Map<std::string, int> mp;

  // 1. Insert & Order
  mp["banana"] = 3;
  mp["apple"] = 1;
  mp["cherry"] = 5;

  CHECK_EQ(3, mp.size());

  auto it = mp.begin();
  CHECK_EQ("apple", it->first);
  CHECK_EQ(1, it->second);
  CHECK_EQ("banana", (++it)->first);
  CHECK_EQ("cherry", (++it)->first);

  // 2. 覆盖已有值
  mp["apple"] = 100;
  CHECK_EQ(100, mp["apple"]);

  // 3. 查找
  CHECK_EQ(3, mp.at("banana"));

  // 4. 删除
  mp.erase("banana");
  CHECK_EQ(2, mp.size());
  CHECK_EQ(0, mp.count("banana"));

  // 5. 移动插入
  std::string key = "date";
  mp[key] = 42;
  CHECK_EQ(42, mp["date"]);
}

static void map_fuzzy_impl() {
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
      case 0: {
        CHECK_EQ(ref.empty(), mp.empty());
        break;
      }
      case 1: {
        CHECK_EQ(ref.size(), mp.size());
        break;
      }
      case 2: {
        mp[key] = value;
        ref[key] = value;
        CHECK_EQ(ref[key], mp[key]);
        break;
      }
      case 3: {
        auto my_ans = mp.insert({key, value});
        auto ref_ans = ref.insert({key, value});
        CHECK_EQ(ref_ans.second, my_ans.second);
        break;
      }
      case 4: {
        auto my_ans = mp.erase(key);
        auto ref_ans = ref.erase(key);
        CHECK_EQ(ref_ans, my_ans);
        break;
      }
      case 5: {
        auto my_ans = (mp.find(key) == mp.end());
        auto ref_ans = (ref.find(key) == ref.end());
        CHECK_EQ(ref_ans, my_ans);
        break;
      }
      case 6: {
        CHECK_EQ(ref.count(key), mp.count(key));
        break;
      }
      case 7: {
        auto my_range = mp.equalRange(key);
        auto ref_range = ref.equal_range(key);
        if (ref_range.first == ref.end()) {
          CHECK_EQ(true, my_range.first == mp.end());
        } else {
          CHECK_EQ(ref_range.first->first, my_range.first->first);
        }
        if (ref_range.second == ref.end()) {
          CHECK_EQ(true, my_range.second == mp.end());
        } else {
          CHECK_EQ(ref_range.second->first, my_range.second->first);
        }
        break;
      }
      default:
        break;
    }
  }
}

MAKE_TEST(Map, Construct) { map_construct_impl(); }
MAKE_TEST(Map, Access) { map_access_impl(); }
MAKE_TEST(Map, Insert) { map_insert_impl(); }
MAKE_TEST(Map, Erase) { map_erase_impl(); }
MAKE_TEST(Map, Iterator) { map_iterator_impl(); }
MAKE_TEST(Map, Lookup) { map_lookup_impl(); }
MAKE_TEST(Map, Swap) { map_swap_impl(); }
MAKE_TEST(Map, MoveOnly) { map_move_only_impl(); }
MAKE_TEST(Map, ConstCorrectness) { map_const_impl(); }
MAKE_TEST(Map, StringType) { map_string_impl(); }
MAKE_TEST(Map, Fuzzy) { map_fuzzy_impl(); }
