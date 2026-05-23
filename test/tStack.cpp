#include <gtest/gtest.h>

#include <stack>
#include <stdexcept>
#include <string>

#include "Stack.hpp"
#include "test_utils.hpp"

using mystd::stack::Stack;

TEST(Stack, DefaultConstructor) {
  Stack<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(Stack, InitializerListConstructor) {
  Stack<int> s{1, 2, 3, 4, 5};
  EXPECT_EQ(s.size(), 5u);
  EXPECT_EQ(s.top(), 5);
}

TEST(Stack, CopyConstructor) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2(s1);
  EXPECT_EQ(s1, s2);
  s2.push(4);
  EXPECT_EQ(s1.size(), 3u);
  EXPECT_EQ(s2.size(), 4u);
}

TEST(Stack, MoveConstructor) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2(std::move(s1));
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 3);
  EXPECT_TRUE(s1.empty());
}

TEST(Stack, CopyAssignment) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2{4, 5};
  s2 = s1;
  EXPECT_EQ(s1, s2);
  s2.push(99);
  EXPECT_EQ(s1.size(), 3u);
}

TEST(Stack, MoveAssignment) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2;
  s2 = std::move(s1);
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 3);
  EXPECT_TRUE(s1.empty());
}

TEST(Stack, SelfCopyAssignment) {
  Stack<int> s{1, 2, 3};
  s = s;
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s.top(), 3);
}

TEST(Stack, SelfMoveAssignment) {
  Stack<int> s{1, 2, 3};
  s = std::move(s);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s.top(), 3);
}

TEST(Stack, TopMutable) {
  Stack<int> s{1, 2, 3};
  s.top() = 99;
  EXPECT_EQ(s.top(), 99);
}

TEST(Stack, TopConst) {
  const Stack<int> s{1, 2, 3};
  EXPECT_EQ(s.top(), 3);
}

TEST(Stack, TopEmpty) {
  Stack<int> s;
  EXPECT_THROW(s.top(), std::out_of_range);
  const Stack<int> &cs = s;
  EXPECT_THROW(cs.top(), std::out_of_range);
}

TEST(Stack, PushLvalue) {
  Stack<int> s;
  int x = 42;
  s.push(x);
  EXPECT_EQ(s.top(), 42);
}

TEST(Stack, PushRvalue) {
  Stack<std::string> s;
  std::string str = "hello";
  s.push(std::move(str));
  EXPECT_EQ(s.top(), "hello");
  EXPECT_TRUE(str.empty());
}

TEST(Stack, Emplace) {
  Stack<std::string> s;
  s.emplace(3, 'a');
  EXPECT_EQ(s.top(), "aaa");
}

TEST(Stack, EmplaceMultipleArgs) {
  struct Point {
    int x, y;
    Point(int _x, int _y) : x(_x), y(_y) {}
    bool operator==(const Point &o) const { return x == o.x && y == o.y; }
  };
  Stack<Point> s;
  s.emplace(10, 20);
  EXPECT_EQ(s.top().x, 10);
  EXPECT_EQ(s.top().y, 20);
}

TEST(Stack, Pop) {
  Stack<int> s{1, 2, 3};
  s.pop();
  EXPECT_EQ(s.size(), 2u);
  EXPECT_EQ(s.top(), 2);
  s.pop();
  EXPECT_EQ(s.top(), 1);
  s.pop();
  EXPECT_TRUE(s.empty());
}

TEST(Stack, PopEmpty) {
  Stack<int> s;
  EXPECT_THROW(s.pop(), std::out_of_range);
}

TEST(Stack, Swap) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2{4, 5};
  s1.swap(s2);
  EXPECT_EQ(s1.size(), 2u);
  EXPECT_EQ(s1.top(), 5);
  EXPECT_EQ(s2.size(), 3u);
  EXPECT_EQ(s2.top(), 3);
}

TEST(Stack, Equality) {
  Stack<int> s1{1, 2, 3};
  Stack<int> s2{1, 2, 3};
  Stack<int> s3{1, 2, 4};
  Stack<int> s4{1, 2};
  EXPECT_TRUE(s1 == s2);
  EXPECT_FALSE(s1 != s2);
  EXPECT_FALSE(s1 == s3);
  EXPECT_TRUE(s1 != s3);
  EXPECT_FALSE(s1 == s4);
  EXPECT_TRUE(s1 != s4);
}

TEST(Stack, EqualityEmpty) {
  Stack<int> s1;
  Stack<int> s2;
  EXPECT_TRUE(s1 == s2);
}

TEST(Stack, EmptyAndSize) {
  Stack<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0u);
  s.push(1);
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.size(), 1u);
  s.push(2);
  EXPECT_EQ(s.size(), 2u);
  s.pop();
  EXPECT_EQ(s.size(), 1u);
}

TEST(Stack, RandomOps) {
  RandomGenerator gen;
  Stack<int> s;
  std::stack<int> ref;
  const int OPS = 100000;
  const int MAX_VAL = 100000;

  for (int t = 0; t < OPS; t++) {
    int opt = gen.uniform_int(0, 5);
    switch (opt) {
      case 0:
        EXPECT_EQ(ref.empty(), s.empty());
        break;
      case 1:
        EXPECT_EQ(ref.size(), s.size());
        break;
      case 2:
        if (s.empty()) {
          EXPECT_THROW(s.top(), std::out_of_range);
        } else {
          EXPECT_EQ(ref.top(), s.top());
        }
        break;
      case 3:
        if (s.empty()) {
          EXPECT_THROW(s.pop(), std::out_of_range);
        } else {
          ref.pop();
          s.pop();
        }
        break;
      case 4: {
        int val = gen.uniform_int(0, MAX_VAL);
        ref.push(val);
        s.push(val);
        break;
      }
      case 5: {
        int val = gen.uniform_int(0, MAX_VAL);
        ref.push(val);
        s.emplace(val);
        break;
      }
      default:
        break;
    }
    if ((t & 0xFF) == 0) {
      EXPECT_EQ(ref.size(), s.size());
      if (!ref.empty()) {
        EXPECT_EQ(ref.top(), s.top());
      }
    }
  }
}