# JSON 模块

提供 JSON 序列化和反序列化功能。

使用前请导入：
```javascript
import std.JSON as JSON;
```

## 方法

### `parse(jsonString)`
将 JSON 字符串解析为 BxScript 对象或数组。

```javascript
import std.JSON as JSON;
import std.IO as IO;
let str = '{"a": 1, "b": [1, 2]}';
let obj = JSON.parse(str);
IO.print(obj.a); // 1
```

### `stringify(value)`
将对象、数组或基础类型转换为 JSON 字符串。

```javascript
import std.JSON as JSON;
import std.IO as IO;
let obj = { x: 100, y: 200 };
let str = JSON.stringify(obj);
IO.print(str); // {"x":100,"y":200}
```