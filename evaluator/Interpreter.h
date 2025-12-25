/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    执行器
 */

#ifndef BXSCRIPT_INTERPRETER_H
#define BXSCRIPT_INTERPRETER_H

#include <utility>

#include "Value.h"
#include "parser/Expression.h"
#include "Environment.h"
#include "common/ModuleHelper.h"
#include "parser/Parser.h"

class Interpreter {
public:
    static std::unordered_map<std::string, ValuePtr> ModuleCache;
    static std::unordered_map<std::string, std::shared_ptr<Program> > ModuleAST;
    static std::vector<std::shared_ptr<Program> > ASTRegistry;
    static std::unordered_map<std::string, ValuePtr> CppStdCache;

    // 环境预热
    static void SetupEnvironment(const std::shared_ptr<Environment>& env);

    // 公用函数执行
    static ValuePtr CallFunction(const ValuePtr &callee, const std::vector<ValuePtr> &args);

    // 运行代码
    static ValuePtr Run(const std::string &sourceCode, std::shared_ptr<Environment> globalEnv = nullptr) {
        if (!globalEnv) {
            globalEnv = std::make_shared<Environment>();
        }
        SetupEnvironment(globalEnv);
        Parser parser(sourceCode);
        auto const programPtr = std::make_shared<Program>(parser.ParseProgram());
        ASTRegistry.push_back(programPtr);
        auto res = EvaluateProgram(*programPtr, globalEnv);
        return std::move(res);
    }

    // 执行代码 -> 二级
    static ValuePtr EvaluateProgram(const Program &program, const std::shared_ptr<Environment>& env);

private:
    // Statement 执行层 (Execute): 负责逻辑控制、变量声明、代码块
    static ValuePtr Execute(Statement *stmt, const std::shared_ptr<Environment>& env);

    // Expression 求值层 (Evaluate): 负责数据计算、赋值、成员访问
    static ValuePtr Evaluate(Expression *expr, std::shared_ptr<Environment> env);

    // 字面量转Bool
    static bool IsTruthy(const ValuePtr &v);

    // 数学运算等
    static ValuePtr ApplyBinary(const Token &op, const ValuePtr &left, const ValuePtr &right);

    // 模块加载
    static void LoadModule(const ImportStatement *stmt, std::shared_ptr<Environment> env);
};

#endif //BXSCRIPT_INTERPRETER_H
