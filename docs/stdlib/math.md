# Math 模块

提供常用数学常数和函数。

使用前请导入：
```javascript
import std.Math as Math;
```

## 常量
*   `Math.PI`: 圆周率 (3.14159...)

## 基础运算

| 方法               | 描述     |
|:-----------------|:-------|
| `abs(x)`         | 绝对值    |
| `ceil(x)`        | 向上取整   |
| `floor(x)`       | 向下取整   |
| `round(x)`       | 四舍五入   |
| `trunc(x)`       | 截断小数部分 |
| `max(x, y, ...)` | 取最大值   |
| `min(x, y, ...)` | 取最小值   |
| `pow(base, exp)` | 指数运算   |
| `sqrt(x)`        | 平方根    |
| `cbrt(x)`        | 立方根    |

## 三角函数
支持 `sin`, `cos`, `tan`, `asin`, `acos`, `atan` 以及双曲函数 `sinh`, `cosh`, `tanh` 等。

## 随机数
*   `random()`: 返回 [0.0, 1.0) 之间的浮点数。
*   `random(min, max)`: 返回 [min, max] 之间的整数。