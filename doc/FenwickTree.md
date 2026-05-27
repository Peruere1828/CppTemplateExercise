# `FenwickTree` 模板使用文档

## 类模板定义

```cpp
template <typename T,
          T (*e)(),
          T (*op)(T, T),
          T (*opInv)(T, T)>
class FenwickTree;
```

### 模板参数含义

| 参数    | 类型               | 含义                                               |
| ------- | ------------------ | -------------------------------------------------- |
| `T`     | 值类型             | 树状数组中存储的元素类型（如整数和、异或值等）     |
| `e`     | 函数指针 `T()`     | 单位元（`op(x, e()) = x`）                         |
| `op`    | 函数指针 `T(T, T)` | 结合操作（如加法、异或、乘法等），用于前缀和累加   |
| `opInv` | 函数指针 `T(T, T)` | 逆操作，`opInv(op(x, y), y) = x`，用于区间查询求差 |

### 模板参数说明

- `op` 需要满足**结合律**，不需要满足交换律（树状数组内部保证正确的前缀累加顺序）
- `opInv` 用于区间查询：`query(L, R) = opInv(query(R), query(L-1))`
  - 例如加法操作对应减法、异或操作对应异或本身（异或的逆运算是自身）、乘法对应除法

## 接口说明

| 方法                                     | 功能                                                    | 时间复杂度  |
| ---------------------------------------- | ------------------------------------------------------- | ----------- |
| `FenwickTree(size_t n)`                  | 构造长度为 $n$ 的树（初值 `e()`）                       | $O(n)$      |
| `FenwickTree(const std::vector<T> &arr)` | 从数组初始化树                                          | $O(n)$      |
| `void apply(size_t p, T val)`            | 单点累加 `val`（`tree[p] = op(tree[p], val)`）          | $O(\log n)$ |
| `T query(ptrdiff_t p)`                   | 查询前缀和 `[0, p]`，`p = -1` 时返回 `e()`              | $O(\log n)$ |
| `T query(size_t L, size_t R)`            | 查询区间 `[L, R]` 的和                                  | $O(\log n)$ |

## 行为说明

- 所有下标**从 $0$ 开始**，内部实现平移到从 $1$ 开始存储
- 前缀和 `query(ptrdiff_t p)` 返回 `op(a[0], a[1], ..., a[p])`，当 $p = -1$ 时返回 `e()`
- 区间查询 `query(L, R)` = `opInv(query(R), query(L-1))`
- `apply(p, val)` 执行 `a[p] = op(a[p], val)`，然后向上更新所有受影响的前缀和节点
- 所有越界访问抛出 `std::out_of_range`

## 使用案例

### 单点加法、区间求和

```cpp
const int mod = 1e9 + 7;
auto e = []() { return 0; };
auto add = [](int x, int y) { return (x + y) % mod; };
auto sub = [](int x, int y) { return ((x - y) % mod + mod) % mod; };

FenwickTree<int, e, add, sub> tree(10);      // 长度为 10，初值全 0
tree.apply(0, 5);                             // a[0] += 5
tree.apply(3, 2);                             // a[3] += 2
tree.query(0);                                // 查询前缀和 [0, 0] → 5
tree.query(0, 3);                             // 查询区间和 [0, 3] → 7
```

### 单点异或、区间异或

异或的逆运算是自身，因此 `op` 与 `opInv` 相同：

```cpp
auto e = []() { return 0; };
auto xor_op = [](int x, int y) { return x ^ y; };

std::vector<int> arr = {1, 2, 3, 4, 5};
FenwickTree<int, e, xor_op, xor_op> tree(arr);
tree.query(0, 2);    // 1 ^ 2 ^ 3 = 0
tree.query(1, 4);    // 2 ^ 3 ^ 4 ^ 5 = 0
tree.apply(2, 1);    // a[2] ^= 1
tree.query(0, 2);    // 1 ^ 2 ^ 4 = 7
```

## 与标准库的差异

`std` 中**没有**直接对应的 `FenwickTree` / Fenwick tree 标准模板。树状数组是竞赛编程中常用的自定义数据结构，此处实现的 `FenwickTree` 与常见的竞赛用模板保持一致。

| 方面        | `mystd::FenwickTree`      | 常见竞赛模板                                 |
| ----------- | ------------------------- | -------------------------------------------- |
| 模板参数    | 函数指针                  | 函数对象 / lambda（C++20 前作为模板参数）    |
| 下标        | $0$-based，内部平移为 $1$ | 通常 $1$-based                               |
| 单点 modify | `apply(p, val)` 执行累加  | 通常命名为 `add(p, val)` 或 `update(p, val)` |
| 区间查询    | 支持 `query(L, R)`        | 通常直接调用两次前缀和                       |
| 容器        | 内部使用 `std::vector<T>` | 通常 `std::vector<T>` 或数组                 |

更为详细的使用案例请参考 `test/tFenwickTree.cpp`。