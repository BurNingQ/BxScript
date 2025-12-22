#include <gtest/gtest.h>
#include <string>
#include <memory>

#include <filesystem>
#include <thread>

#include "../parser/Parser.h"
#include "../evaluator/Interpreter.h"
#include "../evaluator/Value.h"
#include "../evaluator/Environment.h"
#include "../evaluator/EventLoop.h"
#include "../stdlib/GuiModule.h"
#include "gui/GuiRuntime.h"

#define ASSERT_IS_NUMBER(valPtr, expected) \
    do { \
        ASSERT_EQ((valPtr)->type, ValueType::NUMBER) << "Expected NUMBER type"; \
        auto* num = dynamic_cast<NumberValue*>((valPtr).get()); \
        ASSERT_NE(num, nullptr); \
        EXPECT_DOUBLE_EQ(num->Value, (expected)); \
    } while(0)

#define ASSERT_IS_STRING(valPtr, expected) \
    do { \
        ASSERT_EQ((valPtr)->type, ValueType::STRING) << "Expected STRING type"; \
        auto* str = dynamic_cast<StringValue*>((valPtr).get()); \
        ASSERT_NE(str, nullptr); \
        EXPECT_EQ(str->Value, (expected)); \
    } while(0)

#define ASSERT_IS_BOOL(valPtr, expected) \
    do { \
        ASSERT_EQ((valPtr)->type, ValueType::BOOL) << "Expected BOOL type"; \
        auto* b = dynamic_cast<BoolValue*>((valPtr).get()); \
        ASSERT_NE(b, nullptr); \
        EXPECT_EQ(b->Value, (expected)); \
    } while(0)

class InterpreterTest : public ::testing::Test {
protected:
    std::shared_ptr<Environment> globalEnv{};

    void RestTest() {
        EventLoop::Reset();
        globalEnv = std::make_shared<Environment>();
        Interpreter::ModuleCache.clear();
        Interpreter::ModuleAST.clear();
        Interpreter::ASTRegistry.clear();
    }

    ValuePtr Eval(const std::string &code) {
        RestTest();
        auto res = Interpreter::Run(code, globalEnv);
        if (!GuiModule::GlobalForms.empty()) {
            GuiRuntime::Run();
        } else {
            EventLoop::RunLoop();
        }
        return res;
    }

    void EvalAsync(const std::string &code, int timeoutMs = 5000) {
        Eval(code);
        auto start = std::chrono::steady_clock::now();
        while (EventLoop::ShouldKeepAlive()) {
            EventLoop::Dispatch(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            if (elapsed > timeoutMs) {
                std::cerr << "\033[1;31m[Test Error] Async execution timed out after " << timeoutMs << "ms!\033[0m" <<
                        std::endl;
                break;
            }
        }
    }

    ValuePtr GetGlobalVar(const std::string &name) {
        try {
            return globalEnv->LookupVar(name);
        } catch (...) {
            return std::make_shared<NullValue>();
        }
    }
};

// ==========================================
// 基础运算
// ==========================================

TEST_F(InterpreterTest, BasicArithmetic) {
    // 加减乘除
    ASSERT_IS_NUMBER(Eval("1 + 2;"), 3.0);
    ASSERT_IS_NUMBER(Eval("10 - 4;"), 6.0);
    ASSERT_IS_NUMBER(Eval("3 * 4;"), 12.0);
    ASSERT_IS_NUMBER(Eval("10 / 2;"), 5.0);
    // 优先级
    ASSERT_IS_NUMBER(Eval("1 + 2 * 3;"), 7.0);
    ASSERT_IS_NUMBER(Eval("(1 + 2) * 3;"), 9.0);
}

TEST_F(InterpreterTest, StringConcatenation) {
    ASSERT_IS_STRING(Eval("\"Hello \" + \"World\";"), "Hello World");
    ASSERT_IS_STRING(Eval("\"Number: \" + 1;"), "Number: 1"); // 测试隐式转换
}

// ==========================================
// 变量与赋值
// ==========================================

TEST_F(InterpreterTest, VariableDeclarationAndAssignment) {
    // 声明并读取
    ASSERT_IS_NUMBER(Eval("let a = 10; a;"), 10.0);
    // 变量计算
    ASSERT_IS_NUMBER(Eval("let a = 5; let b = a * 2; b;"), 10.0);
    // 重新赋值
    ASSERT_IS_NUMBER(Eval("let a = 1; a = 5; a;"), 5.0);
}

// ==========================================
// 控制流测试 (IF)
// ==========================================

TEST_F(InterpreterTest, IfElseStatement) {
    ASSERT_IS_NUMBER(Eval("let a = 10; if (a > 5) { a = 1; } else { a = 0; } a;"), 1.0);
    ASSERT_IS_NUMBER(Eval("let a = 2; if (a > 5) { a = 1; } else { a = 0; } a;"), 0.0);
}

// ==========================================
// 循环与作用域
// ==========================================
TEST_F(InterpreterTest, WhileLoop) {
    std::string code = R"(
        let a = 1;
        while(1){
            a++;
            if(a == 6) {
                break;
            }
        }
        a;
    )";
    ASSERT_IS_NUMBER(Eval(code), 6.0);
}

