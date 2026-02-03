# Regex 模块

正则表达式支持。

使用前请导入：
```javascript
import std.Regex as Regex;
```

## 方法

### `match(text, pattern)`
检测字符串是否匹配正则。返回 Boolean。

### `replace(text, pattern, replacement)`
替换匹配的内容。

```javascript
let str = "Hello 123";
// 将数字替换为 #
let res = Regex.replace(str, "\\d+", "#"); 
// res = "Hello #"
```