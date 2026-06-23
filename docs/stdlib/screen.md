# Screen 屏幕

屏幕信息获取模块。**仅 Windows 有效**。

使用前请导入：
```javascript
import std.Screen;
```

## 函数

| 函数 | 返回值 | 描述 |
|:-----|:------|:-----|
| `width()` | Number | 主显示器宽度（像素） |
| `height()` | Number | 主显示器高度（像素） |
| `workArea()` | Object `{width, height}` | 工作区大小（排除任务栏） |
| `count()` | Number | 显示器数量 |
| `rate()` | Number | 显示器刷新率（Hz） |
| `bpp()` | Number | 颜色深度（bits per pixel） |

## 示例

```javascript
import std.Screen;
import std.IO as IO;

IO.print("屏幕分辨率: " + Screen.width() + "x" + Screen.height());
IO.print("显示器数量: " + Screen.count());
IO.print("刷新率: " + Screen.rate() + "Hz");

let area = Screen.workArea();
IO.print("工作区: " + area.width + "x" + area.height);

// 配合鼠标模块
import std.Mouse;
// 移动鼠标到屏幕中心
Mouse.move(Screen.width() / 2, Screen.height() / 2);
```