TEST_F(InterpreterTest, ForLoopBasic) {
    // 计算 0 到 9 的和
    std::string code = R"(
        let sum = 0;
        for (let i = 0; i < 10; i++) {
            sum = sum + i;
        }
        sum;
    )";
    ASSERT_IS_NUMBER(Eval(code), 45.0);
}

TEST_F(InterpreterTest, ForLoopScope) {
    // 测试循环内部的 let 是否会污染外部或者重复定义报错
    std::string code = R"(
        let i = 999;
        for (let i = 0; i < 5; i++) {
            // 这里的 i 应该是内部的 i，不影响外部
            let temp = i;
        }
        i; // 应该还是 999
    )";
    ASSERT_IS_NUMBER(Eval(code), 999.0);
}

TEST_F(InterpreterTest, BreakStatement) {
    std::string code = R"(
        let i = 0;
        for (let k = 0; k < 10; k++) {
            if (k == 5) {
                break;
            }
            i = i + 1;
        }
        i;
    )";
    // 循环应该执行 0,1,2,3,4 次，共 5 次，k=5时退出
    ASSERT_IS_NUMBER(Eval(code), 5.0);
}

TEST_F(InterpreterTest, ContinueStatement) {
    std::string code = R"(
        let sum = 0;
        for (let i = 0; i < 5; i++) {
            if (i == 2) {
                continue; // 跳过 2
            }
            sum = sum + i;
        }
        sum;
    )";
    // 0 + 1 + 3 + 4 = 8
    ASSERT_IS_NUMBER(Eval(code), 8.0);
}

// ==========================================
// 对象与数组
// ==========================================

TEST_F(InterpreterTest, ObjectLiteral) {
    std::string code = R"(
        let obj = { x: 10, y: 20 };
        obj.z = obj.x + obj.y;
        obj.z;
    )";
    ASSERT_IS_NUMBER(Eval(code), 30.0);
}

TEST_F(InterpreterTest, ArrayPushPop) {
    // 测试 Push
    std::string codePush = R"(
        let arr = [];
        arr.push(1);
        arr.push(2);
        arr[1];
    )";
    ASSERT_IS_NUMBER(Eval(codePush), 2.0);

    // 测试 Length
    std::string codeLen = R"(
        let arr = [1, 2, 3];
        arr.length;
    )";
    ASSERT_IS_NUMBER(Eval(codeLen), 3.0);

    // 测试 Pop
    std::string codePop = R"(
        let arr = [10, 20];
        let val = arr.pop();
        val;
    )";
    ASSERT_IS_NUMBER(Eval(codePop), 20.0);

    std::string codePop1 = R"(
        let val = [10, 20].pop();
        val;
    )";
    ASSERT_IS_NUMBER(Eval(codePop1), 20.0);
}

