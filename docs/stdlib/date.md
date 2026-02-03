# Date 模块

日期和时间处理。

使用前请导入：
```javascript
import std.Date;
```

## 方法

### `now()`
获取当前时间对象。

```javascript
import std.IO as IO;
let d = Date.now();
IO.print(d.year + "-" + d.month + "-" + d.day);
```

### `from(timestamp | dateString)`
从时间戳（毫秒）或日期字符串创建对象。
*   支持格式：`yyyy-MM-dd HH:mm:ss`

### `format(pattern)`
**注：此方法是 Date 对象实例的方法**。
格式化日期。占位符：`yyyy`, `MM`, `dd`, `HH`, `mm`, `ss`。

```javascript
let d = Date.now();
IO.print(d.format("yyyy年MM月dd日 HH:mm:ss"));
```

## Date 对象属性
*   `timestamp`: 时间戳 (ms)
*   `year`, `month`, `day`, `hour`, `minute`, `second`