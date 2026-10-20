# Keyboard 键盘

键盘模拟与全局热键模块。**仅 Windows 有效**。

使用前请导入：
```javascript
import std.KeyBoard as KB;
import std.Console as Console;
```

## 键名常量

键名支持大小写不敏感的字符串或虚拟键码。常用键名：

| 键名 | 说明 |
|:-----|:-----|
| `A`-`Z`, `0`-`9` | 字母和数字键 |
| `F1`-`F12` | 功能键 |
| `SHIFT`, `LCTRL`, `RCTRL`, `LALT`, `RALT` | 修饰键 |
| `ENTER`, `TAB`, `ESC`, `SPACE` | 控制键 |
| `LEFT`, `UP`, `RIGHT`, `DOWN` | 方向键 |
| `HOME`, `END`, `PGUP`, `PGDN` | 导航键 |
| `INS`, `DEL`, `PRTSCR` | 编辑键 |
| `NUMLK`, `SCRLK`, `CAPSLK` | 锁定键 |

## 函数

### `press(key1, key2, ...)`
依次按下并释放每个键（按下 → 延时 → 释放 → 延时）。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `key...` | String 或 Number | 要按下的键名或虚拟键码 |

```javascript
import std.KeyBoard as KB;
import std.Console as Console;
KB.press("A", "B", "C");  // 依次输入 A B C
KB.press("ENTER");        // 按下回车
```

### `down(key1, key2, ...)`
依次按下每个键（不释放）。

```javascript
KB.down("LCTRL", "C");  // 按下 Ctrl+C
```

### `up(key1, key2, ...)`
依次释放每个键。

```javascript
KB.up("C", "LCTRL");  // 释放 Ctrl+C
```

### `combine(key1, key2, ...)`
组合键：全部按下 → 停留 → 逆序释放。适用于快捷键。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `key...` | String 或 Number | 要组合的键名 |

```javascript
KB.combine("LCTRL", "C");  // Ctrl+C 复制
KB.combine("LALT", "F4");  // Alt+F4
```

### `sendString(text, [delayMs])`
模拟键盘输入 Unicode 字符串。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `text` | String | 要输入的文本 |
| `delayMs` | Number | 可选，字符间延时（毫秒） |

```javascript
KB.sendString("Hello World!");
KB.sendString("慢速输入", 100);  // 每字间隔 100ms
```

### `getState(key)`
获取切换键的开关状态。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `key` | String 或 Number | 键名（如 `CAPSLK`、`NUMLK`） |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否处于开启状态 |

```javascript
if (KB.getState("CAPSLK")) {
    Console.print("大写锁定已开启");
}
```

### `isPressed(key)`
检测按键是否正在被按住。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `key` | String 或 Number | 键名 |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否处于按住状态 |

```javascript
if (KB.isPressed("SHIFT")) {
    Console.print("Shift 正在被按住");
}
```

### `wait(key)`
阻塞等待按键完成一次完整的"按下→释放"周期。

```javascript
KB.wait("ENTER");  // 等待回车键按下并释放
Console.print("回车被按下");
```

### `block([enable])`
阻塞/取消阻塞所有键盘和鼠标输入。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `enable` | Bool | 可选，`true` 阻塞输入，`false` 解阻塞。默认 `true` |

```javascript
KB.block();      // 锁定输入
// ... 执行自动化操作 ...
KB.block(false); // 解锁输入
```

### `hotkey(keys, handler)`
注册全局热键。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `keys` | String | 热键组合，如 `"CTRL+H"` |
| `handler` | Function | 回调函数 |

| 返回值 | 描述 |
|:-------|:-----|
| Number | 热键 ID，用于注销 |

```javascript
let id = KB.hotkey("CTRL+SHIFT+K", function() {
    Console.print("全局热键被触发！");
});
```

### `unhotkey(id)`
注销全局热键。

| 参数 | 类型 | 描述 |
|:-----|:-----|:-----|
| `id` | Number | 热键 ID |

| 返回值 | 描述 |
|:-------|:-----|
| Bool | 是否成功注销 |

```javascript
KB.unhotkey(id);
```
