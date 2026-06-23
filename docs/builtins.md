# 📌 内置类型

BxScript 共有 8 种内置类型。每种类型都有原型链支持，可通过 `prototype` 扩展。

## String (字符串)

UTF-8 编码，通过 `u32string` 内部处理 Unicode。支持数字索引访问单个字符。

| 属性/方法 | 描述 |
|:----------|:-----|
| `length` | Unicode 字符数 |
| `[N]` | 索引访问，返回单字符或 `null` |
| `indexOf(s)` | 首次出现位置 |
| `lastIndexOf(s, [from])` | 末次出现位置 |
| `split(sep)` | 按分隔符拆分 |
| `startsWith(s)` | 是否以 `s` 开头 |
| `endsWith(s)` | 是否以 `s` 结尾 |
| `replace(s, r)` | 替换全部匹配 |
| `toLower()` | 转小写 |
| `toUpper()` | 转大写 |
| `trim()` | 去除首尾空白 |
| `charCodeAt(i)` | Unicode 码点 |
| `substr(a, b)` | 截取子串 |
| `String.fromCharCode(...)` | 码点 → 字符串（静态） |

```javascript
"Hello".length;      // 5
"a,b".split(",");    // ["a", "b"]
"HELLO".toLower();   // "hello"
"  hi  ".trim();     // "hi"
```

详见：[String 完整文档](stdlib/string.md)

---

## Number (数值)

双精度浮点数（`double`）。

| 属性/方法 | 描述 |
|:----------|:-----|
| `toFixed(n)` | 保留 n 位小数 |
| `toString()` | 转为字符串 |
| `Number.MAX_VALUE` | 最大值（静态） |
| `Number.MIN_VALUE` | 最小正值（静态） |

```javascript
(3.14159).toFixed(2);  // "3.14"
```

详见：[Number 完整文档](stdlib/number.md)

---

## Array (数组)

可变长度有序集合。

| 属性/方法 | 描述 |
|:----------|:-----|
| `length` | 元素个数 |
| `[N]` | 索引访问（读写），越界设值自动扩展 |
| `push(...)` | 末尾添加，返回长度 |
| `pop()` | 移除末元素 |
| `shift()` | 移除首元素 |
| `unshift(...)` | 开头插入 |
| `concat(...)` | 合并数组 |
| `join([sep])` | 连接为字符串 |
| `indexOf(v, [s])` | 首次出现位置 |
| `lastIndexOf(v)` | 末次出现位置 |
| `slice([s], [e])` | 截取子数组 |
| `remove(i, [n])` | 删除元素 |
| `insert(i, v)` | 插入元素 |
| `Array.isArray(v)` | 类型检测（静态） |

```javascript
[1,2,3].push(4);          // 4
[1,2,3].join(" - ");      // "1 - 2 - 3"
Array.isArray([1,2]);     // true
```

详见：[Array 完整文档](stdlib/array.md)

---

## Object (对象)

键值对集合，支持 getter/setter 和原型链。

| 属性/方法 | 描述 |
|:----------|:-----|
| `.key` / `["key"]` | 属性读写 |
| `Object.keys(obj)` | 获取键数组（静态） |
| `Object.remove(obj, ...)` | 删除属性（静态） |
| `Object.defineGetter(obj, k, fn)` | 定义 getter（静态） |
| `Object.defineSetter(obj, k, fn)` | 定义 setter（静态） |

```javascript
let obj = { name: "Alice", age: 30 };
Object.keys(obj);  // ["name", "age"]
```

详见：[Object 完整文档](stdlib/object.md)

---

## Function (函数)

支持声明、表达式、匿名、闭包。

```javascript
// 函数声明
function add(a, b) { return a + b; }

// 函数表达式
let mul = function(a, b) { return a * b; };

// 闭包
function counter() {
    let n = 0;
    return function() { n++; return n; };
}
```

---

## Boolean (布尔)

`true` 或 `false`。以下值在条件判断中为假：
- `false`
- `null`
- `0`
- `""` (空字符串)
- 空数组 `[]`
- 空对象 `{}`

---

## Null (空值)

表示"无值"。只有一个值：`null`。

```javascript
let x = null;
x == null;  // true
```

---

## Buffer (缓冲区)

固定大小的二进制数据容器，用于 IO/网络原始字节处理。

| 属性/方法 | 描述 |
|:----------|:-----|
| `length` / `size` | 字节数 |
| `[N]` | 索引读写（0-255） |

```javascript
let buf = new Buffer(8);
buf[0] = 65;   // 'A'
buf.length;    // 8
```

详见：[Buffer 完整文档](stdlib/buffer.md)
