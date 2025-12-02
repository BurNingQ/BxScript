#include <gtest/gtest.h>
#include <string>
#include <memory>

#include <filesystem>
#include "../parser/Parser.h"
#include "../evaluator/Interpreter.h"
#include "../evaluator/Value.h"
#include "../evaluator/Environment.h"

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
    // 辅助函数：执行代码并返回最后一个语句的结果
    ValuePtr Eval(const std::string &code) {
        Parser parser(code);
        Program program = parser.ParseProgram();
        // 全局环境
        globalEnv = std::make_shared<Environment>();
        return Interpreter::EvaluateProgram(program, globalEnv);
    }

    std::shared_ptr<Environment> globalEnv;
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
    out << "let PI = 3.14; function double(x) { return x * 2; }";
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
    ASSERT_IS_BOOL(res,true);

    res = Eval(R"("HELLO".lastIndexOf("L"))");
    ASSERT_IS_NUMBER(res, 3);
}

TEST_F(InterpreterTest, ProtoTest) {
    auto res = Eval(R"(
           let globalVar = "我是全局变量";
            String.test = function() {
                return globalVar + "X";
            };
            "abc".test();
    )");
    ASSERT_IS_STRING(res, "我是全局变量X");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
