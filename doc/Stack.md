# Stack vs std::stack

## 已实现的功能

| 功能 | Stack | std::stack |
|------|-------|------------|
| 默认构造 | `Stack()` | `stack()` |
| initializer_list 构造 | `Stack{1, 2, 3}` | 无 (标准 stack 没有此构造) |
| 拷贝构造 | 有 (由 Vector 隐式提供) | 有 |
| 移动构造 | 有 (由 Vector 隐式提供) | 有 |
| 拷贝赋值 | 有 (由 Vector 隐式提供) | 有 |
| 移动赋值 | 有 (由 Vector 隐式提供) | 有 |
| top() | `stk.top()` (可变和 const) | `stk.top()` (可变和 const) |
| push | `stk.push(val)` (万能引用) | `push(val)` + `push(T&&)` |
| emplace | `stk.emplace(args...)` | `emplace(args...)` |
| pop | `stk.pop()` | `pop()` |
| swap | `stk.swap(other)` | `swap(other)` |
| empty | `stk.empty()` | `empty()` |
| size | `stk.size()` | `size()` |
| operator== / != | 有 (成员函数) | C++20 为非成员 |

## 未实现的功能

| 功能 | 原因 |
|------|------|
| Container 模板参数 | 设计决策：底层固定使用 Vector |
| `c` 成员 (底层容器访问) | 不暴露内部容器 |
| 非成员函数 (`<`, `<=`, `>`, `>=`, `std::swap`) | 设计决策：不实现 |
| 成员类型别名 (container_type, value_type 等) | 不在设计范围内 |

## 行为差异

| 项目 | Stack | std::stack |
|------|-------|------------|
| 底层容器 | `Vector<T>` (固定) | 模板参数指定 (默认 `std::deque<T>`) |
| 容器访问 | 无 (private) | `protected` 成员 `c` |
| `top()` 空栈 | 抛出 `std::out_of_range` | 未定义行为 |
| `pop()` 空栈 | 抛出 `std::out_of_range` | 未定义行为 |
| initializer_list 构造 | 支持 | 不支持 |
| `operator==` | 成员函数 | C++20 为非成员 |