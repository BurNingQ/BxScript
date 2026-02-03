# Thread 模块

提供多线程支持。

使用前请导入：
```javascript
import std.Thread as Thread;
```

## 方法

### `sleep(ms)`
使当前线程休眠指定毫秒数。

### `invoke(func, [args...])`
启动一个新的线程执行函数。

```javascript
import std.IO as IO;
import std.Thread as Thread;
Thread.invoke(function(name) {
    IO.print("Hello from thread: " + name);
}, "Worker-1");
```

### 线程通信
*   `onMessage(callback)`: 注册主线程消息接收回调。
*   `postMessage(data...)`: 从子线程向主线程发送消息。

```javascript
// 在主线程
Thread.onMessage(function(msg) {
    IO.print("收到子线程消息: " + msg);
});

Thread.invoke(function() {
    Thread.sleep(1000);
    Thread.postMessage("任务完成");
});
```