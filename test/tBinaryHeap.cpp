#include <gtest/gtest.h>

#include <string>

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
