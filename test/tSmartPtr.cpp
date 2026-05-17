#include <gtest/gtest.h>

#include <type_traits>

#include "SmartPtr.hpp"

using mystd::memory::UniquePtr;

namespace TestSmartPtr {
struct Tracker {
  static int alive_count;
  int value;
  Tracker(int v = 0) : value(v) { alive_count++; }
  virtual ~Tracker() { alive_count--; }
};
int Tracker::alive_count = 0;

struct DerivedTracker : Tracker {
  DerivedTracker(int v = 0) : Tracker(v) {}
  ~DerivedTracker() override = default;
};

struct TrueEmptyDeleter {
  void operator()(Tracker* p) const { delete p; }
};
struct CallCountingDeleter {
  int* call_count = nullptr;
  void operator()(Tracker* p) const {
    if (call_count) (*call_count)++;
    delete p;
  }
};
struct StatefulDeleter {
  int state;
  void operator()(Tracker* p) const { delete p; }
};

struct ArrayTracker {
  static int alive_count;
  int value;

  ArrayTracker() : value(0) { alive_count++; }
  ~ArrayTracker() { alive_count--; }

  ArrayTracker(const ArrayTracker&) = delete;
  ArrayTracker& operator=(const ArrayTracker&) = delete;
};
int ArrayTracker::alive_count = 0;

struct BaseTrackerArray {
  virtual ~BaseTrackerArray() = default;
};
struct DerivedTrackerArray : BaseTrackerArray {};
struct TrueEmptyArrayDeleter {
  void operator()(ArrayTracker* p) const { delete[] p; }
};
struct CallCountingArrayDeleter {
  int* call_count = nullptr;
  void operator()(ArrayTracker* p) const {
    if (call_count) (*call_count)++;
    delete[] p;
  }
};
struct MyIntArrayPtr {
  int* p;
  MyIntArrayPtr(int* p = nullptr) : p(p) {}
  MyIntArrayPtr(std::nullptr_t) : p(nullptr) {}
  bool operator==(std::nullptr_t) const { return p == nullptr; }
  bool operator!=(std::nullptr_t) const { return p != nullptr; }
  explicit operator bool() const { return p != nullptr; }
  int& operator[](std::size_t i) const { return p[i]; }
};

struct FancyArrayDeleter {
  using Pointer = MyIntArrayPtr;
  void operator()(MyIntArrayPtr ptr) const { delete[] ptr.p; }
};
struct HasStar {
  template <typename T, typename = decltype(*std::declval<T>())>
  static std::true_type test(int);
  template <typename>
  static std::false_type test(...);
  template <typename UPtr>
  static constexpr bool value = decltype(test<UPtr>(0))::value;
};
};  // namespace TestSmartPtr

using namespace TestSmartPtr;

TEST(UniquePtr, Basic) {
  UniquePtr<int> p1;
  UniquePtr<int> p2(nullptr);
  EXPECT_EQ(true, p1.get() == nullptr);
  EXPECT_EQ(true, p2.get() == nullptr);
  EXPECT_EQ(false, static_cast<bool>(p1));
  UniquePtr<int> p3(new int(42));
  EXPECT_EQ(true, p3.get() != nullptr);
  EXPECT_EQ(true, static_cast<bool>(p3));
  EXPECT_EQ(42, *p3);
  *p3 = 100;
  EXPECT_EQ(100, *p3);
  UniquePtr<Tracker> p4(new Tracker(99));
  EXPECT_EQ(99, p4->value);
}

TEST(UniquePtr, ResetRelease) {
  Tracker::alive_count = 0;
  {
    UniquePtr<Tracker> p1(new Tracker(10));
    EXPECT_EQ(1, Tracker::alive_count);
    Tracker* raw = p1.release();
    EXPECT_EQ(true, p1.get() == nullptr);
    EXPECT_EQ(1, Tracker::alive_count);
    p1.reset(raw);
    EXPECT_EQ(1, Tracker::alive_count);
    EXPECT_EQ(10, p1->value);
    p1.reset(new Tracker(20));
    EXPECT_EQ(1, Tracker::alive_count);
    EXPECT_EQ(20, p1->value);
    p1.reset();
    EXPECT_EQ(0, Tracker::alive_count);
    EXPECT_EQ(true, p1.get() == nullptr);
    p1.reset(new Tracker(30));
    EXPECT_EQ(1, Tracker::alive_count);
    p1 = nullptr;
    EXPECT_EQ(0, Tracker::alive_count);
  }
  EXPECT_EQ(0, Tracker::alive_count);
}

