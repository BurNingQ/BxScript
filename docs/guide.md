# ⚡ 快速开始

欢迎使用 BxScript！这是一个轻量级的嵌入式脚本语言。本指南将帮助你快速搭建环境并运行你的第一行代码。

## 🛠️ 构建与安装

### 环境要求
*   **Windows 10/11** (推荐) 或 Linux/macOS (部分 GUI 功能受限)
*   C++17 编译器 (MSVC 2019+, GCC 8+, Clang 10+)
*   CMake 3.20+

### 编译步骤

```bash
# 1. 克隆仓库
git clone https://github.com/BurNingQ/BxScript.git
cd BxScript

# 2. 创建构建目录
mkdir cmake-build-release
cd cmake-build-release

# 3. 编译 (Release模式)
cmake ..
cmake --build . --config Release
```

## 🚀 运行代码

### 交互式模式 (REPL)
直接运行 `BxScript` 可执行文件进入交互模式：

```bash
./BxScript
BxScript v1.0.0
>> let a = 10;
>> let b = 20;
>> a + b;
30
```

### 运行脚本文件
创建一个名为 `hello.bx` 的文件：

```javascript
// hello.bx
import std.Console as Console;
Console.println("Hello, BxScript!");
```

通过命令行运行：
```bash
./BxScript hello.bx
```

## 📖 基础语法速览

### 变量声明

使用 `let` 声明变量：

```javascript
let name = "BxScript";
let count = 0;
let flag = true;
let obj = { x: 1, y: 2 };
let arr = [1, 2, 3];
```

语句可以用 `;` 结束，也可以直接**换行**结束（`}` 和文件结束同样终止语句）。但同一行内不能省略分隔符：`let a = 1 a += 1`、`1 2;`、`def a = 1;` 都会报语法错误。

### 字符串

字符串**只能用双引号**（不支持单引号）。

```javascript
let s = "hello";
```

**支持跨行**，行尾换行会原样保留，写起来适合内联 HTML 这类多行内容：

```javascript
let page = "
<div class=\"box\">
    <h1>标题</h1>
</div>
";
```

**转义序列**：`\n`（换行）、`\r`（回车）、`\t`（制表符）、`\\`（反斜杠）、`\"`（双引号）。

```javascript
let s = "第一行\n第二行";   // 与写成两行等价
```

> 📌 跨行写法里的换行是**真实换行字符**，和 `\n` 产生的完全一样。所以内联 `<script>` 时不会被拼成一行。

### 数字

十进制整数和小数直接用字面量写：

```javascript
let a = 42;
let b = 3.14;
```

**支持三种进制前缀**（大小写都行）：

| 写法 | 进制 | 例 | 值 |
|:-----|:-----|:---|:---|
| `0x` / `0X` | 十六进制 | `0xFF` | 255 |
| `0b` / `0B` | 二进制 | `0b1010` | 10 |
| `0o` / `0O` | 八进制 | `0o17` | 15 |

进制字面量就是普通数字，可以正常参与运算：

```javascript
0x41 + 1;          // 66
0xFF - 0x0F;       // 240
0b1 + 0o7 + 0x10;  // 24（可以混用）

// 二进制数据操作里最好用（Buffer 的字节值）
Buffer.from([0x41, 0x42]);   // 两个字节：'A' 'B'
```

> ⚠️ **不支持科学计数法和下划线分隔**。`1e3` 会**静默变成 `1`**（不是 1000），`1_000` 会变成 `1`——写的时候避开。
> 前缀后面没有合法数字（如 `0x`、`0b2`）会直接报错。

### 控制流

#### if / else
```javascript
let score = 85;
if (score >= 90) {
    Console.print("优秀");
} else if (score >= 60) {
    Console.print("及格");
} else {
    Console.print("不及格");
}
```

#### 三元运算
```javascript
let result = score > 60 ? "通过" : "不通过";
let max = a > b ? a : b;
```

#### for 循环
```javascript
// C 风格 for
for (let i = 0; i < 5; i++) {
    Console.print(i);
}

// for...in 遍历
let obj = { a: 1, b: 2, c: 3 };
for (let key in obj) {
    Console.print(key + ": " + obj[key]);
}
```

`for...in` 支持三种可遍历值：

| 遍历对象 | 每轮得到 | 例 |
|:---------|:---------|:---|
| 对象 `{a:1,b:2}` | **键名**（String） | `"a"`, `"b"` |
| 数组 `[7,8,9]` | **下标**（Number） | `0`, `1`, `2` |
| 字符串 `"abc"` | **下标**（Number） | `0`, `1`, `2` |

```javascript
let arr = [7, 8, 9];
for (let i in arr) {
    Console.print(i + " -> " + arr[i]);   // 0 -> 7, 1 -> 8, 2 -> 9
}

// 字符串按 Unicode 码点索引，中文也算一个字符
for (let i in "中文") {
    Console.print(i + "=" + "中文"[i]);    // 0=中, 1=文
}
```

循环体里可以用 `break` / `continue` / `return`（`return` 会直接结束所在函数）。

> ⚠️ 遍历**对象**时键的顺序是**不确定的**（底层是哈希表，不是 JS 那样的插入序）。需要固定顺序就改用数组。
> ⚠️ 遍历其他类型（数字、`null` 等）会抛运行时错误：`for-in 只能遍历对象、数组或字符串`。

