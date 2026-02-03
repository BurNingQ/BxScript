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
Type 'exit' or 'quit' to leave.
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

for (let i = 0; i < 5; i++) {
    IO.print(i + " ");
}
```

通过命令行运行：
```bash
./BxScript hello.bx
```

## 📖 基础语法速览

### 变量与控制流
```javascript
import std.IO as IO;
let count = 0;
while (count < 10) {
    count++;
    if (count % 2 == 0) continue;
    IO.print(count); // 输出奇数
}
```

### 函数与闭包
```javascript
import std.IO as IO;
function createGreeter(name) {
    return function() {
        return "Hello, " + name;
    };
}

let greetWorld = createGreeter("World");
IO.print(greetWorld()); // "Hello, World"
```

### 模块导入
```javascript
import std.IO as IO;
import std.Math as Math; // 导入标准库
import std.Json as J; // 导入并重命名

IO.print(Math.random());
```