TEST(UniquePtr, MoveSemantics) {
  Tracker::alive_count = 0;
  {
    UniquePtr<Tracker> p1(new Tracker(1));
    UniquePtr<Tracker> p2(std::move(p1));
    EXPECT_EQ(true, p1.get() == nullptr);
    EXPECT_EQ(1, p2->value);
    EXPECT_EQ(1, Tracker::alive_count);
    UniquePtr<Tracker> p3(new Tracker(3));
    p3 = std::move(p2);
    EXPECT_EQ(true, p2.get() == nullptr);
    EXPECT_EQ(1, p3->value);
    EXPECT_EQ(1, Tracker::alive_count);
  }
  EXPECT_EQ(0, Tracker::alive_count);
  {
    UniquePtr<DerivedTracker> p_derived(new DerivedTracker(100));
    UniquePtr<Tracker> p_base(std::move(p_derived));
    EXPECT_EQ(true, p_derived.get() == nullptr);
    EXPECT_EQ(100, p_base->value);
    UniquePtr<DerivedTracker> p_derived2(new DerivedTracker(200));
    p_base = std::move(p_derived2);
    EXPECT_EQ(true, p_derived2.get() == nullptr);
    EXPECT_EQ(200, p_base->value);
    EXPECT_EQ(1, Tracker::alive_count);
    p_base.reset();
    EXPECT_EQ(0, Tracker::alive_count);
  }
}

TEST(UniquePtr, DeletersAndEBCO) {
  EXPECT_EQ(sizeof(void*), sizeof(UniquePtr<Tracker, TrueEmptyDeleter>));
  EXPECT_EQ(true, sizeof(UniquePtr<Tracker, StatefulDeleter>) > sizeof(void*));
  int calls = 0;
  CallCountingDeleter del{&calls};
  {
    UniquePtr<Tracker, CallCountingDeleter&> p1(new Tracker(5), del);
    EXPECT_EQ(sizeof(void*) * 2, sizeof(p1));
  }
  EXPECT_EQ(1, calls);
  {
    CallCountingDeleter del2{&calls};
    UniquePtr<Tracker, CallCountingDeleter&> p2(new Tracker(6), del2);
    UniquePtr<Tracker, CallCountingDeleter&> p3(std::move(p2));
    EXPECT_EQ(true, p2.get() == nullptr);
  }
  EXPECT_EQ(2, calls);
  {
    StatefulDeleter s_del{99};
    UniquePtr<Tracker, StatefulDeleter> p4(new Tracker(7), s_del);
    EXPECT_EQ(99, p4.getDeleter().state);
    UniquePtr<Tracker, StatefulDeleter> p5(std::move(p4));
    EXPECT_EQ(99, p5.getDeleter().state);
  }
}

TEST(UniquePtr, Basic_arr) {
  ArrayTracker::alive_count = 0;
  {
    UniquePtr<ArrayTracker[]> p1;
    UniquePtr<ArrayTracker[]> p2(nullptr);
    EXPECT_EQ(true, p1.get() == nullptr);
    EXPECT_EQ(true, p2.get() == nullptr);
    EXPECT_EQ(false, static_cast<bool>(p1));
    UniquePtr<ArrayTracker[]> p3(new ArrayTracker[5]);
    EXPECT_EQ(true, p3.get() != nullptr);
    EXPECT_EQ(true, static_cast<bool>(p3));
    EXPECT_EQ(5, ArrayTracker::alive_count);
    p3[0].value = 10;
    p3[4].value = 20;
    EXPECT_EQ(10, p3[0].value);
    EXPECT_EQ(20, p3[4].value);
  }
  EXPECT_EQ(0, ArrayTracker::alive_count);
}

TEST(UniquePtr, ResetRelease_arr) {
  ArrayTracker::alive_count = 0;
  {
    UniquePtr<ArrayTracker[]> p1(new ArrayTracker[3]);
    EXPECT_EQ(3, ArrayTracker::alive_count);
    ArrayTracker* raw = p1.release();
    EXPECT_EQ(true, p1.get() == nullptr);
    EXPECT_EQ(3, ArrayTracker::alive_count);
    p1.reset(raw);
    EXPECT_EQ(3, ArrayTracker::alive_count);
    p1.reset(new ArrayTracker[2]);
    EXPECT_EQ(2, ArrayTracker::alive_count);
    p1.reset();
    EXPECT_EQ(0, ArrayTracker::alive_count);
    EXPECT_EQ(true, p1.get() == nullptr);
  }
  EXPECT_EQ(0, ArrayTracker::alive_count);
}

