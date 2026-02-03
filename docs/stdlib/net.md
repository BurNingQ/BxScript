# Net 模块

提供 HTTP 客户端功能 (基于 WinINet)。所有请求均为**异步非阻塞**。

使用前请导入：
```javascript
import std.Net as Net;
```

## HTTP 请求

所有方法均接受回调函数作为最后一个参数。

### `get(url, [headers], callback)`
发送 GET 请求。

```javascript
import std.IO as IO;
import std.Net as Net;
Net.get("https://api.github.com", function(res) {
    if (res.status == 200) {
        IO.print(res.body);
    }
});
```

### `post(url, data, [headers], callback)`
发送 POST 请求。`data` 可以是字符串、Buffer 或 对象（自动识别 Content-Type）。

```javascript
import std.IO as IO;
import std.Net as Net;
let data = { username: "admin", password: "123" };
Net.post("http://localhost/login", data, function(res) {
    IO.print("Login status: " + res.status);
});
```

### 其他方法
*   `put(url, data, ...)`
*   `delete(url, ...)`
*   `patch(url, data, ...)`

## 响应对象结构
回调函数接收到的 `res` 对象包含：
*   `status`: HTTP 状态码 (Number)
*   `body`: 响应内容 (String 或 Buffer，根据 Content-Type 自动判断)
*   `error`: 错误信息 (String，成功为 null)