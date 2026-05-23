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

TEST(Vector, DefaultConstructor) {
  Vector<int> v;
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(v.size(), 0u);
  EXPECT_EQ(v.capacity(), 0u);
  EXPECT_EQ(v.data(), nullptr);
  EXPECT_EQ(v.begin(), v.end());
}

TEST(Vector, ConstructorCount) {
  Vector<int> v(5);
  std::vector<int> ref(5);
  full_compare(v, ref);
}

TEST(Vector, ConstructorCountVal) {
  Vector<int> v(5, 42);
  std::vector<int> ref(5, 42);
  full_compare(v, ref);
}

TEST(Vector, CopyConstructor) {
  Vector<int> v{1, 2, 3, 4, 5};
  Vector<int> copy(v);
  EXPECT_EQ(v, copy);
  copy[0] = 99;
  EXPECT_EQ(v[0], 1);
}

TEST(Vector, MoveConstructor) {
  Vector<int> v{1, 2, 3, 4, 5};
  Vector<int> moved(std::move(v));
  std::vector<int> ref{1, 2, 3, 4, 5};
  full_compare(moved, ref);
  EXPECT_TRUE(v.empty());
}

TEST(Vector, InitializerListConstructor) {
  Vector<int> v{10, 20, 30};
  std::vector<int> ref{10, 20, 30};
  full_compare(v, ref);
}

TEST(Vector, CopyAssignment) {
  Vector<int> v{1, 2, 3};
  Vector<int> copy;
  copy = v;
  EXPECT_EQ(v, copy);
  copy[0] = 99;
  EXPECT_EQ(v[0], 1);
}

TEST(Vector, MoveAssignment) {
  Vector<int> v{1, 2, 3};
  Vector<int> moved;
  moved = std::move(v);
  std::vector<int> ref{1, 2, 3};
  full_compare(moved, ref);
  EXPECT_TRUE(v.empty());
}

TEST(Vector, InitializerListAssignment) {
  Vector<int> v{1, 2, 3};
  v = {4, 5, 6, 7};
  std::vector<int> ref{4, 5, 6, 7};
  full_compare(v, ref);
}