// 验证数组在循环中的操作
TEST_F(InterpreterTest, ArrayInLoop) {
    std::string code = R"(
        let arr = [];
        for (let i = 0; i < 5; i++) {
            arr.push(i * 2);
        }
        arr[3]; // 应该 6
    )";
    ASSERT_IS_NUMBER(Eval(code), 6.0);
}

TEST_F(InterpreterTest, FunctionCall) {
    std::string code = R"(
        function add(a, b) {
            return a + b;
        }
        let res = add(10, 20);
        res;
    )";
    ASSERT_IS_NUMBER(Eval(code), 30.0);
}

TEST_F(InterpreterTest, Recursion) {
    std::string code = R"(
        function fib(n) {
            if (n <= 1) { return n ;}
            return fib(n - 1) + fib(n - 2);
        }
        fib(10);
    )";
    ASSERT_IS_NUMBER(Eval(code), 55.0);
}

TEST_F(InterpreterTest, Closure) {
    // 闭包测试
    std::string code = R"(
        function makeAdder(x) {
            return function(y) {
                return x + y;
            };
        }
        let add5 = makeAdder(5);
        add5(10);
    )";
    ASSERT_IS_NUMBER(Eval(code), 15.0);
}

TEST_F(InterpreterTest, TryCatch) {
    std::string code = R"(
        let res = 0;
        try {
            throw 100;
            res = 1; // 不会执行
        } catch (e) {
            res = e;
        }
        res;
    )";
    ASSERT_IS_NUMBER(Eval(code), 100);
}

TEST_F(InterpreterTest, ToFixed) {
    std::string code = R"(
        let a = 3.141592653589793;
        a = a.toFixed(2);
        a;
    )";
    ASSERT_IS_STRING(Eval(code), "3.14");
}

TEST_F(InterpreterTest, ToFixed_Rounding) {
    std::string code = R"(
        let a = 5.56789;
        a.toFixed(1);
    )";
    ASSERT_IS_STRING(Eval(code), "5.6");
}

TEST_F(InterpreterTest, ToFixed_Padding) {
    std::string code = R"(
        let a = 10;
        a.toFixed(3);
    )";
    ASSERT_IS_STRING(Eval(code), "10.000");
}

TEST_F(InterpreterTest, ToFixed_Error_ArgsCount) {
    std::string code = "let a = 1; a.toFixed();";
    EXPECT_THROW({
                 Eval(code);
                 }, std::runtime_error);
}

// toFixed
TEST_F(InterpreterTest, ToFixed_Error_ArgType) {
    std::string code = "let a = 1; a.toFixed(\"2\");";
    EXPECT_THROW({
                 Eval(code);
                 }, std::runtime_error);
}

namespace fs = std::filesystem;

// import
TEST_F(InterpreterTest, ImportSystem) {
    fs::create_directories("lib");
    std::ofstream out("lib/math.bx");
    out << "let PI = 3.14; function double(x) { return x * 2; };";
    out.close();
    std::string code = R"(
        // 会去读取 ./lib/math.bx
        import std.math as m;
        let res = m.double(10) + m.PI;
        res;
    )";
    ASSERT_IS_NUMBER(Eval(code), 23.14);
    fs::remove_all("lib");
}

// import
TEST_F(InterpreterTest, ImportSystemO) {
    fs::create_directories("lib");
    std::ofstream out("lib/math.bx");
    out << "String.Go = function(){return this;}";
    out.close();
    std::string code = R"(
        // 会去读取 ./lib/math.bx
        import std.math as m;
        "ABc".Go();
    )";
    ASSERT_IS_STRING(Eval(code), "ABc");
    fs::remove_all("lib");
}

