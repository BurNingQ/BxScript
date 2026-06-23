# UI 模块

BxScript 强大的原生 Win32 GUI 构建模块。采用声明式、链式调用 API。

**仅 Windows 有效。**

使用前请导入：
```javascript
import std.Win as win;
import std.Dlg as dlg;
```

---

## Dlg (通用对话框)

| 方法 | 返回值 | 描述 |
|:-----|:------|:-----|
| `alert(msg, [title])` | Null | 信息提示框，默认标题"提示" |
| `confirm(msg, [title])` | Bool | 确认对话框，返回是否点击"是" |
| `error(msg, [title])` | Null | 错误提示框，默认标题"错误" |
| `open([title], [filter])` | String | 打开文件对话框，返回路径或空串 |
| `save([title], [filter])` | String | 保存文件对话框，返回路径或空串 |
| `folder([title])` | String | 选择文件夹对话框，返回路径或空串 |

```javascript
let path = dlg.open("选择文件", "文本文件|*.txt");
if (path != "") {
    IO.print("选择了: " + path);
}

if (dlg.confirm("确定要删除吗？")) {
    IO.remove("target.txt");
}
```

---

## Win (控件与窗体)

### 控件工厂函数

所有控件通过 `win.xxx(id)` 创建，返回控件对象，支持链式调用设置属性。

| 函数 | 描述 |
|:-----|:-----|
| `form(id)` | 创建主窗口 |
| `button(id)` | 创建按钮 |
| `label(id)` | 创建文本标签 |
| `input(id)` | 创建单行输入框 |
| `password(id)` | 创建密码输入框 |
| `multiline(id)` | 创建多行文本框 |
| `checkbox(id)` | 创建复选框 |
| `radio(id)` | 创建单选按钮 |
| `select(id)` | 创建下拉选择框 |
| `slider(id)` | 创建滑动条 |
| `progress(id)` | 创建进度条 |
| `image(id)` | 创建图像控件 |
| `list(id)` | 创建列表/表格 |
| `group(id)` | 创建分组容器 |
| `panel(id)` | 创建面板 |

---

### 通用方法

所有控件都支持以下链式方法：

#### 尺寸与位置

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `size(w, h)` | Number, Number | 设置宽度和高度 |
| `width(w)` | Number | 设置宽度 |
| `height(h)` | Number | 设置高度 |
| `pos(x, y)` | Number, Number | 设置绝对坐标 |
| `x(x)` | Number | 设置 X 坐标 |
| `y(y)` | Number | 设置 Y 坐标 |

#### 外观

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `text([str])` | String | 获取/设置控件文本 |
| `font(opt)` | Object | 设置字体：`{fontSize, fontFamily, fontColor, fontBlod}` |
| `bgColor(color)` | Object 或 Hex 字符串 | 设置背景色：`{R, G, B}` 或 `"#RRGGBB"` |
| `align([v])` | String | 文本对齐：`"left"` / `"center"` / `"right"` |

#### 状态控制

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `visible([v])` | Bool | 获取/设置可见性 |
| `disable([v])` | Bool | 获取/设置禁用状态 |
| `show()` | 无 | 显示控件 |
| `hide()` | 无 | 隐藏控件 |

#### 事件

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `on(event, fn)` | String, Function | 注册事件监听 |

支持的 GUI 事件：`click`、`change`、`mouseover`、`mouseleave`、`mousemove`、`mousedown`、`mouseup`、`close`、`create`、`resize`、`paint`、`keyup`、`keydown`、`dbclick`、`focus`、`blur`、`scroll`

---

### Form 专属方法

| 方法 | 描述 |
|:-----|:-----|
| `add(control \| [controls])` | 添加子控件（或控件数组） |
| `center()` | 窗口居中 |
| `icon(path)` | 设置窗口图标 |
| `menu(items)` | 设置菜单栏 |
| `tray(conf)` | 配置系统托盘图标 |
| `doMax()` | 最大化窗口 |
| `doMin()` | 最小化窗口 |
| `doCap()` | 还原窗口 |
| `doTray(conf)` | 设置托盘 |

---

### 特定控件方法

| 控件 | 方法 | 描述 |
|:-----|:-----|:-----|
| Image | `src([path])` | 获取/设置图片路径 |
| List/Select | `items([arr])` | 获取/设置列表项 |
| List | `heads([arr])` | 获取/设置列头 |

---

### 全局方法

| 方法 | 描述 |
|:-----|:-----|
| `loop()` | 进入消息循环（脚本末尾调用） |
| `doExit([code])` | 退出应用 |

---

### 完整示例

```javascript
import std.Win as win;
import std.Dlg as dlg;

// 创建主窗口
let f = win.form("mainForm")
    .size(400, 300)
    .center()
    .text("BxScript App");

// 创建按钮
let btn = win.button("btnSubmit")
    .text("提交")
    .pos(150, 100)
    .size(100, 40)
    .font({fontSize: 14});

// 绑定点击事件
btn.on("click", function() {
    dlg.alert("Hello BxScript!");
});

// 创建标签
let lbl = win.label("lblTitle")
    .text("欢迎使用 BxScript GUI")
    .pos(100, 50)
    .font({fontSize: 16, fontColor: {R: 0, G: 100, B: 200}});

// 添加到窗口
f.add([lbl, btn]);

// 进入消息循环（必须放在最后）
win.loop();
```

## WebView 控件

WebView2 嵌入式浏览器控件：

```javascript
let web = win.webview()
    .title("WebView Demo")
    .size(800, 600)
    .html(`<h1>Hello WebView</h1>`);

// 绑定函数供 JS 调用
web.bind("nativePing", function(msg) {
    IO.print("JS says: " + msg);
    return "pong from C++";
});

// 可选配置
web.transparent();  // 透明背景
web.debug();        // 开启 DevTools

win.loop();
```
