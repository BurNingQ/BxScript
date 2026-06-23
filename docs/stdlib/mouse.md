# Mouse 鼠标

鼠标光标控制与模拟模块。**仅 Windows 有效**。

使用前请导入：
```javascript
import std.Mouse;
```

## 函数

| 函数 | 参数 | 描述 |
|:-----|:-----|:-----|
| `move(x, y)` | 两个 Number | 移动光标到绝对屏幕坐标 |
| `click([x, y])` | 可选坐标 | 在指定位置点击左键，省略则点击当前位置 |
| `down([btn])` | `"left"` / `"right"` / `"middle"` | 按下鼠标按键（不释放），默认左键 |
| `up([btn])` | `"left"` / `"right"` | 释放鼠标按键，默认左键 |
| `scroll([delta])` | Number | 滚动滚轮，正数下滚，负数上滚。默认 -120 |
| `getPos()` | 无 | 获取当前光标坐标 |

### `getPos()` 返回值

| 属性 | 类型 | 描述 |
|:-----|:-----|:-----|
| `x` | Number | X 坐标 |
| `y` | Number | Y 坐标 |

## 示例

```javascript
import std.Mouse;
import std.Screen;
import std.IO as IO;

// 获取当前位置
let pos = Mouse.getPos();
IO.print("当前位置: " + pos.x + ", " + pos.y);

// 移动到 (100, 200) 后点击
Mouse.move(100, 200);
Mouse.click();

// 右键点击
Mouse.down("right");
Mouse.up("right");

// 滚动
Mouse.scroll(-120);  // 向上滚动

// 配合 Screen 模块，移动到屏幕中心并双击
let cx = Screen.width() / 2;
let cy = Screen.height() / 2;
Mouse.move(cx, cy);
Mouse.click();
Mouse.click();
```
