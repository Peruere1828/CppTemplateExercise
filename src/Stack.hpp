#ifndef STACK_HPP
#define STACK_HPP

#include <initializer_list>
#include <utility>

#include "Vector.hpp"

namespace mystd::stack {
template <typename T>
class Stack {
private:
  vector::Vector<T> data_;

public:
  Stack() = default;
  Stack(std::initializer_list<T> init) : data_(init) {}

  [[nodiscard]] auto empty() const -> bool { return data_.empty(); }
  [[nodiscard]] auto size() const -> size_t { return data_.size(); }

  auto top() -> T & {
    if (empty()) {
      throw std::out_of_range("Stack::top on empty stack");
    }
    return data_.back();
  }
  auto top() const -> const T & {
    if (empty()) {
      throw std::out_of_range("Stack::top on empty stack");
    }
    return data_.back();
  }

  template <typename U>
  void push(U &&val) {
    data_.pushBack(std::forward<U>(val));
  }

  template <typename... Args>
  auto emplace(Args &&...args) -> T & {
    return data_.emplaceBack(std::forward<Args>(args)...);
  }

  void pop() {
    if (empty()) {
      throw std::out_of_range("Stack::pop on empty stack");
    }
    data_.popBack();
  }

  void swap(Stack &ano) noexcept { data_.swap(ano.data_); }

  auto operator==(const Stack &ano) const -> bool {
    return data_ == ano.data_;
  }
  auto operator!=(const Stack &ano) const -> bool {
    return data_ != ano.data_;
  }
};
}  // namespace mystd::stack
#endif