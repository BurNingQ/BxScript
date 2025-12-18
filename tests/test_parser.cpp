#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

// 引入核心头文件
#include "../parser/Parser.h"
#include "../lexer/Lexer.h"
#include "../parser/Expression.h"

// ==========================================
// 1. Lexer 单元测试
// ==========================================

TEST(LexerTest, BasicTokenize) {
    // 直接传入字符串，不需要文件
    std::string code = "let a = 10";
    Lexer lexer(code);

    Token t1 = lexer.NextToken();
    EXPECT_EQ(t1.TokenValue, "let");
    EXPECT_EQ(t1._TokenType.GetEnum(), TokenKind::KEYWORD);

    Token t2 = lexer.NextToken();
    EXPECT_EQ(t2.TokenValue, "a");
    EXPECT_EQ(t2._TokenType.GetEnum(), TokenKind::IDENTITY);

    Token t3 = lexer.NextToken();
    EXPECT_EQ(t3.TokenValue, "=");

    Token t4 = lexer.NextToken();
    EXPECT_EQ(t4.TokenValue, "10");
    EXPECT_EQ(t4._TokenType.GetEnum(), TokenKind::INT);

    Token t5 = lexer.NextToken(); // ;

    Token t6 = lexer.NextToken(); // EOF
    EXPECT_EQ(t6._TokenType.GetEnum(), TokenKind::FILE_END);
}

TEST(LexerTest, StringHandling) {
    std::string code = R"(let s = "hello world")";
    Lexer lexer(code);

    lexer.NextToken(); // let
    lexer.NextToken(); // s
    lexer.NextToken(); // =

    Token strToken = lexer.NextToken();
    EXPECT_EQ(strToken.TokenValue, "hello world");
    EXPECT_EQ(strToken._TokenType.GetEnum(), TokenKind::STRING);
}

// ==========================================
// 2. Parser 单元测试
// ==========================================

// 测试辅助宏：简化 dynamic_cast 检查
#define CAST_OR_FAIL(type, ptr) \
    dynamic_cast<type*>(ptr); \
    ASSERT_NE(dynamic_cast<type*>(ptr), nullptr) << "Type mismatch or null pointer"

// 测试变量声明
TEST(ParserTest, ParseVariableStatement) {
    std::string code = "let number = 100";
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);

    // 转换 Statement -> VariableStatement
    auto* varStmt = CAST_OR_FAIL(VariableStatement, program.Body[0].get());

    ASSERT_EQ(varStmt->List.size(), 1);

    // 转换 Expression -> VariableExpression
    // 注意：VariableStatement 的 List 存的是 unique_ptr<Expression>，但实际指向 VariableExpression
    auto* varExpr = CAST_OR_FAIL(VariableExpression, varStmt->List[0].get());

    EXPECT_EQ(varExpr->Name, "number");

    // 检查初始值
    auto* initVal = CAST_OR_FAIL(NumberLiteral, varExpr->Initializer.get());
    EXPECT_EQ(initVal->Literal, "100");
}

// 测试函数声明
// TEST(ParserTest, ParseFunctionDeclaration) {
//     std::string code = "function add(a, b) { return a + b; }";
//     Parser parser(code);
//     Program program = parser.ParseProgram();
//
//     // 函数声明应该被提升到 DeclarationFuncs 中
//     ASSERT_EQ(program.DeclarationFuncs.size(), 1);
//
//     auto* funcDecl = program.DeclarationFuncs[0].get();
//     auto* funcLit = funcDecl->Function.get();
//
//     EXPECT_EQ(funcLit->Name->Name, "add");
//     ASSERT_EQ(funcLit->Parameters->Parameters.size(), 2);
// }

// 测试数学表达式优先级
// 1 + 2 * 3 应该解析为 1 + (2 * 3)
TEST(ParserTest, OperatorPrecedence) {
    std::string code = "let x = 1 + 2 * 3";
    Parser parser(code);
    Program program = parser.ParseProgram();

    auto* varStmt = CAST_OR_FAIL(VariableStatement, program.Body[0].get());
    auto* varExpr = CAST_OR_FAIL(VariableExpression, varStmt->List[0].get());

    // 顶层应该是 +
    auto* binaryAdd = CAST_OR_FAIL(BinaryExpression, varExpr->Initializer.get());
    EXPECT_EQ(binaryAdd->Operator.TokenValue, "+");

    // 左边是 1
    auto* leftNum = CAST_OR_FAIL(NumberLiteral, binaryAdd->Left.get());
    EXPECT_EQ(leftNum->Literal, "1");

    // 右边应该是 *
    auto* binaryMul = CAST_OR_FAIL(BinaryExpression, binaryAdd->Right.get());
    EXPECT_EQ(binaryMul->Operator.TokenValue, "*");
}

// 测试 IF 语句结构
TEST(ParserTest, ParseIfStatement) {
    std::string code = "if (x > 0) { let a = 1; } else { let a = 2; }";
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);
    auto* ifStmt = CAST_OR_FAIL(IfStatement, program.Body[0].get());

    ASSERT_NE(ifStmt->Condition, nullptr);
    ASSERT_NE(ifStmt->Ok, nullptr); // True 分支
    ASSERT_NE(ifStmt->Else, nullptr); // False 分支
}

