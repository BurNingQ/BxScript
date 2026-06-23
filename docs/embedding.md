# C++ 集成指南

BxScript 作为嵌入式脚本语言，可以轻松集成到 C++ 项目中。

## 基本使用

```cpp
#include "evaluator/Interpreter.h"

int main() {
    // 执行脚本字符串
    Interpreter::Run(R"(
        import std.IO as IO;
        IO.println("Hello from C++ embedding!");
    )");

    return 0;
}
```

## 自定义全局环境

```cpp
#include "evaluator/Interpreter.h"
#include "evaluator/Environment.h"

int main() {
    auto env = std::make_shared<Environment>();

    // 注入 C++ 函数
    auto sayHello = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr>& args) -> ValuePtr {
            std::cout << "Hello from C++!" << std::endl;
            return std::make_shared<NullValue>();
        }
    );
    env->DeclareVar("sayHello", sayHello);

    // 注入 C++ 变量
    env->DeclareVar("APP_VERSION", std::make_shared<StringValue>("1.0.0"));

    // 在预设环境中执行
    Interpreter::Run(R"(
        import std.IO as IO;
        IO.println("Version: " + APP_VERSION);
        sayHello();
    )", env);

    return 0;
}
```

## 执行脚本文件

```cpp
#include "evaluator/Interpreter.h"
#include "common/VFS.h"

int main() {
    std::string code = VFS::ReadFile("app.bx");
    Interpreter::Run(code);
    return 0;
}
```

## 自定义 NativeFunction

```cpp
// 定义一个原生函数，接收参数并返回值
auto addFn = std::make_shared<NativeFunctionValue>(
    [](const std::vector<ValuePtr>& args) -> ValuePtr {
        double sum = 0;
        for (auto& arg : args) {
            if (arg->type == ValueType::NUMBER) {
                sum += std::static_pointer_cast<NumberValue>(arg)->Value;
            }
        }
        return std::make_shared<NumberValue>(sum);
    }
);
env->DeclareVar("add", addFn);
```

脚本中调用：
```javascript
let result = add(1, 2, 3, 4);  // 10
```

## 与 GUI 协同

当脚本包含 GUI 时，需要调用 `GuiRuntime::Run()` 进入消息循环：

```cpp
#include "gui/GuiRuntime.h"

int main() {
    Interpreter::Run(sourceCode);
    // 如果有窗体，进入消息循环
    if (GuiRuntime::HasForms()) {
        GuiRuntime::Run();
    }
    return 0;
}
```

## 错误处理

```cpp
try {
    Interpreter::Run(sourceCode);
} catch (const RuntimeError& e) {
    std::cerr << "脚本错误: " << e.Message << std::endl;
}
```

## 模块缓存

多次执行共享模块缓存，避免重复解析：

```cpp
// 第一次执行加载模块
Interpreter::Run("import std.Math as Math;");

// 第二次执行命中缓存，直接使用
Interpreter::Run("import std.Math as Math; IO.print(Math.PI);");
```

如需清除缓存：
```cpp
Interpreter::ModuleCache.clear();
Interpreter::ASTRegistry.clear();
```
