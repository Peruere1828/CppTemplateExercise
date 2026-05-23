# Vector vs std::vector

## 已实现的功能

| 功能                     | Vector                          | std::vector                             |
| ------------------------ | ------------------------------- | --------------------------------------- |
| 默认构造                 | `Vector()`                      | `vector()`                              |
| 指定数量构造             | `Vector(count)`                 | `vector(count)`                         |
| 指定数量+值构造          | `Vector(count, val)`            | `vector(count, val)`                    |
| 拷贝构造                 | `Vector(const Vector&)`         | `vector(const vector&)`                 |
| 移动构造                 | `Vector(Vector&&)`              | `vector(vector&&)`                      |
| initializer_list 构造    | `Vector{...}`                   | `vector{...}`                           |
| 析构                     | `~Vector()`                     | `~vector()`                             |
| 拷贝赋值                 | `operator=(const Vector&)`      | `operator=(const vector&)`              |
| 移动赋值                 | `operator=(Vector&&)`           | `operator=(vector&&)`                   |
| initializer_list 赋值    | `operator={...}`                | `operator={...}`                        |
| assign(count, val)       | 有                              | `assign(count, val)`                    |
| assign(initializer_list) | 有                              | `assign(initializer_list)`              |
| operator[]               | `v[i]`                          | `v[i]`                                  |
| at()                     | `v.at(i)` (抛 out_of_range)     | `v.at(i)` (抛 out_of_range)             |
| front()                  | `v.front()`                     | `v.front()`                             |
| back()                   | `v.back()`                      | `v.back()`                              |
| data()                   | `v.data()`                      | `v.data()`                              |
| begin / cbegin           | `v.begin()` → `T*`              | `v.begin()` → 实现定义迭代器            |
| end / cend               | `v.end()` → `T*`                | `v.end()` → 实现定义迭代器              |
| pushBack                 | `v.pushBack(val)` (万能引用)    | `push_back(val)` + `push_back(T&&)`     |
| emplaceBack              | `v.emplaceBack(args...)`        | `emplace_back(args...)`                 |
| insert(单个值)           | `v.insert(pos, val)` (万能引用) | `insert(pos, val)` + `insert(pos, T&&)` |
| insert(count, val)       | `v.insert(pos, count, val)`     | `insert(pos, count, val)`               |
| insert(initializer_list) | `v.insert(pos, {...})`          | `insert(pos, {...})`                    |
| emplace                  | `v.emplace(pos, args...)`       | `emplace(pos, args...)`                 |
| erase                    | `v.erase(pos)`                  | `erase(pos)`                            |
| popBack                  | `v.popBack()`                   | `pop_back()`                            |
| clear                    | `v.clear()`                     | `v.clear()`                             |
| resize                   | `v.resize(new_size)`            | `resize(new_size)`                      |
| resize(val)              | `v.resize(new_size, val)`       | `resize(new_size, val)`                 |
| reserve                  | `v.reserve(cap)`                | `reserve(cap)`                          |
| shrinkToFit              | `v.shrinkToFit()`               | `shrink_to_fit()`                       |
| swap                     | `v.swap(other)`                 | `swap(other)`                           |
| empty                    | `v.empty()`                     | `empty()`                               |
| size                     | `v.size()`                      | `size()`                                |
| capacity                 | `v.capacity()`                  | `capacity()`                            |
| operator== / !=          | 有 (成员函数)                   | C++17 为非成员，C++20 为成员            |

## 未实现的功能

| 功能                                                            | 原因                                        |
| --------------------------------------------------------------- | ------------------------------------------- |
| Allocator 模板参数                                              | 设计决策：不使用 allocator                  |
| `get_allocator()`                                               | 无 allocator                                |
| `max_size()`                                                    | 依赖 allocator                              |
| PMR (`std::pmr::vector`)                                        | 设计决策：不实现                            |
| 反向迭代器 (rbegin/rend/crbegin/crend)                          | 设计决策：不实现                            |
| `assign` / `insert` 的迭代器范围重载                            | 仅提供 count+value 和 initializer_list 版本 |
| 非成员函数 (`<`, `<=`, `>`, `>=`, `std::swap`, `std::erase` 等) | 设计决策：不实现                            |
| 成员类型别名 (value_type, iterator 等)                          | 不在设计范围内                              |

## 行为差异

| 项目                   | Vector                                  | std::vector                     |
| ---------------------- | --------------------------------------- | ------------------------------- |
| 迭代器类型             | 原始指针 `T*` / `const T*`              | 实现定义                        |
| `front()` 空容器       | 抛出 `std::out_of_range`                | 未定义行为                      |
| `back()` 空容器        | 抛出 `std::out_of_range`                | 未定义行为                      |
| `popBack()` 空容器     | 抛出 `std::out_of_range`                | 未定义行为                      |
| `insert(pos)` 越界     | 抛出 `std::out_of_range` (pos > size)   | 未定义行为                      |
| `erase(pos)` 越界      | 抛出 `std::out_of_range` (pos >= size)  | 未定义行为                      |
| 内部结构               | `size_` + `capacity_` + `data_`         | begin/end/end_of_storage 三指针 |
| 内存分配               | `::operator new/delete` + placement new | 通过 `Allocator`                |
| 增长因子               | 固定 2x                                 | 不固定（常见 2x 或 1.5x）       |
| 函数命名               | camelCase                               | snake_case                      |
| `shrinkToFit()` 空容器 | 释放内存，data 置 nullptr               | 实现定义                        |