// 测试错误处理
TEST(ParserTest, ParseSyntaxError) {
    // 缺少分号，或者是括号不匹配
    std::string code = "let a = (1 + 2";

    // 你的 Parser 遇到错误会抛出 runtime_error
    EXPECT_THROW({
        Parser parser(code);
        parser.ParseProgram();
    }, std::runtime_error);
}

// ==========================================
// 3. 复杂语句测试 (For, Throw, Try, Import)
// ==========================================

// 测试标准 FOR 循环
// for (let i = 0; i < 10; i++) { }
TEST(ParserTest, ParseForStatement) {
    std::string code = "for (let i = 0; i < 10; i++) { }";
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);
    auto* forStmt = CAST_OR_FAIL(ForStatement, program.Body[0].get());

    // 1. 检查初始化 (let i = 0)
    // ParseForOrForInStatement 中，let 会被封装进 SequenceExpression
    auto* initSeq = CAST_OR_FAIL(SequenceExpression, forStmt->Initializer.get());
    ASSERT_EQ(initSeq->Sequence.size(), 1);

    // 这里的 VariableExpression 可能被包装，也可能直接是 VariableExpression
    // 你的 ParseVariableDeclarationList 返回 vector<unique_ptr<Expression>> (内含 VariableExpression)
    auto* varExpr = CAST_OR_FAIL(VariableExpression, initSeq->Sequence[0].get());
    EXPECT_EQ(varExpr->Name, "i");

    // 2. 检查条件 (i < 10)
    auto* testExpr = CAST_OR_FAIL(BinaryExpression, forStmt->Test.get());
    EXPECT_EQ(testExpr->Operator.TokenValue, "<");

    // 3. 检查更新 (i++)
    auto* updateExpr = CAST_OR_FAIL(UnaryExpression, forStmt->Update.get());
    EXPECT_EQ(updateExpr->Operator.TokenValue, "++");

    // 4. 检查循环体
    CAST_OR_FAIL(BlockStatement, forStmt->Body.get());
}

// 测试 FOR-IN 循环
// for (let k in obj) { }
TEST(ParserTest, ParseForInStatement) {
    std::string code = "for (let k in obj) { }";
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);
    auto* forInStmt = CAST_OR_FAIL(ForInStatement, program.Body[0].get());

    // 检查变量 (let k)
    // 注意：你的 ParseForOrForInStatement 处理 let 时，如果检测到 in，
    // 会把 VariableDeclarationList 的第一个元素移交给 Into
    auto* varExpr = CAST_OR_FAIL(VariableExpression, forInStmt->Into.get());
    EXPECT_EQ(varExpr->Name, "k");

    // 检查源对象 (obj)
    auto* sourceExpr = CAST_OR_FAIL(Identifier, forInStmt->Source.get());
    EXPECT_EQ(sourceExpr->Name, "obj");
}

// 测试 THROW 语句
// throw "error";
TEST(ParserTest, ParseThrowStatement) {
    std::string code = R"(throw "error";)"; // 注意转义引号
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);
    auto* throwStmt = CAST_OR_FAIL(ThrowStatement, program.Body[0].get());

    auto* arg = CAST_OR_FAIL(StringLiteral, throwStmt->Argument.get());
    // 如果你的 Lexer 去除了引号，这里是 "error"，没去除则是 "\"error\""
    // 假设 Lexer 保留了原始内容（根据之前的讨论）
    EXPECT_EQ(arg->Literal, "error");
}

// 测试 TRY-CATCH-FINALLY 语句
// try { } catch (e) { } finally { }
TEST(ParserTest, ParseTryStatement) {
    std::string code = "try { let a = 1; } catch (e) { let b = 2; } finally { let c = 3; }";
    Parser parser(code);
    Program program = parser.ParseProgram();

    ASSERT_EQ(program.Body.size(), 1);
    auto* tryStmt = CAST_OR_FAIL(TryStatement, program.Body[0].get());

    // 1. Check Try Body
    CAST_OR_FAIL(BlockStatement, tryStmt->Body.get());

    // 2. Check Catch
    ASSERT_NE(tryStmt->Catch, nullptr);
    EXPECT_EQ(tryStmt->Catch->Parameter->Name, "e"); // Catch 的参数名
    CAST_OR_FAIL(BlockStatement, tryStmt->Catch->Body.get());

    // 3. Check Finally
    ASSERT_NE(tryStmt->Finally, nullptr);
    CAST_OR_FAIL(BlockStatement, tryStmt->Finally.get());
}

// 测试 IMPORT 语句
// import std.io as io;
TEST(ParserTest, ParseImportStatement) {
    std::string code = "import std.io as io;";
    Parser parser(code);
    Program program = parser.ParseProgram();

    // 注意：根据你的 Parser.cpp 逻辑，ImportStatement 被存放在 program.Imports 中
    // 而不是 program.Body 中。
    ASSERT_EQ(program.Imports.size(), 1);

    auto* importStmt = program.Imports[0].get();

    // 验证路径 std.io
    ASSERT_EQ(importStmt->Path.size(), 2);
    EXPECT_EQ(importStmt->Path[0], "std");
    EXPECT_EQ(importStmt->Path[1], "io");

    // 验证别名
    EXPECT_EQ(importStmt->AliasName, "io");
}