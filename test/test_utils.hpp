#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <cstddef>
#include <random>
#include <type_traits>

// NOLINTBEGIN
class RandomGenerator {
private:
  std::mt19937 rng_;

public:
  RandomGenerator() : rng_(std::random_device{}()) {}

  explicit RandomGenerator(std::mt19937::result_type seed) : rng_(seed) {}

  auto operator()() -> size_t {
    std::uniform_int_distribution<size_t> dist(0, SIZE_MAX);
    return dist(rng_);
  }

  // 生成指定范围的随机整数
  template <class A, class B>
  auto uniform_int(A min, B max) -> std::common_type_t<A, B> {
    using T = std::common_type_t<A, B>;
    static_assert(std::is_integral_v<T>);
    std::uniform_int_distribution<T> dist(static_cast<T>(min),
                                          static_cast<T>(max));
    return dist(rng_);
  }

  // 生成指定范围的随机浮点数
  template <class A, class B>
  auto uniform_real(A min, B max) -> double {
    static_assert(std::is_arithmetic_v<A> && std::is_arithmetic_v<B>);
    std::uniform_real_distribution<double> dist(static_cast<double>(min),
                                                static_cast<double>(max));
    return dist(rng_);
  }

  // 生成随机布尔值
  auto bernoulli(double p = 0.5) -> bool {
    std::bernoulli_distribution dist(p);
    return dist(rng_);
  }

  auto engine() -> std::mt19937& { return rng_; }
};
// NOLINTEND

#endif  // TEST_UTILS_HPP