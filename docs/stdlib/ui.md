# UI 模块

BxScript 强大的原生 GUI 构建模块。包含 `std.Win` (主窗体) 和 `std.Dlg` (通用对话框)。

使用前请导入：
```javascript
import std.Win as win; // 推荐别名
import std.Dlg as dlg;
```

## Dlg (通用对话框)

| 方法                        | 描述                       |
|:--------------------------|:-------------------------|
| `alert(msg, [title])`     | 显示提示框。                   |
| `confirm(msg, [title])`   | 显示确认框，返回 `true`/`false`。 |
| `open([title], [filter])` | 打开文件选择框。                 |
| `save([title], [filter])` | 打开保存文件框。                 |
| `folder([title])`         | 打开文件夹选择框。                |

## Win (控件与窗体)

### 核心方法
*   `loop()`: 进入消息循环（必须在脚本末尾调用）。
*   `doExit([code])`: 退出应用程序。
*   `doMax()`: 最大化
*   `doMin()`: 最小化
*   `doCap()`: 拖拽

### 控件创建
采用链式调用设置属性。

#### `form(title, width, height)`
创建主窗口。
*   方法: `add(control)`, `show()`, `center()`, `menu(arr)`, `tray(obj)`.

#### `button(text)`
创建按钮。
*   事件: `click`

#### `label(text)`
创建文本标签。

#### `input(text)`
创建单行输入框。
*   属性: `text()` 获取或设置内容。

#### `webview()`
创建 WebView2 浏览器控件。
*   `html(str)`: 设置 HTML 内容。
*   `bind(name, func)`: 绑定 C++ 函数供 JS 调用。
*   `transparnet()`: 开启透明窗口
*   `debug()`: 开启调试模式
*   `size(width, height)`: 设置窗口大小
*   `width(wid)`: 设置宽度
*   `height(hei)`: 设置高度
*   `doMax()`: 最大化
*   `doMin()`: 最小化
*   `doCap()`: 拖拽
*   `doExit([code])`: 退出应用程序。

# 🛠️ 通用函数 API 手册

### 📐 空间与定位
> **尺寸设置**
> - `size(width, height)`: 设置窗口或控件的整体大小
> - `width(width)`: 独立设置宽度
> - `height(height)`: 独立设置高度
>
> **坐标定位**
> - `pos(x, y)`: 设置绝对定位坐标
> - `x(x)`: 设置 X 轴数值
> - `y(y)`: 设置 Y 轴数值

---

### 🎨 视觉与样式
> **外观定制**
> - `font({fontSize, fontFamily, fontColor})`: 设置字体细节
    >   - *示例：`{fontSize: 12, fontFamily: "msyh", fontColor: {R:125,G:125,B:121}}`*
> - `bgColor({R, G, B})`: 设置背景颜色
> - `text(txt)`: 修改控件显示的文字或标题内容

---

### ⚙️ 逻辑与状态
> **交互控制**
> - `on("event", fn)`: 注册事件监听（如 `"click"`）
> - `disable(bool)`: 切换禁用/启用状态
> - `visible(bool)`: 切换可见/隐藏状态
>
> **显隐操作**
> - `show()`: 立即显示控件
> - `hide()`: 立即隐藏控件

### 示例

```javascript
let f = win.form("formId").size(800, 600).center();
let b = win.button("btn1").pos(10, 10);
f.add(b);

b.on("click", function() {
    f.refs.btn1.text("Clicked");
    dlg.alert("Clicked!");
});

win.loop();
```