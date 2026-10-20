# UI (界面与弹窗)

BxScript 的原生 Win32 GUI 模块，采用声明式、链式调用 API。

**仅 Windows 有效。**

> 📖 本文是 **API 速查表**。想了解怎么用、有哪些坑，请先读 → [GUI 开发指南](../gui.md)。

使用前请导入：

```javascript
import std.Win as win;    // 控件与窗体
import std.Dlg as dlg;    // 通用对话框
```

---

## Dlg (通用对话框)

系统对话框均可独立使用，无需先创建窗口。

| 方法 | 返回值 | 描述 |
|:-----|:------|:-----|
| `alert(msg, [title])` | Null | 信息提示框，默认标题"提示" |
| `confirm(msg, [title])` | Bool | 确认对话框，返回是否点击"是" |
| `error(msg, [title])` | Null | 错误提示框，默认标题"错误" |
| `open([title], [filter])` | String | 打开文件对话框，返回路径或空串 |
| `save([title], [filter])` | String | 保存文件对话框，返回路径或空串 |
| `folder([title])` | String | 选择文件夹对话框，返回路径或空串 |

```javascript
import std.Dlg as dlg;
import std.IO as IO;
import std.Console as Console;

let path = dlg.open("选择文件", "文本文件|*.txt");
if (path != "") {
    Console.println("选择了: " + path);
}

if (dlg.confirm("确定要删除吗？")) {
    IO.remove("target.txt");
}
```

`filter` 格式为 `描述|通配符`。

> **文本长度上限**：Windows 消息框最多接受 65535 个字符，超出时底层 `MessageBoxW`
> 会直接返回 0 且不创建窗口（连 `GetLastError` 都是 0），表现为"调用没反应"。
> `CommonDlgs::MsgBox` 会统一截断到 60000 字符并在末尾追加
> `……（内容过长，已截断；原始长度 N 字符）`，`alert`/`confirm`/`error` 都适用。
> 但想看长内容请自己在脚本里截取，例如 `dlg.alert(res.body.substr(0, 500))`。

---

## Win (控件与窗体)

### 工厂函数

所有控件通过 `win.xxx(id)` 创建，返回控件对象，支持链式调用。

| 函数 | 返回 | 描述 |
|:-----|:-----|:-----|
| `form(id)` | Form | 创建主窗口 |
| `button(id)` | Button | 创建按钮 |
| `label(id)` | Label | 创建文本标签 |
| `input(id)` | Edit | 创建单行输入框 |
| `password(id)` | Edit | 创建密码输入框 |
| `multiline(id)` | MultiEdit | 创建多行文本框 |
| `checkbox(id)` | CheckBox | 创建复选框 |
| `radio(id)` | RadioButton | 创建单选按钮 |
| `select(id)` | ComboBox | 创建下拉选择框 |
| `slider(id)` | Slider | 创建滑动条 |
| `progress(id)` | ProgressBar | 创建进度条 |
| `image(id)` | ImageView | 创建图像控件 |
| `list(id)` | ListView | 创建列表/表格 |
| `group(id)` | GroupBox | 创建分组容器 |
| `panel(id)` | Panel | 创建面板 |

> ⚠️ **`id` 是必需的**，不传会返回 `null`，无法继续链式调用。

其余全局成员：

