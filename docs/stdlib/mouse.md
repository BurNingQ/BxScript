使用前请导入：
```javascript
import std.Mouse;
import std.Screen;
```

## Mouse (鼠标模拟)
仅 Windows 有效。
*   `move(x, y)`: 移动鼠标。
*   `click([x, y])`: 点击鼠标左键。
*   `down([btn])`: 按下鼠标 (`"left"`, `"right"`).
*   `up([btn])`: 抬起鼠标。
*   `scroll(delta)`: 滚动滚轮。
*   `pos()`: 获取当前坐标 `{x, y}`。