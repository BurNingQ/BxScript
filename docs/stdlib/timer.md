# Timer 模块

定时器功能。

使用前请导入：
```javascript
import std.Timer as Timer;
```

## 方法

### `setTimeout(callback, delayMs)`
在指定延迟后执行一次回调。返回 timerId。

### `setInterval(callback, intervalMs)`
每隔指定时间重复执行回调。返回 timerId。

### `clear(timerId)`
取消定时器。