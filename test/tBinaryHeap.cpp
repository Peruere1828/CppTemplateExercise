#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "BinaryHeap.hpp"
#include "Compare.hpp"

using namespace mystd::binary_heap;
using namespace mystd::compare;

struct Person {
  std::string name;
  int score;

  Person(std::string n, int s) : name(std::move(n)), score(s) {}

  bool operator<(const Person &other) const { return score < other.score; }
  bool operator>(const Person &other) const { return score > other.score; }

  bool operator==(const Person &other) const {
    return score == other.score && name == other.name;
  }
};

TEST(BinaryHeap, BasicInt) {
  BinaryHeap<int, Greater<int>> min_heap;
  EXPECT_EQ(true, min_heap.empty());
  min_heap.push(5);
  min_heap.push(3);
  min_heap.push(7);
  min_heap.push(1);
  EXPECT_EQ(4u, min_heap.size());
  EXPECT_EQ(1, min_heap.top());
  min_heap.pop();
  EXPECT_EQ(3, min_heap.top());
  min_heap.pop();
  EXPECT_EQ(5, min_heap.top());
  BinaryHeap<int, Less<int>> max_heap{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(6, max_heap.top());
  max_heap.pop();
  EXPECT_EQ(5, max_heap.top());
  max_heap.pop();
  EXPECT_EQ(4, max_heap.top());
}

TEST(BinaryHeap, Person) {
  BinaryHeap<Person, Greater<Person>> min_heap;
  min_heap.emplace("Alice", 90);
  min_heap.emplace("Bob", 70);
  min_heap.emplace("Charlie", 85);
  EXPECT_EQ(3u, min_heap.size());
  EXPECT_EQ(Person("Bob", 70), min_heap.top());
  min_heap.pop();
  EXPECT_EQ(Person("Charlie", 85), min_heap.top());
}

TEST(BinaryHeap, InitList) {
  BinaryHeap<int, Less<int>> max_heap{3, 1, 4, 2};
  EXPECT_EQ(4u, max_heap.size());
  EXPECT_EQ(4, max_heap.top());
}

TEST(BinaryHeap, Exceptions) {
  BinaryHeap<int, Greater<int>> h;
  EXPECT_THROW(h.top(), std::out_of_range);
  EXPECT_THROW(h.pop(), std::out_of_range);
  h.push(1);
  EXPECT_NO_THROW(h.top());
  EXPECT_NO_THROW(h.pop());
  EXPECT_THROW(h.pop(), std::out_of_range);
}

TEST(BinaryHeap, Swap) {
  BinaryHeap<int, Greater<int>> h1{3, 1, 4, 1, 5};
  BinaryHeap<int, Greater<int>> h2{9, 2, 6};
  EXPECT_EQ(5u, h1.size());
  EXPECT_EQ(3u, h2.size());
  EXPECT_EQ(1, h1.top());
  EXPECT_EQ(2, h2.top());

  h1.swap(h2);
  EXPECT_EQ(3u, h1.size());
  EXPECT_EQ(5u, h2.size());
  EXPECT_EQ(2, h1.top());
  EXPECT_EQ(1, h2.top());

  h1.swap(h2);
  EXPECT_EQ(5u, h1.size());
  EXPECT_EQ(3u, h2.size());
  EXPECT_EQ(1, h1.top());
  EXPECT_EQ(2, h2.top());
}

TEST(BinaryHeap, StringType) {
  BinaryHeap<std::string, Less<std::string>> max_heap;
  max_heap.push("apple");
  max_heap.push("zebra");
  max_heap.push("banana");
  EXPECT_EQ("zebra", max_heap.top());
  max_heap.pop();
  EXPECT_EQ("banana", max_heap.top());

  BinaryHeap<std::string, Greater<std::string>> min_heap;
  min_heap.push("apple");
  min_heap.push("zebra");
  min_heap.push("banana");
  EXPECT_EQ("apple", min_heap.top());
}

TEST(BinaryHeap, DuplicateValues) {
  BinaryHeap<int, Less<int>> max_heap{5, 3, 5, 3, 1, 5};
  EXPECT_EQ(6u, max_heap.size());
  EXPECT_EQ(5, max_heap.top());
  max_heap.pop();
  EXPECT_EQ(5, max_heap.top());
  max_heap.pop();
  EXPECT_EQ(5, max_heap.top());
  max_heap.pop();
  EXPECT_EQ(3, max_heap.top());
}

TEST(BinaryHeap, MoveSemantics) {
  BinaryHeap<std::string, Less<std::string>> h;
  std::string s1 = "hello";
  std::string s2 = "world";
  h.push(std::move(s1));
  h.push(std::move(s2));
  EXPECT_EQ("world", h.top());
  EXPECT_TRUE(s1.empty());
  EXPECT_TRUE(s2.empty());
}

TEST(BinaryHeap, LargeData) {
  const int N = 10000;
  BinaryHeap<int, Less<int>> max_heap;
  std::vector<int> ref;
  for (int i = 0; i < N; i++) {
    int val = (i * 17 + 31) % 9973;
    max_heap.push(val);
    ref.push_back(val);
  }
  std::sort(ref.begin(), ref.end(), std::greater<int>());
  EXPECT_EQ(N, static_cast<int>(max_heap.size()));
  for (int i = 0; i < N; i++) {
    EXPECT_EQ(ref[i], max_heap.top());
    max_heap.pop();
  }
  EXPECT_TRUE(max_heap.empty());
}

TEST(BinaryHeap, SingleElement) {
  BinaryHeap<int, Less<int>> max_heap{42};
  EXPECT_EQ(1u, max_heap.size());
  EXPECT_EQ(42, max_heap.top());
  max_heap.pop();
  EXPECT_TRUE(max_heap.empty());

  BinaryHeap<int, Greater<int>> min_heap{42};
  EXPECT_EQ(1u, min_heap.size());
  EXPECT_EQ(42, min_heap.top());
}
