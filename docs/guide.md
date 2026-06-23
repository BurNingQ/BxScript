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
import std.IO as IO;
IO.println("Hello, BxScript!");
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

### 控制流

#### if / else
```javascript
let score = 85;
if (score >= 90) {
    IO.print("优秀");
} else if (score >= 60) {
    IO.print("及格");
} else {
    IO.print("不及格");
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
    IO.print(i);
}

// for...in 遍历
let obj = { a: 1, b: 2, c: 3 };
for (let key in obj) {
    IO.print(key + ": " + obj[key]);
}
```

#### while 循环
```javascript
let i = 0;
while (i < 5) {
    IO.print(i);
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
    IO.print("捕获异常: " + e);
} finally {
    IO.print("清理工作");
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
        IO.print(str);  // 最后执行
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
    IO.print("立即执行");
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
| IO | 文件读写、控制台 | `import std.IO as IO` |
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
