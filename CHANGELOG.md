# BxScript 升级日志

> 版本：未发布（当前工作区未提交改动）
> 整理日期：2026-10-20
> 说明：本日志覆盖工作区中全部未提交内容，其中部分改动早于本次整理。

---

## ⚠️ 行为变更（升级前请阅读）

1. **分号策略（Go / JS ASI 风格）**
   - 同一行出现多条语句时必须用 `;` 分隔：`let a = 1 a += 1` 现在会报「此处期望: ; 或换行」。
   - 每条语句独占一行时分号可省略：`let a = 1` 换行 `a += 1` 合法。
   - 换行即语句结束。目前覆盖：表达式语句、`let`、`throw` 的结尾判定。
   - `import`、`return`、`break`、`continue` 仍要求 `;` 或 `}`（后续可按同一规则扩展）。

2. **变量语义放宽**
   - 读取未声明的变量返回 `null`（不再抛「变量未定义」）。
   - 给未声明的变量赋值会在全局作用域创建它。
   - 同一作用域重复 `let` 不报错，后者覆盖。

3. **函数声明**
   - 顶层和每个 `{}` 块都会提升，任何作用域都可以先调用后定义。
   - 同名重复声明后者生效。
   - 块内 / 函数体内的 `function` 声明不再被静默丢弃。

4. **相等语义**
   - `==` / `!=` 一律为深比较；语言不提供 `===` / `!==`（写了会明确报错）。
   - `Buffer` 按字节内容比较。
   - 自引用对象（`s.me = s; s == s`）直接判等，不再栈溢出。
   - 两个不同的循环引用对象会抛可被 `try/catch` 接住的「比较嵌套过深（可能存在循环引用）」。

5. **词法边界**
   - 标识符、数字、符号不再跨行合并：
     - `1` 换行 `2` 不再变成 `12`
     - `foo` 换行 `bar` 不再变成 `foobar`
     - `=` 换行 `=` 不再变成 `==`

6. **事件回调异常**
   - 事件循环中回调抛出的未捕获异常不再让进程静默消失；改为输出错误（GUI 版还会弹错误框），程序继续运行。

---

## 语言核心

### 词法 / 解析

- 支持 `0x` / `0b` / `0o` 进制字面量。
- 支持跨行字符串，行尾换行会原样保留。
- 明确拦截 `===` / `!==`，并给出「不支持的运算符」错误。
- 标识符 / 数字 / 符号在行尾停止（见上面的行为变更 5）。
- 解析器引入 token 行号跟踪（`ConsumedLines`），用于换行断句。
- 函数声明提升扩展到块级。
- `for...in` 执行逻辑落地：对象遍历键名、数组遍历下标、字符串按下标（Unicode 码点）遍历。
- 数字字面量解析支持进制前缀。

### 运行时 / 值模型

- `Environment` 的宽松变量语义（未定义=null、赋值自动建全局、重复声明覆盖）。
- 新增 `ValueEqual` 深比较入口（指针快速路径 + 深度上限 1000）。
- `BufferValue`：
  - 新增 `Buffer.alloc(n)`、`Buffer.from(str | number[])`。
  - 支持下标读写、`length` / `size`。
  - 新增按内容相等的 `Equal`。
- 事件循环新增 `ErrorHandler`：回调异常统一走「stderr（GUI 版再加错误框）」。
- REPL 修复：运行错误后 `EvaluateProgram` 返回 `nullptr`，`PrintResult` 现在会判空，不再段错误。

---

## 标准库

### Net

- `HttpQueryInfoW` 改用宽字符缓冲区读取 Content-Type，`type` 字段不再出现 `"t\0e\0x\0t..."` 乱码。
- 修正 body 类型分支：
  - 文本响应 → `String`
  - 二进制响应 → `Buffer`
- 字节嗅探改为在读完 body 之后执行。

### Dlg / 消息框

- 修复超过 65535 字符时 `MessageBoxW` 静默失败的问题：自动截断到 60000 字符，并注明原始长度。
- `alert` / `confirm` / `error` 全部受益。

### GUI（Win 模块）

- 新增 `close()`：关闭窗口（走 `WM_CLOSE`，会触发关闭事件）。
- 新增 `owner(form)`：指定所有者窗体；有 owner 的窗口不占任务栏、不进 Alt-Tab。
- 新增 `modal([bool])`：配合 `win.render()` 使用，阻塞到子窗体关闭后脚本继续执行。
- `win.render()` 支持模态运行（`GuiRuntime::RunModal`）。
- 多窗体：顶层窗口计数，只有最后一个顶层窗体关闭时才退出进程。
- 修复 `WM_DESTROY` 未注销 `WindowRegistry` 导致 HWND 复用时查表错乱的问题。
- 修复背景画刷管理：`formBrush` 改为 `Brush` 对象，析构正确释放 `HBRUSH`。
- `bgColor` 支持 `{R,G,B[,A]}` 对象、`"#RRGGBB"`、`0xRRGGBB`（此前传入 `{R,G,B}` 会静默失效）。
- 新增 `GuiRenderer::FindControl(id)`，供运行时按脚本 id 反查原生控件。
- `App` 新增 `IsAlive` / `SetEnabled` / `Activate`，供模态循环使用。

---

## 测试与回归

- **测试套件恢复可运行**：此前 `IIFEFunc` 段错误导致后续约 70 个用例从未执行；修复后 **100/100 全部通过**。
  - 补齐用例缺失的 `import`。
  - GUI 用例不再调用 `win.loop()`（无头环境会永久阻塞）。
  - 修正测试自身的问题（`IO.println` 不存在、`StringPrototype` 死循环、`defer` 语法、线程 API 名称等）。
  - 运行期错误改用脚本内 `try/catch` 断言（顶层错误会被解释器吞掉）。
- 新增用例：
  - 分号 / 换行策略（同行为多语句报错、换行分隔、跨行链式调用、行尾运算符）。
  - 词法 token 不跨行合并。
  - 深比较环处理（自引用对象、两个不同的环、自引用数组）。
  - `Buffer` 相等。
- 新增 `regression.bx`（381 行，约 12 秒自动跑完）与 `regression_check.ps1`：
  - 覆盖 `for...in`、`Buffer`、进制字面量、跨行字符串、`bgColor`、关闭窗口、owner、modal、close。

---

## 文档

- 新增：
  - `docs/gui.md`（920 行 GUI 开发指南：最小示例、控件、链式属性、事件、菜单、托盘、多窗体、WebView）。
  - `docs/stdlib/clipboard.md`、`console.md`、`keyboard.md`、`type.md`。
- 更新：
  - `docs/guide.md`：补充字符串、数字章节。
  - `docs/stdlib/ui.md`：大幅扩充多窗体、菜单栏、系统托盘、WebView 控件说明。
  - `docs/stdlib/net.md`：明确 `body` 按 Content-Type 返回 String / Buffer。
  - `docs/stdlib/buffer.md`、`date.md`、`io.md`、`json.md`、`mouse.md`、`os.md`、`screen.md`、`thread.md`：统一 import 示例与 API 名称。
  - `docs/builtins.md`、`embedding.md`、`other.md`：内容与 API 描述对齐。
  - `docs/_sidebar.md`：补充新增文档的导航入口。
  - `readme.md` / `docs/README.md`：修正示例中的 import 别名、`dlg.alert`、`fontSize`、字符串引号等，使之可直接运行。

---

## 构建与产物

- `BxScript_Console` / `BxScript_GUI` 已重新构建并通过全量测试。
- 发布用二进制可经 `upx -9` 压缩（约 917 KB → 299 KB）。
