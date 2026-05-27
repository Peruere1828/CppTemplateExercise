#ifndef LINKLIST_HPP
#define LINKLIST_HPP

#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace mystd::linklist {

template <typename T>
class LinkList {
private:
  struct Node {
    T val_;
    Node *nxt_, *pre_;

    template <typename U>
    explicit Node(U &&val)
        : val_(std::forward<U>(val)), nxt_(nullptr), pre_(nullptr) {}
  };
  Node *head_, *tail_;
  size_t len_ = 0;

  auto getNodeAt(size_t ind) -> Node * {
    if (ind >= len_) {
      return nullptr;
    }
    Node *cur;
    if (ind < len_ / 2) {
      cur = head_;
      for (size_t i = 0; i < ind; i++) {
        cur = cur->nxt_;
      }
    } else {
      cur = tail_;
      for (size_t i = len_ - 1; i > ind; i--) {
        cur = cur->pre_;
      }
    }
    return cur;
  }
  auto getNodeAt(size_t ind) const -> const Node * {
    if (ind >= len_) {
      return nullptr;
    }
    const Node *cur;
    if (ind < len_ / 2) {
      cur = head_;
      for (size_t i = 0; i < ind; i++) {
        cur = cur->nxt_;
      }
    } else {
      cur = tail_;
      for (size_t i = len_ - 1; i > ind; i--) {
        cur = cur->pre_;
      }
    }
    return cur;
  }

public:
  template <bool IsConst>
  class IteratorBase {
    friend class LinkList;

  private:
    using NodePtr = std::conditional_t<IsConst, const Node *, Node *>;
    NodePtr cur_;

  public:
    explicit IteratorBase(NodePtr cur = nullptr) : cur_(cur) {}
    // 允许 Iterator -> ConstIterator 的隐式转换
    template <bool OtherConst,
              std::enable_if_t<IsConst && !OtherConst, int> = 0>
    IteratorBase(const IteratorBase<OtherConst> &other)
        : cur_(other.cur_) {}
    auto operator*() const -> std::conditional_t<IsConst, const T &, T &> {
      return cur_->val_;
    }
    auto operator->() const
        -> std::conditional_t<IsConst, const T *, T *> {
      return &(cur_->val_);
    }

    auto operator++() -> IteratorBase & {
      cur_ = cur_->nxt_;
      return *this;
    }
    auto operator++(int) -> IteratorBase {
      IteratorBase temp(cur_);
      cur_ = cur_->nxt_;
      return temp;
    }
    auto operator--() -> IteratorBase & {
      cur_ = cur_->pre_;
      return *this;
    }
    auto operator--(int) -> IteratorBase {
      IteratorBase temp(cur_);
      cur_ = cur_->pre_;
      return temp;
    }

    auto operator==(const IteratorBase &other) const -> bool {
      return cur_ == other.cur_;
    }
    auto operator!=(const IteratorBase &other) const -> bool {
      return cur_ != other.cur_;
    }
  };

  using Iterator = IteratorBase<false>;
  using ConstIterator = IteratorBase<true>;

  auto begin() -> Iterator { return Iterator(head_); }
  auto end() -> Iterator { return Iterator(); }
  auto begin() const -> ConstIterator { return ConstIterator(head_); }
  auto end() const -> ConstIterator { return ConstIterator(); }
  auto cbegin() const -> ConstIterator { return ConstIterator(head_); }
  auto cend() const -> ConstIterator { return ConstIterator(); }

  [[nodiscard]] auto empty() const noexcept -> bool { return len_ == 0; }
  [[nodiscard]] auto size() const noexcept -> size_t { return len_; }
  [[nodiscard]] auto ssize() const noexcept -> ptrdiff_t {
    return static_cast<ptrdiff_t>(len_);
  }

