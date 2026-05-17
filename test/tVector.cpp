#include <gtest/gtest.h>

#include <vector>

#include "Vector.hpp"
#include "test_utils.hpp"

using namespace mystd::vector;

namespace TestVector {
class Tmp {
private:
  int a, b;

public:
  Tmp() : a(0), b(0) {}
  Tmp(int _a, int _b) : a(_a), b(_b) {}
  bool operator==(const Tmp &o) const { return a == o.a && b == o.b; }
};
}  // namespace TestVector
using namespace TestVector;

template <typename T>
static void full_compare(const Vector<T> &v, const std::vector<T> &ref) {
  ASSERT_EQ(ref.size(), v.size());
  for (size_t i = 0; i < ref.size(); ++i) {
    EXPECT_EQ(ref[i], v[i]);
  }
}

TEST(Vector, RandomInt) {
  Vector<int> v;
  std::vector<int> ref;
  const int OPS = (256 * 2048);
  const int MAX_VAL = 1000000;
  RandomGenerator gen;

  for (int i = 0; i < OPS; ++i) {
    int op = gen.uniform_int(0, 7);
    if (op == 0) {
      int x = gen.uniform_int(0, MAX_VAL);
      v.pushBack(x);
      ref.push_back(x);
    } else if (op == 1) {
      if (ref.empty()) {
        EXPECT_THROW(v.popBack(), std::out_of_range);
      } else {
        v.popBack();
        ref.pop_back();
      }
    } else if (op == 2) {
      size_t pos = gen.uniform_int(0ul, ref.size());
      int x = gen.uniform_int(0, MAX_VAL);
      v.insert(v.begin() + pos, x);
      ref.insert(ref.begin() + pos, x);
    } else if (op == 3) {
      if (ref.empty()) {
        EXPECT_THROW(v.erase(v.begin()), std::out_of_range);
      } else {
        size_t pos = gen.uniform_int(0ul, ref.size() - 1);
        v.erase(v.begin() + pos);
        ref.erase(ref.begin() + pos);
      }
    } else if (op == 4) {
      size_t new_size = gen.uniform_int(0, 200);
      v.resize(new_size);
      ref.resize(new_size);
    } else if (op == 5) {
      size_t cap = gen.uniform_int(0, 500);
      v.reserve(cap);
      ref.reserve(cap);
    } else if (op == 6) {
      if (ref.empty()) {
        EXPECT_THROW(v.at(0), std::out_of_range);
      } else {
        size_t pos = gen.uniform_int(0ul, ref.size() - 1);
        EXPECT_EQ(ref[pos], v[pos]);
        EXPECT_EQ(ref[pos], v.at(pos));
        EXPECT_EQ(ref.front(), v.front());
        EXPECT_EQ(ref.back(), v.back());
      }
    } else {
      v.clear();
      ref.clear();
    }

    if ((i & 0xFF) == 0) {
      full_compare<int>(v, ref);
    }
  }
  full_compare<int>(v, ref);
}

TEST(Vector, EmplaceNonTrivial) {
  Vector<Tmp> v;
  std::vector<Tmp> ref;
  const int OPS = (256 * 2048);
  RandomGenerator gen;

  for (int i = 0; i < OPS; ++i) {
    int op = gen.uniform_int(0, 4);
    if (op == 0) {
      int a = gen.uniform_int(0, 999);
      int b = gen.uniform_int(0, 999);
      v.emplaceBack(a, b);
      ref.emplace_back(a, b);
    } else if (op == 1) {
      int a = gen.uniform_int(0, 999);
      int b = gen.uniform_int(0, 999);
      if (ref.empty()) {
        v.emplaceBack(a, b);
        ref.emplace_back(a, b);
      } else {
        v.emplace(v.begin(), a, b);
        ref.insert(ref.begin(), Tmp(a, b));
      }
    } else if (op == 2) {
      if (ref.empty()) {
        EXPECT_THROW(v.popBack(), std::out_of_range);
      } else {
        v.popBack();
        ref.pop_back();
      }
    } else if (op == 3) {
      size_t new_size = gen.uniform_int(0, 100);
      v.resize(new_size);
      ref.resize(new_size);
    } else {
      v.clear();
      ref.clear();
    }

    if ((i & 0xFF) == 0) {
      full_compare<Tmp>(v, ref);
    }
  }
  full_compare<Tmp>(v, ref);
}
