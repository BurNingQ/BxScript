# Console 控制台

控制台窗口操作与输入输出模块。

使用前请导入：
```javascript
import std.Console as Console;
```

## 函数

### `print(arg1, arg2, ...)`
向标准输出打印内容（不换行），多个参数以空格分隔。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `arg...` | any | 要打印的值 |

```javascript
Console.print("Hello");
Console.print(" ", "World");  // "Hello World"（同一行）
```

### `println(arg1, arg2, ...)`
向标准输出打印内容并换行。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `arg...` | any | 要打印的值 |

```javascript
Console.println("Line 1");
Console.println("Line 2");
// 输出：
// Line 1
// Line 2
```

### `input([prompt])`
从标准输入读取一行文本。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `prompt` | String | 可选，输入提示 |

| 返回值 | 描述 |
|:-------|:-----|
| String | 用户输入的内容 |

```javascript
let name = Console.input("请输入名字: ");
Console.println("你好, " + name);
```

### `clear()`
清除控制台屏幕（ANSI 转义序列）。

```javascript
Console.clear();
```

### `show()`
显示控制台窗口。**仅 Windows 有效**。

```javascript
Console.show();
```

### `hide()`
隐藏控制台窗口。**仅 Windows 有效**。

```javascript
Console.hide();
```

### `title(text)`
设置控制台窗口标题。**仅 Windows 有效**。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `text` | String | 窗口标题 |

```javascript
Console.title("BxScript Console");
```