TEST_F(InterpreterTest, CompoundAssignment) {
    ASSERT_IS_NUMBER(Eval("let a = 1; a += 2; a;"), 3.0);
    ASSERT_IS_NUMBER(Eval("let a = 5; a -= 2; a;"), 3.0);
    ASSERT_IS_NUMBER(Eval("let a = 2; a *= 3; a;"), 6.0);
}

TEST_F(InterpreterTest, StringFunc) {
    auto res = Eval(R"("HELLO".indexOf("E"))");
    ASSERT_IS_NUMBER(res, 1);

    res = Eval(R"("HELLO".endsWith("O"))");
    ASSERT_IS_BOOL(res, true);

    res = Eval(R"("HELLO".startsWith("H"))");
    ASSERT_IS_BOOL(res, true);

    res = Eval(R"("HELLO".lastIndexOf("L"))");
    ASSERT_IS_NUMBER(res, 3);
}

TEST_F(InterpreterTest, ProtoTest) {
    auto res = Eval(R"(
           String.prototype.test = function() {
               return this;
           };
           "abc".test();
    )");
    ASSERT_IS_STRING(res, "abc");
}

TEST_F(InterpreterTest, StringFromCharCode) {
    auto res = Eval(R"(
           String.fromCharCode(65);
    )");
    ASSERT_IS_STRING(res, "A");
}

TEST_F(InterpreterTest, StringPrototype) {
    auto res = Eval(R"(
           String.prototype.hello = function(){
                return this + " hello";
           }
           "BurNing".hello();
    )");
    ASSERT_IS_STRING(res, "BurNing hello");
}

TEST_F(InterpreterTest, StringStatic) {
    auto res = Eval(R"(
           String.hello = function(){
                return "33"
           }
           String.hello();
    )");
    ASSERT_IS_STRING(res, "33");
}

TEST_F(InterpreterTest, ArrayIsSame) {
    auto res = Eval(R"(
            let a = [1,2,3,4];
            let b = [1,2,3,4];
            a == b;
    )");
    ASSERT_IS_BOOL(res, true);
}

TEST_F(InterpreterTest, ArrayIndexOf) {
    auto res = Eval(R"(
            let a = [1,2,3,4];
            a.indexOf(2);
    )");
    ASSERT_IS_NUMBER(res, 1);
}

TEST_F(InterpreterTest, ObjectIsSame) {
    auto res = Eval(R"(
            let a = {
                sex: 1,
                name: "burning"
            };
            let b = {
                name: "burning",
                sex: 1
            };
            a == b;
    )");
    ASSERT_IS_BOOL(res, true);
    res = Eval(R"(
            let a = {
                sex: 1,
                name: "burning"
            };
            let b = {
                sex: 0,
                name: "burning"
            };
            a == b;
    )");
    ASSERT_IS_BOOL(res, false);
}

TEST_F(InterpreterTest, ArrayForEachScript) {
    std::string code = R"(
        Array.prototype.forEach = function(cb) {
            for (let i = 0; i < this.length; i++) {
                cb(this[i], i);
            }
        };
        let sum = 0;
        let arr = [1, 2, 3];
        arr.forEach(function(e, i) {
            sum = sum + e;
        });
        sum;
    )";
    ASSERT_IS_NUMBER(Eval(code), 6.0);
}

TEST_F(InterpreterTest, ThreadDispatchNonBlocking) {
    std::string code = R"(
        // 模拟 UI 状态
        let ui_state = 0;
        Thread.onMessage(function(data) {
            // 收到子线程的数据，更新 UI
            ui_state = ui_state + data;
        });

        // 启动子线程
        Thread.start(function() {
            // 模拟耗时
            Thread.sleep(50);
            // 发送结果 10
            Thread.postMessage(10);

            // 模拟耗时
            Thread.sleep(50);
            // 发送结果 20
            Thread.postMessage(20);
        });

        // 模拟 GUI 主循环
        let frameCount = 0;

        while (ui_state < 30) {
            let processed = Thread.dispatchMessage();
            // 模拟渲染界面 (这里只是计数)
            frameCount++;
            // 超时保护 (防止测试代码写挂了死循环)
            if (frameCount > 10000000) {
                break;
            }
        }
        ui_state;
    )";
    ASSERT_IS_NUMBER(Eval(code), 30.0);
}