| 函数 | 描述 |
|:-----|:-----|
| `webview()` | 创建 WebView（**不接 id**） |
| `item(text, [children\|click])` | 构造菜单项，见[菜单栏](#菜单栏) |
| `loop()` | 渲染所有窗体并进入消息循环，阻塞直到退出（关窗或 `doExit`）后才返回 |
| `render(form)` | 立即渲染并显示指定窗体，见[多窗体](#多窗体) |
| `doExit([code])` | 退出应用，默认退出码 0 |

---

### 通用方法

所有控件都支持以下链式方法。**setter 均返回控件本身。**

#### 尺寸与位置

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `size(w, h)` / `size(obj)` | Number, Number / Object | 设置宽高（只传一个参数时宽高同值） |
| `width(w)` | Number | 设置宽度 |
| `height(h)` | Number | 设置高度 |
| `pos(x, y)` / `pos(obj)` | Number, Number / Object | 设置绝对坐标 |
| `x(x)` | Number | 设置 X 坐标 |
| `y(y)` | Number | 设置 Y 坐标 |

#### 外观

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `text([str])` | String | 获取/设置控件文本；对 `form` 是窗口标题 |
| `font(obj)` | Object | 设置字体，见下方说明 |
| `bgColor([color])` | Object / String | 获取/设置背景色，见下方说明 |
| `align([v])` | String | ⚠️ **空操作，不生效** |

**字体字段：**

```javascript
lbl.font({
    fontSize: 16,               // ✅ 生效
    fontColor: {R: 0, G: 100, B: 200}   // ✅ 生效
});
// ⚠️ fontFamily 和 fontBlod 被接受但不生效
```

**颜色：**

```javascript
lbl.bgColor({R: 240, G: 240, B: 240});      // ✅ 对象
lbl.bgColor({R: 255, G: 200, B: 0, A: 128}); // ✅ 带透明度
lbl.bgColor("#F0F0F0");                     // ✅ 十六进制
lbl.bgColor("0x2E8B57");                    // ✅ 0x 前缀

let c = lbl.bgColor();                      // 读取，未设置返回 null
```

> ⚠️ `button` 上设置背景色**不会显色**（Win32 标准按钮不响应 `WM_CTLCOLOR*`，需自绘）。`form`、`label`、`input`、`multiline`、`select`、`list` 正常。
> 十六进制支持 `#RRGGBB` / `#RRGGBBAA` / `0xRRGGBB`；三位简写 `"#F00"` 会被解析成黑色。

#### 状态控制

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `visible([v])` | Bool | 获取/设置可见性 |
| `disable([v])` | Bool | 获取/设置禁用状态（`true` = 禁用） |
| `show()` | 无 | 显示控件 |
| `hide()` | 无 | 隐藏控件 |

#### 事件

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `on(event, fn)` | String, Function | 注册事件监听，同名事件后者覆盖前者 |

**合法事件名（17 个）：**

```
click    change    mouseover   mouseleave   mousemove
mousedown  mouseup  close      create       resize
paint    keyup     keydown     dbclick      focus
blur     scroll
```

传非法名称会抛 `参数错误: xxx 不支持`。

> ⚠️ **合法 ≠ 会触发。** 实际绑定情况：
>
> | 事件 | 真正会触发的控件 |
> |:-----|:----------------|
> | `mouseover` `mouseleave` `mousemove` `mousedown` `mouseup` `keydown` `keyup` `scroll` `resize` `create` `close` `paint` | 所有控件 |
> | `click` | **仅** Button、Label、ListView |
> | `change` | **仅** Edit、MultiEdit、Slider、ComboBox、ListView |
> | `dbclick` `focus` `blur` | **从不触发** |
>
> `mousedown` / `mouseup` 只响应**鼠标左键**。

**事件回调参数：**

回调只收到**一个**事件对象参数，字段随事件类型变化：

| 事件 | 字段 |
|:-----|:-----|
| 鼠标类 | `type: "mouse"`, `x`, `y`, `button`, `wheel` |
| `keydown` | `type: "keydown"`, `keyCode`, `scanCode` |
| `keyup` | `type: "keyup"`, `keyCode` |
| `resize` | `type: "resize"`, `width`, `height`, `state` |
| 拖入文件 | `type: "drop"`, `x`, `y`, `files` |
| ListView `click` | `value`（列索引） |
| 其余事件 | **空对象 `{}`** |

> ⚠️ 事件对象**没有 `target`**，回调里也**不能用 `this`**（未绑定，会抛异常）。请用闭包或 `refs` 取控件。
> 回调返回值会被丢弃。

---

### Form 专属方法

| 方法 | 参数 | 描述 |
|:-----|:-----|:-----|
| `add(control \| [controls])` | 控件或数组 | 添加子控件，返回 form 本身 |
| `center()` | 无 | 窗口居中 |
| `icon(path)` | String | 设置窗口图标 |
| `menu(items)` | Array | 设置菜单栏，元素用 `win.item()` 构造 |
| `doTray(conf)` | Object | 配置系统托盘 |
| `doMin()` | 无 | 最小化 |
| `doMax()` | 无 | 最大化 / 还原（**切换**） |
| `doCap()` | 无 | **拖动窗口**（非"还原"） |
| `close()` | 无 | 关闭（销毁）本窗口；模态对话框靠它结束 `render()` 阻塞 |
| `owner(form)` | Form | 指定所有者窗体，见[多窗体](#多窗体) |
| `modal([bool])` | Bool | 标记为模态（默认 `true`），配合 `win.render()` 生效 |
| `refs` | 属性 | 自动收集的子控件索引表，见下 |

> ⚠️ **没有 `tray()` 方法**，托盘是 `doTray(conf)`。
> ⚠️ `doMin` / `doMax` / `doCap` **在渲染前调用会抛异常**，只能写在事件回调里。

**refs：** `add()` 时自动把所有子孙控件按 `id` 收进 `form.refs`。

```javascript
let f = win.form("main").add([
    win.input("inpName").pos(20, 20).size(200, 28)
]);

f.refs.inpName.text("默认值");     // 按 id 取控件
```

---

### 特定控件方法

| 控件 | 方法 | 描述 |
|:-----|:-----|:-----|
| Image | `src([path])` | 设置图片路径，支持本地文件和 `http` 地址 |
| List / Select | `items([arr])` | 获取/设置列表项 |
| List | `heads([arr])` | 获取/设置列头 |

**ListView 专属方法：**

| 方法 | 返回 | 描述 |
|:-----|:-----|:-----|
| `getSelectedIndex()` | Number | 当前选中行，无选中为 -1 |
| `setSelectedIndex(i)` | Null | 选中第 i 行 |
| `getSelectedItem(i)` | 任意 | 第 i 行的原始数据 |

**列头与数据的写法：**

```javascript
f.refs.lstUser.heads([
    { text: "姓名", width: 120 },   // 对象形式可指定列宽，默认 100
    { text: "年龄" }                 // 也可直接用字符串
]).items([
    "单列字符串",                    // 单列行
    ["张三", 28]                    // 多列行用数组
]);
```

> Select（ComboBox）**没有** `getSelectedIndex` / `getSelectedItem`，取当前值请用 `select.text()`。

---

### 菜单栏

用 `win.item()` 构造菜单项，再传给 `f.menu()`。

```javascript
f.menu([
    win.item("文件", [                          // 第二参数传数组 → 子菜单
        win.item("新建", function() { newFile(); }),
        win.item("-"),                          // "-" → 分隔线
        win.item("退出", function() { win.doExit(); })
    ]),
    win.item("关于", function() {                // 第二参数传函数 → 可点击项
        dlg.alert("关于");
    })
]);
```

| 写法 | 含义 |
|:-----|:-----|
| `win.item(text)` | 普通菜单项 |
| `win.item("-")` | 分隔线（**根级分隔线会被忽略**，仅子菜单有效） |
| `win.item(text, [子项数组])` | 子菜单 |
| `win.item(text, 函数)` | 可点击项 |

- 点击回调收到**空事件对象 `{}`**，无法从参数判断点了哪一项，需每项各自绑定函数。
- **不支持快捷键**和菜单图标。
- 可给 item 对象赋 `enabled`（Bool）、`checked`（Bool）两个字段。

---

### 系统托盘

```javascript
f.doTray({
    icon: "tray.ico",       // 必填，否则不显示图标
    tip: "提示文本",         // 悬停提示
    click: function() {     // 点击托盘（收到空对象）
        f.show();
    }
});
```

点击托盘时会先 `show()` 并置前窗口，再触发 `click`。

---

### 多窗体

`win.form()` 每调用一次就登记一个窗口，`loop()` 会全部渲染。

`win.render(form)` 可以**立即**渲染并显示指定窗体（并把它移出待渲染队列），常用于在回调里弹出新窗口：

```javascript
btn.on("click", function() {
    let fAdd = win.form("add").owner(f).modal(true)
        .text("新增").size(400, 350).center().add([...]);
    win.render(fAdd);       // 模态时阻塞到对话框关闭
});
```

**子窗口关系：**

| 方法 | 效果 |
|:-----|:-----|
| `owner(父窗体)` | 子窗压在父窗之上、跟随父窗最小化、**不占任务栏** |
| `modal([bool])` | 弹出后**禁用父窗**，关闭后恢复（默认 `true`） |
| `close()` | 关闭窗口；模态对话框靠它结束 `render()` 的阻塞 |

窗口间切换用 `hide()` / `show()`。`hide()` 只是隐藏，**不会**结束模态阻塞。

> ⚠️ 值必须在子窗**关闭前**取出：窗口销毁后控件一并消失，之后 `refs.xxx.text()` 只能读到空串。
> ⚠️ **关闭最后一个顶层窗口**才退出程序；`f.close()` 只关自己。
> ⚠️ `modal(true)` 建议配合 `owner()` 用；无 owner 时不会报错，但退化成"只阻塞脚本"。
> ⚠️ 模态窗内**没有 Tab 键导航**。
> ⚠️ `render()` 之前父窗必须已渲染出来（在事件回调里 `render(child)` 天然满足）。

---

### WebView 控件

WebView2 嵌入式浏览器控件。**注意 `win.webview()` 不接 id。**

```javascript
let wb = win.webview()
    .title("WebView Demo")      // 默认 "BxScriptWebView"
    .size(800, 600)             // 默认 800 × 600，窗口自动居中
    .html("<h1>Hello WebView</h1>")
    .bind("nativeFunc", function(arg) {
        return "pong: " + arg;  // 返回值自动 JSON 化送回 JS
    });

win.loop();
```

**可用方法：**

| 方法 | 描述 |
|:-----|:-----|
| `title([str])` | 获取/设置窗口标题 |
| `html([str])` | 获取/设置页面内容 |
| `size(w, h)` / `width(w)` / `height(h)` | 尺寸 |
| `bind(name, fn)` | 注册供 JS 调用的函数 |
| `transparent()` | 透明背景 |
| `debug()` | 开启 DevTools（F12） |
| `show()` / `hide()` | 显示/隐藏 |
| `doTray(icon, tip)` 或 `doTray({icon, tip})` | 托盘（**无点击回调**） |
| `doMin()` / `doMax()` / `doCap()` / `doExit()` | 窗口控制 |

**JavaScript ↔ 脚本通信规则：**

```javascript
// 脚本侧
.bind("calc", function(a, b) {
    return a + b;
})
```

```javascript
// JS 侧：返回 Promise，必须 await
let r = await calc(1, 2);
```

- JS 实参按**位置**逐个传入脚本函数。
- 脚本返回值自动 JSON 序列化；无返回或出错时 JS 收到 `null`。
- **JS 侧调用一律返回 Promise。**

> ⚠️ form 与 webview **不能混用**：只要创建过 webview，`win.loop()` 就只跑 webview。

---

## 相关

- 📖 [GUI 开发指南](../gui.md) —— 完整用法、实战示例与已知限制
- 仓库 `examples/snippets/snippets.bx` —— UI 用法最全的示例
