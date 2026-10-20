# 🖥️ GUI 开发指南

用 BxScript 构建原生 Win32 桌面应用。

> 本文是**使用指南**，讲怎么把界面搭起来、事件怎么接、有哪些坑。
> 逐个 API 的速查表请见 → [UI 模块参考](stdlib/ui.md)。

> ⚠️ **仅 Windows 有效。** GUI 深度依赖 Win32 API，macOS / Linux 暂不支持。

---

## 最小可运行示例

```javascript
import std.Win as win;
import std.Dlg as dlg;

// 1. 建窗口（必须给 id）
let f = win.form("main")
    .text("我的第一个应用")   // 对 form 来说，text 就是窗口标题
    .size(400, 300)
    .center();

// 2. 建控件，链式设置属性
let btn = win.button("btnHello")
    .text("点我")
    .pos(150, 120)
    .size(100, 36);

// 3. 绑定事件
btn.on("click", function() {
    dlg.alert("Hello BxScript!");
});

// 4. 把控件挂到窗口上
f.add(btn);

// 5. 进入消息循环（所有界面代码都必须在它之前）
win.loop();
```

跑起来就是：一个居中的 400×300 窗口，中间一个按钮，点了弹提示框。

---

## 一、界面是怎么跑起来的

BxScript 的 GUI 是**声明式 + 延迟渲染**的：你在脚本里描述界面，直到 `win.loop()` 才真正建窗。

```
win.form(id)          创建窗口对象（此时还没建原生窗口）
   ↓
win.button(id) ...    创建控件对象，设置属性（都只写在对象上）
   ↓
f.add(...)            把控件挂到窗口的 children 上
   ↓
win.loop()            真正渲染 → 建原生窗口 → 进入消息循环（阻塞到退出）
```

理解这一点很重要，它解释了两个常见困惑：

**① 属性是"渲染时"才生效的。**
`win.loop()` 之前，`.text()` 这类取值返回的是内存里的默认值（`""`、`0`、`true`），不是真实控件值。渲染之后再读才准。

**② `win.loop()` 会阻塞到消息循环结束，然后才继续往下执行。**
循环里的代码在 `win.loop()` 之前全部执行完毕后，界面才真正建出来（所以**所有初始化都要放在 `loop()` 之前**）；`loop()` 本身会一直阻塞，直到消息循环收到退出消息——用户关闭窗口，或脚本里调用了 `win.doExit()`。

两者都会让 `loop()` 正常返回，**它后面的代码会继续执行**，可以拿来做收尾（保存数据、写日志等）：

```javascript
win.loop();
Console.println("界面已关闭，开始收尾");   // 用户关窗或 doExit 之后会执行
```

### 动态修改界面

渲染之后改属性是立即生效的（走 hook 直接作用于原生控件）：

```javascript
btn.on("click", function() {
    btn.text("已点击");        // 立即刷新按钮文字
    btn.disable(true);        // 立即禁用
    f.refs.lblStatus.text("操作完成");
});
```

---

## 二、创建控件

所有控件都是 `win.xxx(id)` 工厂函数，**id 是必需的**，不传会返回 `null`，后面就没法链式调用了。

| 函数 | 控件 |
|:-----|:-----|
| `win.form(id)` | 主窗口 |
| `win.button(id)` | 按钮 |
| `win.label(id)` | 文本标签 |
| `win.input(id)` | 单行输入框 |
| `win.password(id)` | 密码输入框 |
| `win.multiline(id)` | 多行文本框 |
| `win.checkbox(id)` | 复选框 |
| `win.radio(id)` | 单选按钮 |
| `win.select(id)` | 下拉选择框 |
| `win.slider(id)` | 滑动条 |
| `win.progress(id)` | 进度条 |
| `win.image(id)` | 图像控件 |
| `win.list(id)` | 列表 / 表格（ListView） |
| `win.group(id)` | 分组容器 |
| `win.panel(id)` | 面板 |