TEST_F(InterpreterTest, DateNowStructure) {
    std::string code = R"(
        let d = Date.now();
        if (d.timestamp <= 0) { throw "timestamp error"; }
        if (d.year < 2024) { throw "year error"; }
        d.year;
    )";
    auto res = Eval(code);
    ASSERT_EQ(res->type, ValueType::NUMBER);
}

TEST_F(InterpreterTest, DateFromTimestamp) {
    std::string code = R"(
        let t = 1698381001000;
        let d = Date.from(t);
        d.format("yyyy-MM-dd");
    )";
    // 运行不崩，且返回字符串
    auto res = Eval(code);
    ASSERT_IS_STRING(res, "2023-10-27");
}

TEST_F(InterpreterTest, DateFromStringStandard) {
    std::string code = R"(
        let d = Date.from("2025-12-09 10:20:30");
        d.format("HH:mm:ss");
    )";
    ASSERT_IS_STRING(Eval(code), "10:20:30");
}

TEST_F(InterpreterTest, DateFromStringChinese) {
    std::string code = R"(
        let d = Date.from("2025年12月09日 10时20分30秒");
        d.format("yyyy/MM/dd");
    )";
    ASSERT_IS_STRING(Eval(code), "2025/12/09");
}

TEST_F(InterpreterTest, DateChaining) {
    std::string code = R"(
        let y = Date.from("2022-01-01 00:00:00").year;
        y;
    )";
    ASSERT_IS_NUMBER(Eval(code), 2022.0);
}

TEST_F(InterpreterTest, DateMutability) {
    std::string code = R"(
        let d = Date.from("2020-01-01 00:00:00");
        d.timestamp = d.timestamp + 31536000000; // 加一年 (粗略)
        d.timestamp = 0; // 回到 1970
        d.format("yyyy");
    )";
    ASSERT_IS_STRING(Eval(code), "1970");
}

TEST_F(InterpreterTest, ObjectKeys) {
    std::string code = R"(
        let obj = {
            a: 1,
            b: 2,
            name: "test"
        };
        let keys = Object.keys(obj);
        if (!Array.isArray(keys)) {
            throw "keys return error";
        }
        keys.length;
    )";
    ASSERT_IS_NUMBER(Eval(code), 3.0);
}

TEST_F(InterpreterTest, ObjectRemove) {
    std::string code = R"(
        let obj = {
            a: 1,
            b: 2,
            c: 3
        };
        Object.remove(obj, "a");
        if (obj.a != null) {
            throw "remove a failed"
        }
        Object.remove(obj, "b", "c");
        Object.keys(obj).length;
    )";
    ASSERT_IS_NUMBER(Eval(code), 0.0);
}

TEST_F(InterpreterTest, ObjectKeysEmpty) {
    std::string code = R"(
        let obj = {};
        Object.keys(obj).length;
    )";
    ASSERT_IS_NUMBER(Eval(code), 0.0);
}

TEST_F(InterpreterTest, CryptBase64) {
    std::string code = R"(
        let a = "你好ABC123";
        let c = Crypt.encode(a);
        Crypt.decode(c)
    )";
    ASSERT_IS_STRING(Eval(code), "你好ABC123");
}

TEST_F(InterpreterTest, JsonParseBasic) {
    std::string code = R"(
        let jsonStr = "{\"name\": \"BxScript\", \"version\": 1.0, \"features\": [\"io\", \"json\"]}";
        let obj = JSON.parse(jsonStr);
        if (obj.name != "BxScript") {
            throw "name error"
        }
        if (obj.version != 1.0) {
            throw "version error"
        }
        if (obj.features[1] != "json") {
            throw "array error"
        }
        obj.name;
    )";
    ASSERT_IS_STRING(Eval(code), "BxScript");
}

