# OS 模块

提供操作系统级别的交互功能。

使用前请导入：
```javascript
import std.OS as OS;
```

## 方法

### `exec(command)`
执行 shell 命令并返回输出结果（阻塞）。

```javascript
import std.OS as OS;
import std.IO as IO;
let output = OS.exec("ipconfig");
IO.print(output);
```

### `getEnv(name)`
获取环境变量的值。

```javascript
import std.OS as OS;
let path = OS.getEnv("PATH");
```

### `platform`
属性，返回当前操作系统名称 ("windows" / "linux")。