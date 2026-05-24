# BinaryHeap vs std::priority_queue

## 已实现的功能

| 功能                  | BinaryHeap                                         | std::priority_queue                  |
| --------------------- | -------------------------------------------------- | ------------------------------------ |
| 默认构造              | `BinaryHeap()`                                     | `priority_queue()`                   |
| 比较器构造            | `BinaryHeap(comp)`（explicit）                     | `priority_queue(comp)`（explicit）   |
| initializer_list 构造 | `BinaryHeap{1, 2, 3}` + `BinaryHeap({1, 2}, comp)` | 无（C++23 引入但无比较器版本）       |
| 拷贝构造              | 有（由 Vector 隐式提供）                           | 有                                   |
| 移动构造              | 有（由 Vector 隐式提供）                           | 有                                   |
| 拷贝赋值              | 有（由 Vector 隐式提供）                           | 有                                   |
| 移动赋值              | 有（由 Vector 隐式提供）                           | 有                                   |
| top()                 | `heap.top()`（返回 `const T&`）                    | `pq.top()`（返回 `const_reference`） |
| push                  | `heap.push(val)`（万能引用，单模板）               | `push(const T&)` + `push(T&&)`       |
| emplace               | `heap.emplace(args...)`                            | `emplace(args...)`                   |
| pop                   | `heap.pop()`                                       | `pop()`                              |
| swap                  | `heap.swap(other)`                                 | `swap(other)`                        |
| empty                 | `heap.empty()`                                     | `empty()`                            |
| size                  | `heap.size()`                                      | `size()`                             |

## 未实现的功能

| 功能                                       | 原因                          |
| ------------------------------------------ | ----------------------------- |
| Container 模板参数                         | 设计决策：底层固定使用 Vector |
| 迭代器范围构造                             | 不在设计范围内                |
| 成员类型别名（value_type, size_type 等）   | 不在设计范围内                |
| `push_range`（C++23）                      | 不在设计范围内                |
| `c` / `comp` 成员（底层容器/比较器访问）   | 不暴露内部成员                |
| 非成员函数（`operator==` 等，`std::swap`） | 设计决策：不实现              |
| Allocator 支持                             | 设计决策：不实现              |

## 行为差异

| 项目                  | BinaryHeap                   | std::priority_queue                          |
| --------------------- | ---------------------------- | -------------------------------------------- |
| 底层容器              | `Vector<T>`（固定）          | 模板参数指定（默认 `std::vector<T>`）        |
| 容器访问              | 无（private）                | `protected` 成员 `c`                         |
| 比较器访问            | 无（private）                | `protected` 成员 `comp`                      |
| `top()` 空堆          | 抛出 `std::out_of_range`     | 未定义行为                                   |
| `pop()` 空堆          | 抛出 `std::out_of_range`     | 未定义行为                                   |
| initializer_list 构造 | 支持，可选传入比较器         | 不支持（C++23 标准引入后也不支持传入比较器） |
| 默认堆序              | 大根堆（Compare = Less<T>）  | 大根堆（Compare = std::less<T>）             |
| push 实现             | 万能引用（单模板，覆盖所有） | 两个独立重载（`const T&` + `T&&`）           |

## 实现说明

BinaryHeap 基于 `mystd::Vector` 实现，采用数组存储的完全二叉树结构：

- 节点 $i$ 的左子节点为 $2i+1$，右子节点为 $2i+2$，父节点为 $\lfloor (i-1)/2 \rfloor$
- `push` / `emplace` 将元素插入末尾后执行 `siftUp`
- `pop` 将堆顶与末尾元素交换后移除末尾，再执行 `siftDown`
- `initializer_list` 构造采用 Floyd 建堆法，时间复杂度 $O(n)$