> **唯一的例外是 `win.webview()`** —— 它不接 id，见[第十一节](#十一webview-混合开发)。

### 用 refs 访问控件

`f.add()` 的时候，窗口会自动把所有子孙控件的 `id` 收进一个 `f.refs` 对象：

```javascript
let f = win.form("main").size(400, 300).add([
    win.input("inpName").pos(20, 20).size(200, 30),
    win.button("btnOk").pos(240, 20).text("确定")
]);

// 之后用 id 取回控件，不用到处存变量
f.refs.inpName.text("默认值");
f.refs.btnOk.on("click", function() {
    showName(f.refs.inpName.text());
});
```

这是**推荐做法**——界面一多，到处传控件变量会很乱。

### 批量添加

`add()` 既接受单个控件，也接受数组，配合链式可以一行建完整个窗口：

```javascript
let f = win.form("main").text("用户信息").size(420, 260).center().add([
    win.label("lbl1").text("姓名：").pos(20, 30),
    win.input("inpName").pos(80, 27).size(300, 28),
    win.label("lbl2").text("备注：").pos(20, 70),
    win.multiline("inpNote").pos(80, 67).size(300, 100),
    win.button("btnSave").text("保存").pos(300, 200).size(80, 32)
]);
win.loop();
```

---

## 三、链式属性设置

所有控件都支持同一套链式方法，**setter 都返回控件本身**，所以可以一直点下去。

### 尺寸与位置

```javascript
.size(400, 300)     // 宽, 高
.pos(20, 30)        // x, y
.width(200)         // 只设宽
.height(100)        // 只设高
.x(20).y(30)        // 分开设
```

也支持对象写法：

```javascript
.size({width: 400, height: 300})
.pos({x: 20, y: 30})
```

> `.size(300)` 只传一个数时，**宽高都会设成 300**。

### 文本

```javascript
label.text("用户名")        // 设置
let s = label.text();       // 读取（渲染后才是真实值）
```

对 `form` 而言，`text()` 是**窗口标题**。

### 颜色

背景色 `bgColor()` 支持两种写法：

```javascript
// ① 对象（0-255），可省略 A（默认 255）
lbl.bgColor({R: 240, G: 240, B: 240});
lbl.bgColor({R: 255, G: 200, B: 0, A: 128});

// ② 十六进制字符串
lbl.bgColor("#F0F0F0");
lbl.bgColor("0x2E8B57");
```

读取当前背景色（未设置时返回 `null`）：

```javascript
let c = lbl.bgColor();
// c.R / c.G / c.B / c.A
```

> ⚠️ **背景色不是所有控件都能显出来。** 它走 `WM_CTLCOLORSTATIC/EDIT/LISTBOX` 和窗体的 `WM_ERASEBKGND` 绘制，所以 **`form`、`label`、`input`、`multiline`、`select`、`list`** 有效；**`button` 无效**（Win32 标准按钮不响应此消息，需要自绘）。给按钮设背景色不会报错，但看不到效果。

> 📌 十六进制支持 `#RRGGBB` / `#RRGGBBAA` / `0xRRGGBB`。**三位简写如 `"#F00"` 不支持**，会被解析成黑色。

### 字体

```javascript
lbl.font({
    fontSize: 16,
    fontColor: {R: 0, G: 100, B: 200}
});
```

> ⚠️ **只有 `fontSize` 和 `fontColor` 真正生效。**
> `fontFamily`（字体名）和 `fontBlod`（加粗，源码就是这个拼写）目前会被接受但不产生任何效果。

### 显示与状态

```javascript
btn.hide();             // 隐藏
btn.show();             // 显示
btn.visible(false);     // 同上，等价写法
btn.disable(true);      // 禁用
btn.visible();          // 读取可见性 → Bool
btn.disable();          // 读取禁用状态 → Bool
```

### 不要用 align

`.align("center")` 这类调用**目前是个空操作**——属性会被存下来，但渲染器从不读取它，任何值都不会产生效果。文本对齐请靠控件尺寸和文本内容来控制。

---

## 四、事件处理

### 基本写法

```javascript
btn.on("click", function(e) {
    // e 是事件对象
});
```

一个控件同一个事件只能绑一个处理函数，**后绑的会覆盖先绑的**：

```javascript
btn.on("click", handlerA);
btn.on("click", handlerB);   // handlerA 被覆盖，不会执行
```

### 哪些事件真正会触发

合法事件名有 17 个，但**并非每个控件都能触发每一个**：

| 事件 | 哪些控件能触发 |
|:-----|:--------------|
| `mouseover` `mouseleave` `mousemove` `mousedown` `mouseup` `keydown` `keyup` `scroll` `resize` `create` `close` `paint` | 所有控件 |
| `click` | **仅** button、label、list |
| `change` | **仅** input、password、multiline、slider、select、list |
| `dbclick` `focus` `blur` | ⚠️ **不会触发**（API 接受但从未绑定） |

常见坑：

- 给 `checkbox` / `radio` / `image` / `panel` 绑 `click` —— 不报错，但永远不会触发。
- `mousedown` / `mouseup` 实际**只响应鼠标左键**。
- 事件名拼写是 `dbclick`（不是 `dblclick`），但它本来也不生效。
- 传非法事件名会抛运行时错误：`参数错误: xxx 不支持`。

### 事件对象里有什么

回调**永远只收到一个参数**——事件对象。字段取决于事件类型：

| 事件 | 字段 |
|:-----|:-----|
| 鼠标类 | `type: "mouse"`, `x`, `y`, `button`, `wheel` |
| `keydown` | `type: "keydown"`, `keyCode`, `scanCode` |
| `keyup` | `type: "keyup"`, `keyCode` |
| `resize` | `type: "resize"`, `width`, `height`, `state` |
| 拖入文件 | `type: "drop"`, `x`, `y`, `files`（数组） |
| list 的 `click` | `value`（被点击的列索引） |
| **其他所有**（button 的 click、input 的 change…） | **空对象 `{}`** |

```javascript
inp.on("keydown", function(e) {
    if (e.keyCode == 13) {      // 注意是 keyCode，不是 key
        submit();
    }
});

win.form("main").on("mousemove", function(e) {
    win.form("main").refs.lblPos.text(e.x + ", " + e.y);
});
```

> ⚠️ 事件对象里**没有 `target` / `sender`**，拿不到"是哪个控件触发的"。

### 怎么在回调里拿到控件

**用闭包捕获，或者用 `refs`：**

```javascript
// 方式一：闭包直接捕获变量
let btn = win.button("b").text("提交");
btn.on("click", function() {
    btn.text("已提交");         // 直接用外面的 btn
});

// 方式二：用 refs
let f = win.form("main").add([ win.input("inp").pos(10,10) ]);
f.refs.inp.on("change", function() {
    f.refs.inp.text("");        // 通过 refs 拿
});
```

> ⚠️ **不要在回调里用 `this`。** 事件回调不是作为对象方法调用的，`this` 没有绑定，值为 `null`；读 `this.xxx` 会抛 `类型错误,不能获取属性 'xxx' 来自: null`。

### 返回值没有用

控件事件的回调返回值会被丢弃，**不能通过 `return false` 阻止默认行为**。

（例外：WebView 的 `bind` 回调，返回值会传回 JS，见下文。）

---

## 五、布局

BxScript **没有布局引擎**，全部是**绝对定位**：

- 坐标系原点在窗口客户区**左上角**，单位是像素。
- 每个控件都要自己 `.pos(x, y)` + `.size(w, h)`。
- 窗口缩放时控件**不会**自动重排，需要自己在 `resize` 事件里手动调整。

```javascript
f.on("resize", function(e) {
    // 手动做响应式：让输入框跟着窗口变宽
    f.refs.inpName.width(e.width - 40);
});
```

项目已启用 DPI Aware（高分屏不会发虚）。脚本里按设计稿像素写即可。

> 想省事的话，**WebView 路线**可以拿到完整的 CSS 布局能力，见[第十一节](#十一webview-混合开发)。

---

## 六、常用控件

### Input / Password / Multiline

```javascript
win.input("inpUser").pos(20, 20).size(200, 28)
win.password("inpPwd").pos(20, 60).size(200, 28)
win.multiline("inpNote").pos(20, 100).size(200, 120)

// 读取内容
let name = f.refs.inpUser.text();

// 输入框内容变化时触发
f.refs.inpUser.on("change", function() {
    checkInput(f.refs.inpUser.text());
});
```

### Select（下拉框）

```javascript
win.select("selCity").pos(20, 20).size(150, 200).items(["北京", "上海", "广州"]);

// 取当前选中项：Select 没有 getSelectedIndex，直接用 text()
let city = f.refs.selCity.text();

f.refs.selCity.on("change", function() {
    update(f.refs.selCity.text());
});
```

> `items()` 会自动选中第 0 项。

### List（列表 / 表格）

这是功能最全的控件，可以当**表格**用。

**简单列表：**

```javascript
win.list("lstItems").pos(20, 60).size(380, 300).items(["苹果", "香蕉", "橘子"]);

f.refs.lstItems.on("click", function() {
    let i = f.refs.lstItems.getSelectedIndex();
    if (i >= 0) {
        dlg.alert("选中了：" + f.refs.lstItems.getSelectedItem(i));
    }
});
```

**多列表格**——`heads` 设列头，`items` 每行传数组：

```javascript
win.list("lstUser").pos(20, 60).size(420, 300)
    .heads([
        { text: "姓名", width: 120 },
        { text: "年龄", width: 80 },
        { text: "城市", width: 200 }
    ])
    .items([
        ["张三", 28, "北京"],
        ["李四", 32, "上海"],
        ["王五", 25, "广州"]
    ]);
```

列头元素既可以是字符串（默认宽 100），也可以是 `{text, width}` 对象。

**List 专属方法：**

| 方法 | 返回 | 说明 |
|:-----|:-----|:-----|
| `getSelectedIndex()` | Number | 当前选中行，无选中为 -1 |
| `setSelectedIndex(i)` | Null | 选中第 i 行 |
| `getSelectedItem(i)` | 任意 | 第 i 行的原始数据（多列表格返回该行的数组） |

配合 `click` 事件的 `e.value` 还能拿到点的是哪一列：

```javascript
f.refs.lstUser.on("click", function(e) {
    let row = f.refs.lstUser.getSelectedIndex();
    let col = e.value;                     // 列索引
    if (col == 1) {
        dlg.alert("年龄列被点击");
    }
});
```

### Image

```javascript
win.image("imgLogo").pos(20, 20).size(200, 150).src("logo.png");
win.image("imgNet").pos(20, 200).size(200, 150).src("https://example.com/a.png");
```

`src` 支持本地文件路径和 `http` 开头的网络地址。

### Button / Label

```javascript
win.button("btnOk").text("确定").pos(20, 20).size(80, 32)
win.label("lblTip").text("提示文本").pos(20, 60)
```

两者都支持 `click` 事件。

### Group / Panel

```javascript
// group 可以装子控件
win.group("grpInfo").text("基本信息").pos(20, 20).size(300, 200).add([
    win.label("lbl1").text("姓名").pos(20, 40)
]);

// panel 目前不能通过脚本 add 子控件
win.panel("pnlMain").pos(0, 0).size(400, 300);
```

> ⚠️ `group` 有 `add()`，**`panel` 没有**。

### Slider / Progress / Checkbox / Radio

> ⚠️ **这四个控件目前只能"显示"，不能通过脚本读写值：**
>
> | 控件 | 现状 |
> |:-----|:-----|
> | `slider` | 无法设置/读取当前值；`change` 事件能触发，但事件对象是空 `{}` |
> | `progress` | 无法设置进度值 |
> | `checkbox` | 无法读取/设置勾选状态，也没有 `click`/`change` 事件 |
> | `radio` | 同上 |

如果需要可交互的进度或勾选，建议用 `label` + 文本、或 `list` 变通实现。

---

## 七、对话框（dlg）

系统对话框，用完即走，不需要挂到窗口上。

```javascript
import std.Dlg as dlg;
import std.IO as IO;
import std.Console as Console;

dlg.alert("操作完成");                          // 信息框，默认标题"提示"
dlg.alert("操作完成", "成功");                   // 自定义标题
dlg.error("读取失败");                          // 错误框，默认标题"错误"
dlg.error("读取失败", "出错了");

if (dlg.confirm("确定要删除吗？")) {             // 返回 Bool
    IO.remove("target.txt");
}

let path = dlg.open("选择文件", "文本文件|*.txt");   // 返回路径，取消返回空串
let save = dlg.save("另存为", "文本文件|*.txt");
let dir  = dlg.folder("选择目录");

if (path != "") {
    Console.println("选择了: " + path);
}
```

`filter` 格式是 `描述|通配符`。

---

## 八、菜单栏

用 `win.item()` 构造菜单项，`f.menu()` 装到窗口上。

```javascript
f.menu([
    win.item("文件", [                          // 第二个参数传数组 → 子菜单
        win.item("新建", function() { newFile(); }),
        win.item("打开", function() { openFile(); }),
        win.item("-"),                          // "-" → 分隔线
        win.item("退出", function() { win.doExit(); })
    ]),
    win.item("编辑", [
        win.item("复制", function() { copy(); }),
        win.item("粘贴", function() { paste(); })
    ]),
    win.item("关于", function() {                // 直接传函数 → 可点击的顶级项
        dlg.alert("BxScript Demo v1.0");
    })
]);
```

要点：

- `win.item(text, 子菜单数组)` → 子菜单
- `win.item(text, 函数)` → 可点击项
- `win.item("-")` → 分隔线（**只在子菜单里有效，顶级分隔线会被忽略**）
- 菜单项的回调收到的是**空事件对象 `{}`**，**无法从参数区分点了哪一项**——所以每一项各自绑自己的函数
- **暂不支持快捷键**（如 Ctrl+S）和菜单图标

菜单项还支持两个可选字段（给 `win.item` 返回的对象赋值即可）：

```javascript
let item = win.item("保存", function() { save(); });
item.enabled = false;      // 置灰
let chk = win.item("显示工具栏", function() { toggle(); });
chk.checked = true;        // 打勾
```

---

## 九、系统托盘

窗口加托盘图标，点击可唤起窗口：

```javascript
f.doTray({
    icon: "tray.ico",          // 必填，否则图标不显示
    tip: "我的应用",            // 鼠标悬停提示
    click: function() {         // 点击托盘图标
        f.show();
    }
});
```

> ⚠️ 方法名是 **`doTray`**，没有 `tray()`。
> 点击托盘时会自动把窗口 `show()` 并置前，然后才触发 `click`。

---

## 十、多窗口与窗体跳转

### 同时建多个窗口

每次 `win.form()` 都会登记一个新窗口，`win.loop()` 会把它们全部渲染出来：

```javascript
let f1 = win.form("win1").text("窗口一").size(300, 200).pos(100, 100);
let f2 = win.form("win2").text("窗口二").size(300, 200).pos(450, 100);
win.loop();     // 两个窗口一起显示
```

### 子窗口：owner / modal

从主窗口打开子窗口时，用 `owner()` 和 `modal()` 说明这个子窗跟主窗是什么关系。

**`owner(父窗体)`** —— 指定所有者。效果：

- 子窗永远压在主窗**之上**，不会被主窗盖住
- 主窗**最小化时子窗跟着消失**，还原时一起回来
- 子窗**不占任务栏按钮**（也不会出现在 Alt-Tab 里）

**`modal([bool])`** —— 模态（默认 `true`）。效果是**弹出后父窗被禁用**，点它没反应，直到子窗关闭才恢复。

**`close()`** —— 关闭（销毁）窗口，会正常触发 `on("close")` 事件。模态对话框靠它结束 `win.render()` 的阻塞。

#### 用法一：非模态子窗（工具面板类）

只加 `owner()`，父窗照常可点、`render()` 立即返回：

```javascript
let fPanel = win.form("find").owner(f)
    .text("查找").size(320, 120)
    .add([ win.input("inpFind").pos(20, 20).size(240, 28) ]);

btn.on("click", function() {
    win.render(fPanel);     // 立即返回，主窗照常操作
});
```

#### 用法二：模态对话框（填数据类）

`owner()` + `modal(true)`，`win.render()` 会**阻塞**到对话框关闭：

```javascript
let result = "";

let f = win.form("main").text("主窗口").size(480, 320).center().add([
    win.button("btnAdd").text("新增记录").pos(160, 130).size(160, 40)
        .on("click", function() {
            let fAdd = win.form("add")
                .owner(f).modal(true)
                .text("新增").size(400, 220).center()
                .add([
                    win.input("inpName").pos(20, 40).size(340, 30),
                    win.button("btnOk").text("确定").pos(300, 140).size(80, 34)
                        .on("click", function() {
                            result = fAdd.refs.inpName.text();   // ① 先在子窗里取值
                            fAdd.close();                        // ② 再关闭
                        })
                ]);

            win.render(fAdd);       // ③ 阻塞到这里，关窗后才继续
            dlg.alert("你填的是：" + result);   // ④ 用存好的值
        })
]);

win.loop();
```

> ⚠️ **值必须在子窗关闭前取出来。** 窗口一旦销毁，里面的原生控件跟着没了，之后 `fAdd.refs.inpName.text()` 只能读到空串。所以标准写法是：在"确定"按钮的回调里**先取值存到外层变量**，再 `close()`。

### 跳转到新窗体：win.render()

`win.render(form)` **立即**渲染并显示指定窗体。要点：

- 要在 `win.loop()` **已经跑起来之后**调用（也就是在事件回调里）
- 被 `render` 过的窗体会从待渲染队列里移除，不会在 `loop()` 时被重复渲染
- 窗体带 `modal(true)` 时 `render()` **阻塞到它关闭**；否则立即返回

`render()` 之前**父窗必须已经渲染出来**（owner 要在渲染时才能解析到）。在事件回调里 `render(child)` 天然满足；如果用 `loop()` 批量渲染，父窗要写在子窗前面。找不到 owner 会退化成普通独立窗口，不报错。

### 显隐切换

```javascript
f.hide();       // 隐藏
f.show();       // 显示
```

> `hide()` 只是隐藏，**不会**结束模态阻塞，也不会让程序退出。要真正关掉窗口请用 `close()`。

### 关闭窗口与退出

- **`f.close()`** 只关闭这一个窗口
- **关闭最后一个顶层窗口**时，程序才退出
- 所以多窗口应用里，关掉其中一个不会影响其它窗口——全部关完才退出

### 模态的已知限制

- **模态窗内没有 Tab 键导航**：控件之间不能用 Tab 切换焦点（`App::PreTranslateMessage` 里没有 `IsDialogMessage`）
- **`modal` 只在 `win.render()` 路径生效**。交给 `win.loop()` 批量渲染的窗体不会被阻塞
- **`modal(true)` 建议配合 `owner()` 用**。没有 owner 时不会报错，但没有可禁用的对象，退化成"只阻塞脚本"

---

## 十一、WebView 混合开发

用 HTML/CSS/JS 写界面，同时调用 BxScript 的系统能力。基于 Edge WebView2（Chromium）内核。

### 基本用法

```javascript
import std.Win as win;
import std.Crypt as Crypt;

let wb = win.webview()                      // 注意：不传 id
    .title("WebView 应用")                    // 默认 "BxScriptWebView"
    .size(1024, 768)                         // 默认 800 × 600
    .html("
        <h1>Hello WebView</h1>
        <button onclick='callNative()'>调用原生函数</button>
        <script>
            async function callNative() {
                let res = await nativeFunc('来自网页的数据');
                alert('原生返回: ' + res);
            }
        </script>
    ")
    .bind("nativeFunc", function(arg) {      // 注册给 JS 调用的函数
        return Crypt.md5(arg);
    });

win.loop();
```

> 📌 **内联 HTML 的小技巧**：BxScript 字符串用双引号，而 HTML 属性也常用双引号，硬碰硬就得写一堆 `\"`。HTML 允许用单引号写属性、JS 也允许用单引号写字符串字面量，所以**外层用双引号、内层用单引号**，一个转义都不用。字符串**支持跨行**，换行会原样保留。

### 几个必须知道的点

- **`win.webview()` 不接 id**，返回的是 WebView 配置对象。
- 窗口**自动居中**，默认 800×600。
- 配置项：`.title()` `.html()` `.size(w,h)` `.width()` `.height()` `.debug()` `.transparent()` `.show()` `.hide()`。

### JS ↔ 脚本 通信

**脚本 → JS**：`.html()` 里怎么写就怎么用，正常的内嵌 JS。
**JS → 脚本**：脚本侧 `.bind(name, fn)` 注册，JS 侧当作全局函数调用。

通信规则的细节：

```javascript
.bind("calc", function(a, b) {     // JS 传的多个参数 → 按位置进来
    return a + b;                  // 返回值自动转成 JSON 送回 JS
})
```

```javascript
// JS 侧：返回值是一个 Promise，必须 await / then
let r = await calc(1, 2);
calc(3, 4).then(r => console.log(r));
```

- JS 实参会按**位置**逐个传进脚本函数（不是打包成数组）。
- 脚本的返回值会被自动 JSON 序列化后送回 JS；返回 `null` 或出错时 JS 侧拿到 `null`。
- **JS 侧调用一律返回 Promise**，忘了 `await` 会拿到 Promise 对象而不是值。

### 常用配置

```javascript
wb.debug();         // 开启 DevTools（F12），开发时非常有用
wb.transparent();   // 透明背景，做无边框异形窗口
```

### 无边框窗口拖动

配合 `transparent()` 和 `doCap()` 可以做无边框窗口：

```javascript
// HTML 里
<div style="width:100%;height:40px;-webkit-app-region:drag"
     onmousemove="move()">标题栏</div>

// 脚本里
.bind("move", function() {
    wb.doCap();     // doCap = 开始拖动窗口
})
```

### WebView 上的窗口控制

```javascript
wb.doMin();     // 最小化
wb.doMax();     // 最大化 / 还原（切换）
wb.doCap();     // 拖动窗口
wb.doExit();    // 退出
wb.doTray("tray.ico", "提示文字");        // 托盘（两参数写法）
wb.doTray({icon: "tray.ico", tip: "提示"}); // 或对象写法
```

> ⚠️ WebView 的托盘**不支持点击回调**（只有 form 的 `doTray` 支持 `click`）。

### form 与 webview 不能混用

`win.loop()` 的逻辑是"二选一"：**只要创建过 webview，`loop()` 就只跑 webview**，之前用 `win.form()` 建的窗口会被忽略。

> 一个脚本里要么用 `win.form()` 那套，要么用 `win.webview()`，不要混着写。

---

## 十二、窗口控制与退出

```javascript
f.doMin();      // 最小化
f.doMax();      // 最大化 / 还原（切换）
f.doCap();      // 拖动窗口（按住标题栏拖动，常用于无边框窗口）
f.close();      // 关闭本窗口（模态对话框靠它结束 render 阻塞）
f.center();     // 居中
f.icon("app.ico");  // 设置窗口图标

win.doExit();       // 退出程序，退出码 0
win.doExit(1);      // 指定退出码
```

> ⚠️ **`doMin` / `doMax` / `doCap` / `close` 只能在渲染之后调用**，也就是必须写在事件回调里。
> 在 `win.loop()` 之前调用会抛异常：`函数还未挂载或挂载失败`。

### 定时自动关闭

`win.loop()` 里会持续泵事件循环，所以 `Timer` 的定时器在界面运行期间照常生效。配合 `win.doExit()` 就能做**定时自动关闭**（启动画面、演示窗口、超时退出等）：

```javascript
import std.Win as win;
import std.Timer as Timer;
import std.Console as Console;

let f = win.form("splash").text("启动中…").size(400, 200).center();

// 3 秒后自动关闭
Timer.setTimeout(function() {
    win.doExit();
}, 3000);

win.loop();

// doExit 之后 loop() 会正常返回，收尾代码能执行
Console.println("已退出，可以在这里做清理");
```

---

## 十三、完整示例：待办清单

```javascript
import std.Win as win;
import std.Dlg as dlg;

let todos = ["写文档", "修 bug"];

let f = win.form("todoApp")
    .text("待办清单")
    .size(420, 460)
    .center()
    .add([
        win.input("inpNew").pos(10, 10).size(310, 30),
        win.button("btnAdd").text("添加").pos(330, 10).size(70, 30),
        win.list("lstTodo").pos(10, 50).size(390, 340).heads([{text: "事项", width: 380}]),
        win.button("btnDel").text("删除选中").pos(10, 400).size(120, 32),
        win.button("btnClear").text("清空").pos(300, 400).size(100, 32)
    ]);

// 刷新列表显示
function refresh() {
    f.refs.lstTodo.items(todos);
}
refresh();

// 添加
f.refs.btnAdd.on("click", function() {
    let text = f.refs.inpNew.text();
    if (text == "") {
        dlg.alert("请输入内容");
        return;
    }
    todos.push(text);
    f.refs.inpNew.text("");     // 清空输入框
    refresh();
});

// 删除选中
f.refs.btnDel.on("click", function() {
    let i = f.refs.lstTodo.getSelectedIndex();
    if (i < 0) {
        dlg.alert("请先选中一项");
        return;
    }
    if (dlg.confirm("确定删除「" + todos[i] + "」吗？")) {
        todos.remove(i);
        refresh();
    }
});

// 清空
f.refs.btnClear.on("click", function() {
    if (dlg.confirm("确定清空全部？")) {
        todos = [];
        refresh();
    }
});

win.loop();
```

---

## 十四、已知限制与注意事项

**一定要看，能省很多调试时间。**

| 项目 | 现状 |
|:-----|:-----|
| `align()` | **空操作**，任何值都不生效 |
| `bgColor()` | 对象与十六进制字符串都支持；但 **`button` 上不显色**，三位简写 `"#F00"` 不支持 |
| `font()` | **只有 `fontSize` 和 `fontColor` 生效**；`fontFamily`、`fontBlod` 被忽略 |
| `slider` | 不能读写当前值 |
| `progress` | 不能设置进度 |
| `checkbox` / `radio` | 不能读写勾选状态，也没有 click/change 事件 |
| `panel` | 不能通过脚本添加子控件（`group` 可以） |
| `dbclick` / `focus` / `blur` | 事件名合法但**永远不会触发** |
| `click` | 只对 button、label、list 有效 |
| `change` | 只对 input、password、multiline、slider、select、list 有效 |
| 回调中的 `this` | **未绑定，使用会抛异常**，请用闭包或 `refs` |
| 回调返回值 | 被丢弃，无法阻止默认行为 |
| 同一事件重复 `on()` | **后者覆盖前者**，不是叠加 |
| 事件对象 | **没有 `target`**；很多事件是空 `{}` |
| 菜单快捷键 | 不支持 |
| 布局 | 无布局引擎，纯绝对定位，窗口缩放需手动响应 |
| 关闭窗口 | `f.close()` 只关自己；**关闭最后一个顶层窗口才退出程序** |
| 模态窗口 | 支持（`owner()` + `modal(true)`）；但**窗内无 Tab 键导航** |
| 读取控件值 | 窗口销毁后控件一并消失，之后 `refs.xxx.text()` 只能读到空串，**需在关闭前取值** |
| `form` + `webview` | **不能混用**，`loop()` 只跑其中一个 |

---

## 下一步

- 逐个 API 的速查表 → [UI 模块参考](stdlib/ui.md)
- 更多示例 → 仓库 `examples/` 目录（`snippets` 是 UI 用法最全的例子）
- 语言基础 → [快速开始](guide.md)
