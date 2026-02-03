# BxScript 🚀

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Platform](https://img.shields.io/badge/platform-Windows-0078D6.svg)](https://www.microsoft.com/windows)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

> **BxScript** 是一个基于 C++17 开发的、轻量级、专注于 **桌面应用开发** 的嵌入式脚本语言解释器。

> MacOS与Linux部分暂未兼容

> 它的语法设计深受 JavaScript 启发，但在底层深度集成了 Windows 原生 API。通过 BxScript，你可以使用极简的脚本代码构建高性能的 Win32 GUI 应用程序，或是编写系统自动化脚本。它内置了事件循环机制，无需庞大的第三方运行时（如 Electron 或 V8），即可获得现代化的开发体验。

---

## ⚠️ 免责声明与使用限制 (Disclaimer & Restrictions)

**请在使用本项目前仔细阅读以下条款：**

1.  **仅供技术研究**：本项目开源的初衷是用于编程语言原理的学习、技术交流以及合法的软件开发。
2.  **严禁非法用途**：**严禁**将 BxScript 用于任何形式的**灰色产业（灰产）**、**黑客攻击**、**网络诈骗**、**恶意软件编写**或其他违反您所在地法律法规的行为。
3.  **免责条款**：使用者基于本项目开发的任何衍生产品或应用，其产生的一切法律后果由使用者自行承担。开发者（BurNingLi）不对任何因非法使用本项目而导致的损失或法律责任负责。

**如果您无法遵守上述条款，请立即停止使用本项目。**

---

## ✨ 核心特性 (Features)

*   **声明式原生 GUI** 🖥️：
    *   **原生性能**：内置轻量级 Win32 封装库，无需编写繁琐的 C++ 模板代码。
    *   **流式构建**：支持**声明式 UI 构建**，通过链式调用和对象嵌套快速搭建复杂界面。
    *   **现代化特性**：支持高分屏 (DPI Aware)、系统托盘、菜单栏、透明窗体等特性。
*   **WebView2 深度集成** 🌐：
    *   **混合开发**：基于 Edge (Chromium) 内核，支持构建现代化的 Hybrid 应用。
    *   **双向绑定**：实现脚本与网页的高效通信 (Binding)，让 C++ 的系统能力赋能 Web 前端。
*   **异步事件循环 (Event Loop)** ⚡：
    *   **非阻塞设计**：内置微任务/宏任务调度系统，原生支持 `setTimeout`, `setInterval`。
    *   **流畅体验**：网络请求 (`Net`)、文件 IO 等耗时操作均在后台线程执行，回调在主线程触发，保证 UI 界面**永不卡顿**。
*   **全功能标准库** 📦：
    *   **Net**: 封装 WinINet，支持 HTTP/HTTPS 请求 (GET, POST, Multipart)，无需额外依赖 curl。
    *   **Crypt**: 内置 MD5, SHA256, HMAC, Base64, CRC32 等常用加密算法。
    *   **System**: 提供鼠标/键盘模拟 (`Mouse`, `Keyboard` 模块)、屏幕信息获取、进程管理等自动化能力。

---

## 🖥️ 声明式 GUI 构建 (Declarative GUI)

BxScript 使得构建 Windows 桌面应用变得异常简单。你不再需要处理消息循环、句柄或回调函数指针，只需描述你的界面。

### 示例代码

```javascript
// 1. 创建一个主窗口
let app = win.form("apps").size(800, 600)
    .center()         // 屏幕居中
    .icon("app.ico"); // 设置图标

// 2. 创建一个按钮，链式设置属性
let btn = win.button("btn1").text("Click Me")
    .pos(220, 150)
    .size(160, 40)
    .font({size: 14});

// 3. 绑定点击事件 (支持闭包)
btn.on("click", function() {
    win.alert("你好，这是原生弹窗！");
    // 动态修改控件属性
    btn.text("已点击");
});

// 4. 将控件添加到窗口
app.add(btn);

// 5. 入消息循环
win.loop();
```

### 运行效果预览

![GUI Demo](https://via.placeholder.com/600x400.png?text=BxScript+Win32+GUI+Demo)
*(此处为占位图，实际运行将显示原生 Windows 窗口)*

---

## 🌐 WebView2 集成 (Hybrid App)

BxScript 内置了对 WebView2 的支持，允许你使用 HTML/CSS/JS 编写界面，同时调用底层的系统能力。

```javascript
// 创建一个 WebView 窗口
let web = win.webview()
    .size(1024, 768)
    .title("WebView Bridge Demo")
    .html(`
        <h1>Hello from WebView</h1>
        <button onclick="callNative()">Call BxScript</button>
        <script>
            async function callNative() {
                // 直接调用 BxScript 中绑定的函数
                let res = await window.nativeFunc("Data from Web");
                alert("Native response: " + res);
            }
        </script>
    `);

// 绑定原生函数供网页调用
web.bind("nativeFunc", function(arg) {
    print("网页发来数据: " + arg);
    // 执行系统级操作，例如计算文件哈希
    return Crypt.md5(arg);
});

win.loop();
```

---

## ⚙️ 系统自动化与网络 (Automation & Net)

BxScript 非常适合编写系统自动化脚本，内置的非阻塞网络库让它可以轻松处理数据上报。

```javascript
import std.Mouse as mouse;
import std.Screen as screen;
import std.Net as http;

// 1. 模拟自动化操作
let w = screen.width();
let h = screen.height();
// 移动到屏幕中心并点击
mouse.move(w / 2, h / 2);
mouse.click();

print("开始发送网络请求...");

// 2. 异步发送 GET 请求，不会卡住界面
http.get("https://httpbin.org/get", function(res) {
    if (res.status == 200) {
        let json = JSON.parse(res.body);
        print("IP来源: " + json.origin);
    } else {
        print("请求失败: " + res.error);
    }
});

print("请求已发送 (程序继续运行...)");
```

---

## 🛠️ 构建与运行 (Build & Run)

### 环境要求
*   **Windows 10/11** (因深度依赖 Win32 API)
*   C++17 编译器 (MSVC 2019+ 或 MinGW64)
*   CMake 3.20+

### 编译步骤

```bash
mkdir cmake-build-release
cd cmake-build-release
cmake ..
cmake --build . --config Release
```

### 运行脚本

```bash
# 运行脚本文件
./BxScript app.bx

# 进入交互式模式 (REPL)
./BxScript
>> import std.Math;
>> print(Math.random());
```

---

## 🗺️ 未来规划 (Roadmap)

*   **UI 组件库扩充**: 增加 TreeView, TabControl, RichEdit 等高级控件。
*   **布局系统增强**: 引入 Flexbox 或 Grid 布局引擎，替代绝对定位。
*   **内存管理优化**: 引入简单的标记-清除 (Mark-and-Sweep) GC，解决循环引用问题。
*   **包管理器**: 建立类似 npm 的简单的包管理机制，方便分发脚本库。

---

## 🤝 贡献 (Contributing)
| Gemini 3 Pro  |
|---------------|
| 👨🏻‍🎓 执行器导师 |
| ‍🎓 最佳C++导师   |
| 🎅 WIN32导师    |
欢迎提交 Issue 或 Pull Request！
本项目由 **BurNingLi** 与 **Gemini3Pro** 共同开发，同时也感谢 **Google Gemini** 在 Win32 API 移植与重构过程中提供的 AI 辅助支持。

## 📄 开源协议 (License)
本项目采用 MIT License 开源。

[![Docs](https://img.shields.io/badge/docs-read%20online-blue)](https://burningq.github.io/BxScript/)

> 📚 **详细文档与 API 参考手册已上线：** [https://burningq.github.io/BxScript/](https://burningq.github.io/BxScript/)