#### while 循环
```javascript
let i = 0;
while (i < 5) {
    Console.print(i);
    i++;
}
```

### 运算符

#### 算术运算符
| 运算符 | 描述 | 示例 |
|:-------|:-----|:-----|
| `+` | 加法 / 字符串拼接 | `1 + 2`, `"a" + "b"` |
| `-` | 减法 | `5 - 3` |
| `*` | 乘法 | `2 * 3` |
| `/` | 除法 | `10 / 3` |
| `%` | 取余 | `10 % 3` |

#### 比较运算符
| 运算符 | 描述 |
|:-------|:-----|
| `==` | 等于 |
| `!=` | 不等于 |
| `<` | 小于 |
| `>` | 大于 |
| `<=` | 小于等于 |
| `>=` | 大于等于 |

> ⚠️ 本语言**只有 `==` / `!=`，没有 `===` / `!==`**。写 `===` 会直接报语法错误
> （`不支持的运算符: '==='`），不会静默失效。
> 另外 `==` 不做隐式类型转换，`"1" == 1` 是 `false`。
#### 逻辑运算符
| 运算符 | 描述 |
|:-------|:-----|
| `&&` | 逻辑与（短路） |
| `\|\|` | 逻辑或（短路） |
| `!` | 逻辑非 |

#### 自增/自减
```javascript
let a = 1;
a++;     // 后置自增，返回 1，a 变为 2
++a;     // 前置自增，返回 3，a 变为 3
a--;     // 后置自减
--a;     // 前置自减
```

#### 复合赋值
```javascript
let x = 10;
x += 5;   // x = 15
x -= 3;   // x = 12
x *= 2;   // x = 24
x /= 4;   // x = 6
x %= 4;   // x = 2
```

### 异常处理

```javascript
try {
    throw "something wrong";
} catch (e) {
    Console.print("捕获异常: " + e);
} finally {
    Console.print("清理工作");
}
```

### defer (延迟执行)

`defer` 为 BxScript 独创特性，在函数退出时按 LIFO（后进先出）顺序执行：

```javascript
function test() {
    let str = "1";
    str += "2";
    defer (function() {
        str += "3";
        Console.print(str);  // 最后执行
    })();
    defer (function() {
        str += "4";     // 倒数第二执行
    })();
    return str;  // "124"
}
test();  // 先输出 "1243"，再输出 "12434"
```

### 函数与闭包

```javascript
// 函数声明
function add(a, b) { return a + b; }

// 函数表达式（匿名函数）
let mul = function(a, b) { return a * b; };

// 闭包：内层函数捕获外层变量
function makeCounter() {
    let n = 0;
    return function() {
        n++;
        return n;
    };
}
let c = makeCounter();
c();  // 1
c();  // 2
```

`function` 声明会**提升**：顶层和每个 `{}` 块都会先绑定块内声明，所以任何作用域都能先调用后定义。同名重复声明一律后者生效。

变量这边则是**宽松**的：同一作用域重复 `let` 不报错（后者覆盖）；读取未声明的变量返回 `null`，给未声明的变量赋值会在全局作用域创建它 —— 拼错变量名不会报错。

### 高阶函数

函数可以作为参数传递和返回值：

```javascript
function apply(fn, a, b) {
    return fn(a, b);
}
apply(function(x, y) { return x + y; }, 3, 4);  // 7
```

### IIFE (立即执行函数)

```javascript
(function() {
    Console.print("立即执行");
})();
```

### 原型扩展

通过 `prototype` 扩展内置类型：

```javascript
String.prototype.sayHi = function() {
    return "Hi, " + this;
};
"fox".sayHi();  // "Hi, fox"
```

### 模块导入

```javascript
import std.IO as IO;
import std.Math as Math;
import std.JSON as JSON;

// 导入自定义模块（.bx 文件）
import ./utils as utils;
```

## 📦 标准库一览

| 模块 | 用途 | 导入 |
|:-----|:-----|:-----|
| IO | 文件读写 | `import std.IO as IO` |
| Math | 数学运算 | `import std.Math as Math` |
| Net | HTTP 网络请求 | `import std.Net as Net` |
| JSON | JSON 解析 | `import std.JSON as JSON` |
| Crypt | 加密哈希 | `import std.Crypt as Crypt` |
| OS | 系统调用 | `import std.OS as OS` |
| Date | 日期时间 | `import std.Date` |
| Thread | 多线程 | `import std.Thread as Thread` |
| Timer | 定时器 | `import std.Timer as Timer` |
| Regex | 正则表达式 | `import std.Regex as Regex` |
| Win | GUI 界面 | `import std.Win as win` |
| Dlg | 对话框 | `import std.Dlg as dlg` |
| Screen | 屏幕信息 | `import std.Screen` |
| Mouse | 鼠标模拟 | `import std.Mouse` |
| KeyBoard | 键盘模拟 | `import std.KeyBoard as KB` |
| Clipboard | 剪贴板 | `import std.Clipboard as CB` |
| Type | 类型检测 | `import std.Type as Type` |
| Console | 控制台操作 | `import std.Console as Console` |

详见：[内置类型说明](builtins.md) | [标准库文档](stdlib/io.md)