TEST(UniquePtr, MoveSemantics_arr) {
  ArrayTracker::alive_count = 0;
  {
    UniquePtr<ArrayTracker[]> p1(new ArrayTracker[3]);
    p1[0].value = 100;
    UniquePtr<ArrayTracker[]> p2(std::move(p1));
    EXPECT_EQ(true, p1.get() == nullptr);
    EXPECT_EQ(100, p2[0].value);
    EXPECT_EQ(3, ArrayTracker::alive_count);
    UniquePtr<ArrayTracker[]> p3(new ArrayTracker[2]);
    EXPECT_EQ(5, ArrayTracker::alive_count);
    p3 = std::move(p2);
    EXPECT_EQ(true, p2.get() == nullptr);
    EXPECT_EQ(100, p3[0].value);
    EXPECT_EQ(3, ArrayTracker::alive_count);
    p3 = std::move(p3);
    EXPECT_EQ(100, p3[0].value);
    EXPECT_EQ(3, ArrayTracker::alive_count);
  }
  EXPECT_EQ(0, ArrayTracker::alive_count);
}

TEST(UniquePtr, Swap_arr) {
  ArrayTracker::alive_count = 0;
  {
    UniquePtr<ArrayTracker[]> p1(new ArrayTracker[2]);
    p1[0].value = 10;
    UniquePtr<ArrayTracker[]> p2(new ArrayTracker[3]);
    p2[0].value = 20;
    p1.swap(p2);
    EXPECT_EQ(20, p1[0].value);
    EXPECT_EQ(10, p2[0].value);
    EXPECT_EQ(5, ArrayTracker::alive_count);
    mystd::swap(p1, p2);
    EXPECT_EQ(10, p1[0].value);
    EXPECT_EQ(20, p2[0].value);
    EXPECT_EQ(5, ArrayTracker::alive_count);
  }
  EXPECT_EQ(0, ArrayTracker::alive_count);
}

TEST(UniquePtr, SFINAE_arr) {
  using UPtr = mystd::memory::UniquePtr<int[]>;
  EXPECT_EQ(false, (std::is_copy_constructible_v<UPtr>));
  EXPECT_EQ(false, (std::is_copy_assignable_v<UPtr>));
  using ConstUPtr = mystd::memory::UniquePtr<const int[]>;
  EXPECT_EQ(true, (std::is_constructible_v<ConstUPtr, UPtr&&>));
  using BaseUPtr = mystd::memory::UniquePtr<BaseTrackerArray[]>;
  using DerivedUPtr = mystd::memory::UniquePtr<DerivedTrackerArray[]>;
  EXPECT_EQ(false, (std::is_constructible_v<BaseUPtr, DerivedUPtr&&>));
  EXPECT_EQ(false, (std::is_constructible_v<BaseUPtr, DerivedTrackerArray*>));
  using SingleUPtr = mystd::memory::UniquePtr<int>;
  EXPECT_EQ(false, (std::is_constructible_v<UPtr, SingleUPtr&&>));
  EXPECT_EQ(false, (std::is_constructible_v<SingleUPtr, UPtr&&>));
  EXPECT_EQ(false, HasStar::value<UPtr>);
}

TEST(UniquePtr, Advanced_arr) {
  EXPECT_EQ(
      sizeof(void*),
      sizeof(mystd::memory::UniquePtr<ArrayTracker[], TrueEmptyArrayDeleter>));
  int calls = 0;
  CallCountingArrayDeleter del{&calls};
  {
    mystd::memory::UniquePtr<ArrayTracker[], CallCountingArrayDeleter&> p(
        nullptr, del);
    EXPECT_EQ(true, p.get() == nullptr);
  }
  EXPECT_EQ(0, calls);
  {
    mystd::memory::UniquePtr<ArrayTracker[], CallCountingArrayDeleter&> p2(
        new ArrayTracker[2], del);
  }
  EXPECT_EQ(1, calls);
  ArrayTracker::alive_count = 0;
  {
    mystd::memory::UniquePtr<ArrayTracker[]> p_zero(new ArrayTracker[0]);
    EXPECT_EQ(true, p_zero.get() != nullptr);
  }
  EXPECT_EQ(0, ArrayTracker::alive_count);
  {
    MyIntArrayPtr fancy(new int[3]);
    mystd::memory::UniquePtr<int[], FancyArrayDeleter> p_fancy(fancy);
    EXPECT_EQ(true, static_cast<bool>(p_fancy));
    p_fancy.reset(MyIntArrayPtr(new int[2]));
    EXPECT_EQ(true, static_cast<bool>(p_fancy));
  }
}