TEST(Vector, SelfCopyAssignment) {
  Vector<int> v{1, 2, 3};
  v = v;
  std::vector<int> ref{1, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, SelfMoveAssignment) {
  Vector<int> v{1, 2, 3};
  v = std::move(v);
  std::vector<int> ref{1, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, Data) {
  Vector<int> v{1, 2, 3};
  int *p = v.data();
  EXPECT_EQ(p[0], 1);
  EXPECT_EQ(p[1], 2);
  EXPECT_EQ(p[2], 3);

  const Vector<int> &cv = v;
  const int *cp = cv.data();
  EXPECT_EQ(cp[0], 1);
  EXPECT_EQ(cp[1], 2);
  EXPECT_EQ(cp[2], 3);
}

TEST(Vector, DataEmpty) {
  Vector<int> v;
  EXPECT_EQ(v.data(), nullptr);
  const Vector<int> &cv = v;
  EXPECT_EQ(cv.data(), nullptr);
}

TEST(Vector, AssignCount) {
  Vector<int> v{1, 2, 3};
  v.assign(5, 42);
  std::vector<int> ref(5, 42);
  full_compare(v, ref);
}

TEST(Vector, AssignCountFromEmpty) {
  Vector<int> v;
  v.assign(3, 99);
  std::vector<int> ref(3, 99);
  full_compare(v, ref);
}

TEST(Vector, AssignCountZero) {
  Vector<int> v{1, 2, 3};
  v.assign(0, 42);
  EXPECT_TRUE(v.empty());
}

TEST(Vector, AssignInitializerList) {
  Vector<int> v{1, 2, 3};
  v.assign({7, 8, 9, 10, 11});
  std::vector<int> ref{7, 8, 9, 10, 11};
  full_compare(v, ref);
}

TEST(Vector, AssignInitializerListEmpty) {
  Vector<int> v{1, 2, 3};
  v.assign({});
  EXPECT_TRUE(v.empty());
}

TEST(Vector, ReverseIterator) {
  Vector<int> v{1, 2, 3, 4, 5};
  std::vector<int> result;
  for (auto it = v.rbegin(); it != v.rend(); ++it) {
    result.push_back(*it);
  }
  std::vector<int> expected{5, 4, 3, 2, 1};
  EXPECT_EQ(result, expected);
}

TEST(Vector, ConstReverseIterator) {
  const Vector<int> v{1, 2, 3};
  std::vector<int> result;
  for (auto it = v.crbegin(); it != v.crend(); ++it) {
    result.push_back(*it);
  }
  std::vector<int> expected{3, 2, 1};
  EXPECT_EQ(result, expected);
}

TEST(Vector, ReverseIteratorEmpty) {
  Vector<int> v;
  EXPECT_EQ(v.rbegin(), v.rend());
}

TEST(Vector, ResizeWithValue) {
  Vector<int> v{1, 2, 3};
  v.resize(5, 42);
  std::vector<int> ref{1, 2, 3, 42, 42};
  full_compare(v, ref);
}

TEST(Vector, ResizeWithValueShrink) {
  Vector<int> v{1, 2, 3, 4, 5};
  v.resize(2, 42);
  std::vector<int> ref{1, 2};
  full_compare(v, ref);
}

TEST(Vector, Swap) {
  Vector<int> v1{1, 2, 3};
  Vector<int> v2{4, 5, 6, 7};
  std::vector<int> ref1{1, 2, 3};
  std::vector<int> ref2{4, 5, 6, 7};
  v1.swap(v2);
  full_compare(v1, ref2);
  full_compare(v2, ref1);
}

TEST(Vector, ShrinkToFit) {
  Vector<int> v;
  v.reserve(100);
  v.pushBack(1);
  v.pushBack(2);
  v.pushBack(3);
  EXPECT_EQ(v.capacity(), 100u);
  v.shrinkToFit();
  EXPECT_EQ(v.capacity(), v.size());
  std::vector<int> ref{1, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, ShrinkToFitEmpty) {
  Vector<int> v;
  v.reserve(100);
  v.shrinkToFit();
  EXPECT_EQ(v.capacity(), 0u);
  EXPECT_TRUE(v.empty());
}

TEST(Vector, ShrinkToFitAlreadyTight) {
  Vector<int> v{1, 2, 3};
  size_t cap = v.capacity();
  v.shrinkToFit();
  EXPECT_EQ(v.capacity(), cap);
}

TEST(Vector, Equality) {
  Vector<int> v1{1, 2, 3};
  Vector<int> v2{1, 2, 3};
  Vector<int> v3{1, 2, 4};
  Vector<int> v4{1, 2};
  EXPECT_TRUE(v1 == v2);
  EXPECT_FALSE(v1 != v2);
  EXPECT_FALSE(v1 == v3);
  EXPECT_TRUE(v1 != v3);
  EXPECT_FALSE(v1 == v4);
  EXPECT_TRUE(v1 != v4);
}

TEST(Vector, EqualityEmpty) {
  Vector<int> v1;
  Vector<int> v2;
  EXPECT_TRUE(v1 == v2);
}

TEST(Vector, FrontBack) {
  Vector<int> v{10, 20, 30};
  EXPECT_EQ(v.front(), 10);
  EXPECT_EQ(v.back(), 30);
  v.front() = 100;
  v.back() = 300;
  EXPECT_EQ(v.front(), 100);
  EXPECT_EQ(v.back(), 300);

  const Vector<int> &cv = v;
  EXPECT_EQ(cv.front(), 100);
  EXPECT_EQ(cv.back(), 300);
}

TEST(Vector, FrontBackEmpty) {
  Vector<int> v;
  EXPECT_THROW(v.front(), std::out_of_range);
  EXPECT_THROW(v.back(), std::out_of_range);
  const Vector<int> &cv = v;
  EXPECT_THROW(cv.front(), std::out_of_range);
  EXPECT_THROW(cv.back(), std::out_of_range);
}

TEST(Vector, InsertSingle) {
  Vector<int> v{1, 2, 3};
  v.insert(v.begin() + 1, 99);
  std::vector<int> ref{1, 99, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, InsertSingleAtEnd) {
  Vector<int> v{1, 2, 3};
  v.insert(v.end(), 99);
  std::vector<int> ref{1, 2, 3, 99};
  full_compare(v, ref);
}

TEST(Vector, InsertSingleAtBegin) {
  Vector<int> v{1, 2, 3};
  v.insert(v.begin(), 99);
  std::vector<int> ref{99, 1, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, InsertSingleEmpty) {
  Vector<int> v;
  v.insert(v.begin(), 42);
  std::vector<int> ref{42};
  full_compare(v, ref);
}

TEST(Vector, InsertCount) {
  Vector<int> v{1, 2, 3};
  v.insert(v.begin() + 1, 3, 99);
  std::vector<int> ref{1, 99, 99, 99, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, InsertCountAtBegin) {
  Vector<int> v{1, 2, 3};
  v.insert(v.begin(), 2, 99);
  std::vector<int> ref{99, 99, 1, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, InsertCountAtEnd) {
  Vector<int> v{1, 2, 3};
  v.insert(v.end(), 2, 99);
  std::vector<int> ref{1, 2, 3, 99, 99};
  full_compare(v, ref);
}

TEST(Vector, InsertInitializerList) {
  Vector<int> v{1, 2, 3};
  v.insert(v.begin() + 1, {10, 20, 30});
  std::vector<int> ref{1, 10, 20, 30, 2, 3};
  full_compare(v, ref);
}

TEST(Vector, InsertOutOfRange) {
  Vector<int> v{1, 2, 3};
  EXPECT_THROW(v.insert(v.end() + 1, 42), std::out_of_range);
  EXPECT_THROW(v.insert(v.begin() - 1, 42), std::out_of_range);
}

TEST(Vector, EraseSingle) {
  Vector<int> v{1, 2, 3, 4, 5};
  v.erase(v.begin() + 2);
  std::vector<int> ref{1, 2, 4, 5};
  full_compare(v, ref);
}

TEST(Vector, EraseFirst) {
  Vector<int> v{1, 2, 3};
  v.erase(v.begin());
  std::vector<int> ref{2, 3};
  full_compare(v, ref);
}

TEST(Vector, EraseLast) {
  Vector<int> v{1, 2, 3};
  v.erase(v.end() - 1);
  std::vector<int> ref{1, 2};
  full_compare(v, ref);
}

TEST(Vector, EraseEmpty) {
  Vector<int> v;
  EXPECT_THROW(v.erase(v.begin()), std::out_of_range);
}

TEST(Vector, EraseOutOfRange) {
  Vector<int> v{1, 2, 3};
  EXPECT_THROW(v.erase(v.end()), std::out_of_range);
  EXPECT_THROW(v.erase(v.begin() - 1), std::out_of_range);
}

TEST(Vector, Emplace) {
  Vector<Tmp> v;
  v.emplaceBack(1, 2);
  v.emplaceBack(3, 4);
  v.emplace(v.begin() + 1, 5, 6);
  EXPECT_EQ(v[0], Tmp(1, 2));
  EXPECT_EQ(v[1], Tmp(5, 6));
  EXPECT_EQ(v[2], Tmp(3, 4));
}

TEST(Vector, EmplaceAtEnd) {
  Vector<Tmp> v;
  v.emplaceBack(1, 2);
  v.emplace(v.end(), 3, 4);
  EXPECT_EQ(v[0], Tmp(1, 2));
  EXPECT_EQ(v[1], Tmp(3, 4));
}

TEST(Vector, EmplaceOutOfRange) {
  Vector<Tmp> v;
  v.emplaceBack(1, 2);
  EXPECT_THROW(v.emplace(v.end() + 1, 3, 4), std::out_of_range);
}

TEST(Vector, ResizeGrow) {
  Vector<int> v{1, 2, 3};
  v.resize(5);
  std::vector<int> ref{1, 2, 3, 0, 0};
  full_compare(v, ref);
}

TEST(Vector, ResizeShrink) {
  Vector<int> v{1, 2, 3, 4, 5};
  v.resize(2);
  std::vector<int> ref{1, 2};
  full_compare(v, ref);
}

TEST(Vector, ResizeEmpty) {
  Vector<int> v;
  v.resize(3);
  std::vector<int> ref{0, 0, 0};
  full_compare(v, ref);
}

TEST(Vector, Reserve) {
  Vector<int> v;
  v.reserve(100);
  EXPECT_GE(v.capacity(), 100u);
  EXPECT_TRUE(v.empty());
}

TEST(Vector, ReserveSmaller) {
  Vector<int> v;
  v.pushBack(1);
  size_t cap = v.capacity();
  v.reserve(1);
  EXPECT_EQ(v.capacity(), cap);
}

TEST(Vector, Clear) {
  Vector<int> v{1, 2, 3};
  size_t cap = v.capacity();
  v.clear();
  EXPECT_TRUE(v.empty());
  EXPECT_EQ(v.capacity(), cap);
}

TEST(Vector, PopBackOutOfRange) {
  Vector<int> v;
  EXPECT_THROW(v.popBack(), std::out_of_range);
}

TEST(Vector, AtOutOfRange) {
  Vector<int> v{1, 2, 3};
  EXPECT_THROW(v.at(3), std::out_of_range);
  const Vector<int> &cv = v;
  EXPECT_THROW(cv.at(3), std::out_of_range);
}

TEST(Vector, PushBackMoveOnly) {
  Vector<std::unique_ptr<int>> v;
  v.pushBack(std::make_unique<int>(42));
  v.pushBack(std::make_unique<int>(99));
  EXPECT_EQ(*v[0], 42);
  EXPECT_EQ(*v[1], 99);
}

TEST(Vector, PushBackRvalue) {
  Vector<std::string> v;
  std::string s = "hello";
  v.pushBack(std::move(s));
  EXPECT_EQ(v[0], "hello");
  EXPECT_TRUE(s.empty());
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
