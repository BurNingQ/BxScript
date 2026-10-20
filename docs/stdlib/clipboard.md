# Clipboard 剪贴板

系统剪贴板读写模块。**仅 Windows 有效**（Unicode 文本）。

使用前请导入：
```javascript
import std.Clipboard as CB;
import std.Console as Console;
```

## 函数

### `getText()`
读取剪贴板中的文本内容。

| 返回值 | 描述 |
|:-------|:-----|
| String / Null | 剪贴板文本，不可用时返回 `null` |

```javascript
import std.Clipboard as CB;
import std.Console as Console;
let text = CB.getText();
if (text) {
    Console.print("剪贴板内容: " + text);
}
```

### `setText(text)`
将文本写入剪贴板。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `text` | String | 要写入的文本 |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否成功写入 |

```javascript
CB.setText("Hello BxScript!");  // 将文本复制到剪贴板
```

### `clear()`
清空剪贴板。

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否成功清空 |

```javascript
CB.clear();
```