TEST_F(InterpreterTest, JsonStringifyBasic) {
    std::string code = R"(
        let obj = {
            id: 100,
            active: true
        };
        let s = JSON.stringify(obj);
        s;
    )";
    auto res = Eval(code);
    ASSERT_EQ(res->type, ValueType::STRING);
    std::string s = std::static_pointer_cast<StringValue>(res)->Value;
    EXPECT_TRUE(s.find("\"id\":100") != std::string::npos);
    EXPECT_TRUE(s.find("\"active\":true") != std::string::npos);
}

TEST_F(InterpreterTest, JsonRoundTrip) {
    std::string code = R"(
        let original = {
            num: 123.456,
            str: "Hello World",
            boolVal: false,
            nullVal: null,
            arr: [1, 2, { nested: "deep" }],
            subObj: {
                x: 10,
                y: 20
            }
        };
        let jsonStr = JSON.stringify(original);
        let restored = JSON.parse(jsonStr);
        original == restored;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(InterpreterTest, JsonArrayOfObjects) {
    std::string code = R"(
        let list = [
            { id: 1 },
            { id: 2 }
        ];
        let s = JSON.stringify(list);
        let list2 = JSON.parse(s);
        list2[1].id;
    )";
    ASSERT_IS_NUMBER(Eval(code), 2.0);
}

TEST_F(InterpreterTest, JsonParseError) {
    std::string code = R"(
        let badJson = "{ \"a\": 1 ";
        JSON.parse(badJson);
    )";
    EXPECT_THROW({
                 Eval(code);
                 }, std::runtime_error);
}

TEST_F(InterpreterTest, JsonIgnoreFunction) {
    std::string code = R"(
        let obj = {
            data: 1,
            func: function() { return 1; }
        };
        let s = JSON.stringify(obj);
        let obj2 = JSON.parse(s);
        Object.keys(obj2).length;
    )";
    ASSERT_IS_NUMBER(Eval(code), 1.0);
}

TEST_F(InterpreterTest, MD5) {
    std::string code = R"(
        let c = "你好123kld87384&*^%";
        let a = Crypt.md5(c);
        IO.println(a);
        a;
    )";
    auto res = Eval(code);
    ASSERT_IS_STRING(res, "c443f994e8c667e0a2f7896965e3c570");
}

TEST_F(InterpreterTest, SHA256) {
    std::string code = R"(
        let c = "hello";
        let a = Crypt.sha256(c);
        a;
    )";
    auto res = Eval(code);
    ASSERT_IS_STRING(res, "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
}

TEST_F(InterpreterTest, CryptHMAC) {
    std::string code = R"(
        let msg = "The quick brown fox jumps over the lazy dog";
        let key = "key";
        let sign = Crypt.hmac("sha256", key, msg);
        sign;
    )";
    ASSERT_IS_STRING(Eval(code), "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8");
}

TEST_F(InterpreterTest, CryptCRC32) {
    std::string code = R"(
        let msg = "The quick brown fox jumps over the lazy dog";
        let sign = Crypt.crc32(msg);
        sign;
    )";
    ASSERT_IS_STRING(Eval(code), "414fa339");
}

class IOTest : public InterpreterTest {
protected:
    std::string sandboxDir = "io_sandbox";

    void SetUp() override {
        InterpreterTest::SetUp();
        if (fs::exists(sandboxDir)) fs::remove_all(sandboxDir);
        fs::create_directories(sandboxDir);
        std::ofstream f(sandboxDir + "/dummy.txt");
        f << "Hello IO";
        f.close();
    }

    void TearDown() override {
        if (fs::exists(sandboxDir)) fs::remove_all(sandboxDir);
    }
};