  LinkList() noexcept : head_(nullptr), tail_(nullptr) {}
  ~LinkList() { clear(); }
  LinkList(const LinkList &other) : head_(nullptr), tail_(nullptr) {
    Node *cur = other.head_;
    while (cur != nullptr) {
      pushBack(cur->val_);
      cur = cur->nxt_;
    }
  }
  LinkList(LinkList &&other) noexcept
      : head_(other.head_), tail_(other.tail_), len_(other.len_) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.len_ = 0;
  }

  auto operator=(const LinkList &other) -> LinkList & {
    if (this == &other) {
      return *this;
    }
    clear();
    Node *cur = other.head_;
    while (cur != nullptr) {
      pushBack(cur->val_);
      cur = cur->nxt_;
    }
    return *this;
  }
  auto operator=(LinkList &&other) noexcept -> LinkList & {
    if (this == &other) {
      return *this;
    }
    clear();
    head_ = other.head_;
    tail_ = other.tail_;
    len_ = other.len_;
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.len_ = 0;
    return *this;
  }

  auto operator[](size_t ind) -> T & {
    Node *node = getNodeAt(ind);
    if (node == nullptr) {
      throw std::out_of_range("LinkList::operator[] index out of range");
    }
    return node->val_;
  }
  auto operator[](size_t ind) const -> const T & {
    const Node *node = getNodeAt(ind);
    if (node == nullptr) {
      throw std::out_of_range("LinkList::operator[] index out of range");
    }
    return node->val_;
  }

  void clear() {
    Node *cur = head_;
    while (cur != nullptr) {
      Node *tmp = cur->nxt_;
      delete cur;
      cur = tmp;
    }
    head_ = tail_ = nullptr;
    len_ = 0;
  }

  template <typename U>
  auto pushBack(U &&val) -> Iterator {
    Node *new_node = new Node(std::forward<U>(val));
    len_++;
    if (head_ == nullptr) {
      head_ = tail_ = new_node;
      return Iterator(new_node);
    }
    tail_->nxt_ = new_node;
    new_node->pre_ = tail_;
    tail_ = new_node;
    return Iterator(new_node);
  }
  template <typename U>
  auto pushFront(U &&val) -> Iterator {
    Node *new_node = new Node(std::forward<U>(val));
    len_++;
    if (head_ == nullptr) {
      head_ = tail_ = new_node;
      return Iterator(new_node);
    }
    head_->pre_ = new_node;
    new_node->nxt_ = head_;
    head_ = new_node;
    return Iterator(new_node);
  }

  void popBack() {
    if (empty()) {
      throw std::out_of_range("LinkList::pop_back called on empty list");
    }
    len_--;
    if (head_ == tail_) {
      delete tail_;
      head_ = tail_ = nullptr;
      return;
    }
    Node *tmp = tail_;
    tail_ = tail_->pre_;
    tail_->nxt_ = nullptr;
    delete tmp;
  }
  void popFront() {
    if (empty()) {
      throw std::out_of_range("LinkList::pop_front called on empty list");
    }
    len_--;
    if (head_ == tail_) {
      delete head_;
      head_ = tail_ = nullptr;
      return;
    }
    Node *tmp = head_;
    head_ = head_->nxt_;
    head_->pre_ = nullptr;
    delete tmp;
  }

  void erase(size_t ind) {
    if (ind >= len_) {
      throw std::out_of_range("LinkList::erase index out of range");
    }
    if (ind == 0) {
      return popFront();
    }
    if (ind == len_ - 1) {
      return popBack();
    }

    Node *cur = getNodeAt(ind);
    cur->pre_->nxt_ = cur->nxt_;
    cur->nxt_->pre_ = cur->pre_;
    delete cur;
    len_--;
  }

  template <typename U>
  auto insert(size_t ind, U &&val) -> Iterator {
    if (ind > len_) {
      throw std::out_of_range("LinkList::insert index out of range");
    }
    if (ind == 0) {
      return pushFront(std::forward<U>(val));
    }
    if (ind == len_) {
      return pushBack(std::forward<U>(val));
    }

    Node *cur = getNodeAt(ind);
    Node *new_node = new Node(std::forward<U>(val));
    new_node->pre_ = cur->pre_;
    new_node->nxt_ = cur;
    cur->pre_->nxt_ = new_node;
    cur->pre_ = new_node;
    len_++;
    return Iterator(new_node);
  }

  template <typename U>
  auto insert(Iterator iter, U &&val) -> Iterator {
    if (iter == begin()) {
      return pushFront(std::forward<U>(val));
    }
    if (iter == end()) {
      return pushBack(std::forward<U>(val));
    }

    Node *cur = iter.cur_;
    Node *new_node = new Node(std::forward<U>(val));
    new_node->pre_ = cur->pre_;
    new_node->nxt_ = cur;
    cur->pre_->nxt_ = new_node;
    cur->pre_ = new_node;
    len_++;
    return Iterator(new_node);
  }

  auto find(const T &val) const -> ptrdiff_t {
    Node *cur = head_;
    ptrdiff_t ind = 0;
    while (cur != nullptr) {
      if (cur->val_ == val) {
        return ind;
      }
      cur = cur->nxt_;
      ind++;
    }
    return -1;
  }

  /// DEBUG: 左移运算符重载仅用于调试
  friend auto operator<<(std::ostream &oss,
                         const LinkList &lst) -> std::ostream & {
    oss << "[";
    Node *cur = lst.head_;
    while (cur != nullptr) {
      oss << cur->val_;
      if (cur->nxt_ != nullptr) {
        oss << ", ";
      }
      cur = cur->nxt_;
    }
    oss << "]";
    return oss;
  }
};
}  // namespace mystd::linklist

#endif  // LINKLIST_HPP