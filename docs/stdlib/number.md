# Number 数值

BxScript 数值为双精度浮点数（`double`），支持整数和小数运算。

## 属性

| 属性 | 类型 | 描述 |
|:-----|:-----|:-----|
| `Number.MAX_VALUE` | Number | 可表示的最大值 |
| `Number.MIN_VALUE` | Number | 可表示的最小正值 |

## 方法

### `toFixed(precision)`
格式化为指定小数位数的字符串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `precision` | Number | 小数位数（0-100） |

| 返回值 | 描述 |
|:-------|:-----|
| String | 格式化后的字符串 |

```javascript
let n = 3.14159;
n.toFixed(2);  // "3.14"
n.toFixed(0);  // "3"
```

### `toString()`
将数值转为字符串。

```javascript
(123).toString();    // "123"
(3.14).toString();   // "3.14"
```

## 示例

```javascript
let a = 100;
let b = 3.14;
let c = a + b;       // 103.14
let d = a / 3;       // 33.333333...

// 配合 Math 模块
import std.Math as Math;
Math.round(b);       // 3
Math.floor(b);       // 3
Math.ceil(b);        // 4
```