TEST_F(IOTest, ExistAndTypeCheck) {
    std::string code = R"(
        let dir = "io_sandbox";
        let file = "io_sandbox/dummy.txt";
        if (!IO.exist(dir)) {
            throw "dir should exist"
        };
        if (!IO.exist(file)) {
            throw "file should exist"
        };
        if (!IO.isDir(dir)) {
            throw "dir type error"
        };
        if (IO.isFile(dir)) {
            throw "dir is not file"
        };
        if (!IO.isFile(file)) {
            throw "file type error"
        };
        if (IO.isDir(file)) {
            throw "file is not dir"
        };
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, CopyAndRename) {
    std::string code = R"(
        let src = "io_sandbox/dummy.txt";
        let dst = "io_sandbox/copy.txt";
        let moved = "io_sandbox/moved.txt";
        if (!IO.copy(src, dst)) {
            throw "copy failed";
        };
        if (!IO.exist(dst)) {
            throw "dst not created";
        }
        if (!IO.rename(dst, moved)) {
            throw "rename failed";
        }
        if (IO.exist(dst)) {
            throw "dst should be gone";
        }
        if (!IO.exist(moved)) throw "moved should exist";
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, MkdirAndRemove) {
    std::string code = R"(
        let deepDir = "io_sandbox/a/b/c";
        if (!IO.mkdir(deepDir)) {
            throw "mkdir failed";
        }
        if (!IO.isDir(deepDir)) {
            throw "check deep dir failed";
        }
        if (!IO.remove("io_sandbox/a")) {
            throw "remove failed";
        }
        IO.exist("io_sandbox/a");
    )";
    ASSERT_IS_BOOL(Eval(code), false);
}

TEST_F(IOTest, ListDirectory) {
    std::ofstream(sandboxDir + "/a.txt") << "a";
    std::ofstream(sandboxDir + "/b.txt") << "b";
    fs::create_directory(sandboxDir + "/sub");

    std::string code = R"(
        let list = IO.list("io_sandbox");
        if (list.length != 4) {
            throw "list length error: " + list.length;
        }
        let item = list[0];
        if (item.type != "file" && item.type != "dir") {
            throw "item type error";
        }
        if (item.size < 0) {
            throw "item size error";
        }
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, FileAttributes) {
    std::string code = R"(
        let path = "io_sandbox/dummy.txt";
        let info = IO.attr(path);
        if (info.name != "dummy.txt") {
            throw "attr name error";
        }
        if (info.size != 8) {
            throw "attr size error";
        }
        let empty = IO.attr("io_sandbox/ghost.txt");
        if (empty.size != null) {
            throw "ghost file should be empty";
        }
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, AbsolutePath) {
    std::string code = R"(
        let path = "io_sandbox/dummy.txt";
        let abs = IO.abs(path);
        if (abs.length <= path.length) {
            throw "abs path too short";
        }
        IO.println(abs);
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, ReadWriteText) {
    std::string code = R"(
        let path = "io_sandbox/text.txt";
        let content = "Hello BxScript IO";
        if (!IO.write(path, content)) {
            throw "write text failed";
        }
        let readBack = IO.read(path, "utf8");
        if (readBack != content) {
            throw "content mismatch. expected: " + content + ", got: " + readBack;
        }
        readBack;
    )";
    ASSERT_IS_STRING(Eval(code), "Hello BxScript IO");
}

TEST_F(IOTest, ReadBuffer) {
    std::string code = R"(
        let path = "io_sandbox/binary.bin";
        IO.write(path, "ABC");
        let buf = IO.read(path);
        let len = buf.size;
        if (len != 3) {
            throw "buffer size error: " + len;
        }

        if (buf[0] != 65) {throw "byte 0 error";}
        if (buf[1] != 66) {throw "byte 1 error";}
        if (buf[2] != 67) {throw "byte 2 error";}

        len;
    )";
    ASSERT_IS_NUMBER(Eval(code), 3.0);
}

