# 🔧 其他介绍

## IIFE 模式

立即执行函数表达式（Immediately Invoked Function Expression），用于创建独立作用域：

```javascript
import std.IO as io;
(function(){
    io.println("run");
})();
```

## defer (LIFO 延迟执行)

`defer` 是 BxScript 独创特性，灵感来自 Go 语言。`defer` 注册的回调在函数退出时按 **后进先出 (LIFO)** 顺序执行，无论函数是正常 `return` 还是因异常退出。

```javascript
import std.IO as io;
function deferTest() {
    let str = "1";
    str += "2";
    defer (function(){
        str += "3";
        io.println(str);
    })();
}
deferTest();
// 输出: 123
```

**注意：** `defer` 必须紧接着一个函数调用：`defer (function(){ ... })();`

## 高阶函数

BxScript 函数是一等公民，可作为参数传递或作为返回值：

```javascript
function buildFunc(){
    return function(a, b){
        return a + b;
    }
}
let fn = buildFunc();
fn(3, 4);  // 7

// 简写
buildFunc()(3, 4);  // 7
```

## 原型扩展（Prototype Extension）

所有内置类型都有 `prototype` 对象，可通过它扩展方法：

```javascript
String.prototype.sayHi = function() {
    return "Hi, " + this;
};
"fox".sayHi();  // 输出: Hi, fox

Array.prototype.first = function() {
    return this[0];
};
[1, 2, 3].first();  // 1
```

## 并发模型

### EventLoop 机制

BxScript 内置事件循环，负责：
- **定时器回调**：`setTimeout`、`setInterval` 到期后由事件循环分发
- **网络请求回调**：HTTP 请求在后台线程执行，完成后将回调排入事件循环
- **GUI 事件**：窗口消息（点击、输入等）作为事件分发给控件回调
- **Thread 消息**：`Thread.postMessage()` 将消息排入事件循环，由 `Thread.onMessage()` 接收

所有回调均在**主线程**执行，无需担心锁问题，但长时间运行的回调会阻塞 UI。

```javascript
import std.Timer as Timer;

IO.print("开始");

// setTimeout: 单次延迟回调
Timer.setTimeout(function() {
    IO.print("1 秒后执行");
}, 1000);

// setInterval: 重复回调
let id = Timer.setInterval(function() {
    IO.print("每秒执行一次");
}, 1000);

// 取消定时器
Timer.clear(id);
```

### Thread 模块与消息传递

```javascript
import std.Thread as Thread;
import std.IO as IO;

// 主线程注册消息接收
Thread.onMessage(function(msg) {
    IO.print("收到子线程消息: " + msg);
});

// 在子线程执行耗时操作
Thread.invoke(function() {
    Thread.sleep(1000);
    // 向主线程发送消息
    Thread.postMessage("任务完成");
});

IO.print("主线程继续运行...");
```

### 网络异步回调

```javascript
import std.Net as Net;
import std.IO as IO;

// 异步 GET 请求，不阻塞 UI
Net.get("https://api.example.com/data", function(res) {
    if (res.status == 200) {
        IO.print("收到数据: " + res.body);
    }
});

IO.print("请求已发送，程序继续执行...");
```

## 原型链与继承

BxScript 对象的 `prototype` 支持原型链查找。当访问对象上不存在的属性时，会沿着原型链向上查找：

```javascript
let animal = { speak: function() { return "..."; } };
let dog = { name: "Buddy" };

// 设置原型（底层机制，可通过 Object 模块操作）
// dog 继承 animal 的方法
```