TEST_F(IOTest, WriteBuffer) {
    std::string code = R"(
        let path = "io_sandbox/buffer_out.bin";
        // 1. 读取一个现有的文件拿到 Buffer (复用 dummy.txt "Hello IO")
        let rawBuf = IO.read("io_sandbox/dummy.txt");
        // 2. 把这个 Buffer 写入新文件
        if (!IO.write(path, rawBuf)) {throw "write buffer failed";}
        // 3. 读取新文件验证内容
        let str = IO.read(path, "utf8");
        if (str != "Hello IO") {throw "buffer content lost";}
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(IOTest, Overwrite) {
    std::string code = R"(
        let path = "io_sandbox/overwrite.txt";
        IO.write(path, "First");
        IO.write(path, "Second");
        let res = IO.read(path, "utf8");
        if (res != "Second") {throw "overwrite failed, got: " + res;}
        true;
    )";
    ASSERT_IS_BOOL(Eval(code), true);
}

TEST_F(InterpreterTest, NetGetHttpsBasic) {
    std::string code = R"(
        let global_status = 0;
        let global_body = "";
        Net.get("https://www.baidu.com", function(res) {
            global_status = res.status;
            global_body = res.body;
            IO.println("Callback finished!");
        });
    )";
    EvalAsync(code);
    auto statusVal = GetGlobalVar("global_status");
    ASSERT_IS_NUMBER(statusVal, 200.0);
    auto bodyVal = GetGlobalVar("global_body");
    ASSERT_EQ(bodyVal->type, ValueType::STRING);
    EXPECT_FALSE(std::static_pointer_cast<StringValue>(bodyVal)->Value.empty());
}

TEST_F(InterpreterTest, NetGetJsonApi) {
    std::string code = R"(
        let resultId = -1;
        Net.get("https://jsonplaceholder.typicode.com/todos/1", function(res) {
            if (res.status == 200) {
                let data = JSON.parse(res.body);
                resultId = data.id;
            }
        });
    )";
    EvalAsync(code);
    auto statusVal = GetGlobalVar("resultId");
    ASSERT_IS_NUMBER(statusVal, 1);
}

TEST_F(InterpreterTest, NetInvalidUrl) {
    std::string code = R"(
        let errStatus = 0;
        Net.get("https://this-domain-does-not-exist-123456.com", function(res) {
            errStatus = res.status;
        });
    )";
    EvalAsync(code);
    auto statusVal = GetGlobalVar("errStatus");
    ASSERT_IS_NUMBER(statusVal, -1);
}

TEST_F(InterpreterTest, Net404) {
    std::string code = R"(
        let status = 0;
        Net.get("https://www.baidu.com/this_page_is_404", function(res) {
            status = res.status;
        });
    )";
    EvalAsync(code);
    auto statusVal = GetGlobalVar("status");
    ASSERT_IS_NUMBER(statusVal, 404);
}

TEST_F(InterpreterTest, GuiBuildStructure) {
    std::string code = R"(
        import std.Win as win;
        let f = win.form("mainForm", "Test Window", 800, 600);
        let btn = win.button("btn1", "Click", 100, 50, 10, 10);
        btn.x = 999;
        f.add(btn);
    )";
    Eval(code);
    ASSERT_EQ(GuiModule::GlobalForms.size(), 1);
    auto formObj = std::static_pointer_cast<ObjectValue>(GuiModule::GlobalForms[0]);
    auto title = formObj->Get("text");
    ASSERT_EQ(title->ToString(), "Test Window");
    auto childrenVal = formObj->Get("children");
    ASSERT_EQ(childrenVal->type, ValueType::ARRAY);
    auto childrenArr = std::static_pointer_cast<ArrayValue>(childrenVal);
    ASSERT_EQ(childrenArr->Elements.size(), 1);
    auto btnObj = std::static_pointer_cast<ObjectValue>(childrenArr->Elements[0]);
    auto xVal = btnObj->Get("x");
    ASSERT_EQ(std::static_pointer_cast<NumberValue>(xVal)->Value, 999.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
