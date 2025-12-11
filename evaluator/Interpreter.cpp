//
// Created by BurNingLi on 2025/12/9.
//

#include "Interpreter.h"
#include "../stdlib/DateModule.h"
#include <cmath>

#include "stdlib/ThreadModule.h"

std::unordered_map<std::string, ValuePtr> Interpreter::ModuleCache;
std::unordered_map<std::string, std::shared_ptr<Program> > Interpreter::ASTCache;

void Interpreter::SetupEnvironment(std::shared_ptr<Environment> env) {
    env->DeclareVar("String", StringValue::InitBuiltins());
    env->DeclareVar("Number", NumberValue::InitBuiltins());
    env->DeclareVar("Array", ArrayValue::InitBuiltins());
    env->DeclareVar("Function", FunctionValue::InitBuiltins());
    env->DeclareVar("Object", ObjectValue::InitBuiltins());
    env->DeclareVar("Boolean", BoolValue::InitBuiltins());
    env->DeclareVar("Date", DateModule::CreateDateModule());
    env->DeclareVar("Thread", ThreadModule::CreateThreadModule());
}

ValuePtr Interpreter::CallFunction(const ValuePtr &callee, const std::vector<ValuePtr> &args) {
    if (callee->type == ValueType::NATIVE_FUNCTION) {
        auto nativeFn = std::static_pointer_cast<NativeFunctionValue>(callee);
        return nativeFn->Function(args);
    }
    if (callee->type == ValueType::FUNCTION) {
        auto fn = std::static_pointer_cast<FunctionValue>(callee);
        // 创建函数执行环境
        auto scope = std::make_shared<Environment>(fn->Closure);
        // 参数绑定
        for (size_t i = 0; i < fn->Declaration->Parameters->Parameters.size(); ++i) {
            auto paramId = dynamic_cast<Identifier *>(fn->Declaration->Parameters->Parameters[i].get());
            std::string paramName = paramId->Name;
            // 获取实参，缺省NULL
            ValuePtr argVal = (i < args.size()) ? args[i] : std::make_shared<NullValue>();
            scope->DeclareVar(paramName, argVal);
        }
        // 执行函数体
        ValuePtr result = Execute(fn->Declaration->Body.get(), scope);
        // 处理返回值
        if (result->type == ValueType::RETURN) {
            return std::static_pointer_cast<ReturnValue>(result)->Value;
        }
        return result;
    }
    throw std::runtime_error("试图调用非函数对象: " + callee->ToString());
}

ValuePtr Interpreter::EvaluateProgram(const Program &program, std::shared_ptr<Environment> env) {
    for (const auto &importStmt: program.Imports) {
        LoadModule(importStmt.get(), env);
    }
    // 函数提升
    for (const auto &stmt: program.Body) {
        if (const auto *funcStmt = dynamic_cast<FunctionStatement *>(stmt.get())) {
            FunctionLiteral *funcLit = funcStmt->Function.get();
            auto funcValue = std::make_shared<FunctionValue>(funcLit, env);
            env->DeclareVar(funcLit->Name->Name, funcValue);
        }
    }
    // 执行流程
    ValuePtr lastEvaluated = std::make_shared<NullValue>();
    for (const auto &stmt: program.Body) {
        lastEvaluated = Execute(stmt.get(), env);
    }
    return lastEvaluated;
}

ValuePtr Interpreter::Execute(Statement *stmt, std::shared_ptr<Environment> env) {
    // 变量处理
    if (const auto *varStmt = dynamic_cast<VariableStatement *>(stmt)) {
        for (const auto &decl: varStmt->List) {
            if (dynamic_cast<VariableExpression *>(decl.get())) {
                Evaluate(decl.get(), env);
            }
        }
        return std::make_shared<NullValue>();
    }
    // If 语句 (控制流)
    if (const auto *ifStmt = dynamic_cast<IfStatement *>(stmt)) {
        const ValuePtr condition = Evaluate(ifStmt->Condition.get(), env);
        if (IsTruthy(condition)) {
            return Execute(ifStmt->Ok.get(), env);
        }
        if (ifStmt->Else) {
            return Execute(ifStmt->Else.get(), env);
        }
        return std::make_shared<NullValue>();
    }
    // 代码块 { ... }
    if (const auto *block = dynamic_cast<BlockStatement *>(stmt)) {
        const auto blockEnv = std::make_shared<Environment>(env);
        ValuePtr result = std::make_shared<NullValue>();
        for (const auto &s: block->StatementList) {
            result = Execute(s.get(), blockEnv);
            if (result->type == ValueType::RETURN ||
                result->type == ValueType::BREAK ||
                result->type == ValueType::CONTINUE) {
                return result;
            }
        }
        return result;
    }
    // 表达式语句 (a = 1; 或 func();)
    if (const auto *exprStmt = dynamic_cast<ExpressionStatement *>(stmt)) {
        return Evaluate(exprStmt->Expression.get(), env);
    }
    // For 循环 (for (let i=0; i<10; i++))
    if (const auto *forStmt = dynamic_cast<ForStatement *>(stmt)) {
        const auto loopEnv = std::make_shared<Environment>(env);
        // 初始化
        if (forStmt->Initializer) {
            Evaluate(forStmt->Initializer.get(), loopEnv);
        }
        // 脚本循环
        while (true) {
            const auto iterationEnv = std::make_shared<Environment>(loopEnv);
            // 检测条件
            if (forStmt->Test) {
                ValuePtr condition = Evaluate(forStmt->Test.get(), iterationEnv);
                if (!IsTruthy(condition)) {
                    break;
                }
            }
            // 执行循环体
            ValuePtr bodyResult = Execute(forStmt->Body.get(), iterationEnv);
            if (bodyResult->type == ValueType::BREAK) {
                break;
            }
            if (bodyResult->type == ValueType::CONTINUE) {
            }
            if (bodyResult->type == ValueType::RETURN) {
                return bodyResult;
            }
            // 执行更新
            if (forStmt->Update) {
                Evaluate(forStmt->Update.get(), iterationEnv);
            }
        }
        return std::make_shared<NullValue>();
    }
    // throw
    if (const auto *throwStmt = dynamic_cast<ThrowStatement *>(stmt)) {
        ValuePtr error = Evaluate(throwStmt->Argument.get(), env);
        throw BxScriptException(error);
    }
    // try - catch
    if (const auto *tryStmt = dynamic_cast<TryStatement *>(stmt)) {
        try {
            Execute(tryStmt->Body.get(), env);
        } catch (const BxScriptException &e) {
            if (tryStmt->Catch) {
                auto catchEnv = std::make_shared<Environment>(env);
                catchEnv->DeclareVar(tryStmt->Catch->Parameter->Name, e.ErrorValue);
                Execute(tryStmt->Catch->Body.get(), catchEnv);
            }
        }
        if (tryStmt->Finally) {
            Execute(tryStmt->Finally.get(), env);
        }
        return std::make_shared<NullValue>();
    }
    // function不处理
    if (dynamic_cast<FunctionStatement *>(stmt)) {
        return std::make_shared<NullValue>();
    }
    if (const auto *retStmt = dynamic_cast<ReturnStatement *>(stmt)) {
        ValuePtr val;
        if (retStmt->Argument) {
            val = Evaluate(retStmt->Argument.get(), env);
        } else {
            val = std::make_shared<NullValue>();
        }
        return std::make_shared<ReturnValue>(val);
    }
    if (dynamic_cast<BreakStatement *>(stmt)) {
        return std::make_shared<BreakValue>();
    }
    if (dynamic_cast<ContinueStatement *>(stmt)) {
        return std::make_shared<ContinueValue>();
    }
    return std::make_shared<NullValue>();
}

ValuePtr Interpreter::Evaluate(Expression *expr, std::shared_ptr<Environment> env) {
    if (expr == nullptr) {
        return std::make_shared<NullValue>();
    }
    // 序列表达式
    if (const auto *seq = dynamic_cast<SequenceExpression *>(expr)) {
        ValuePtr result = std::make_shared<NullValue>();
        for (const auto &subExpr: seq->Sequence) {
            // 递归求值序列中的每一项
            result = Evaluate(subExpr.get(), env);
        }
        return result;
    }
    // 变量定义表达式 (let a=1)
    if (const auto *varExpr = dynamic_cast<VariableExpression *>(expr)) {
        ValuePtr value;
        if (varExpr->Initializer) {
            value = Evaluate(varExpr->Initializer.get(), env);
        } else {
            value = std::make_shared<NullValue>();
        }
        env->DeclareVar(varExpr->Name, value);
        return value;
    }
    // 数字字面量
    if (const auto *num = dynamic_cast<NumberLiteral *>(expr)) {
        return std::make_shared<NumberValue>(std::stod(num->Literal));
    }
    // 字符串字面量
    if (auto *str = dynamic_cast<StringLiteral *>(expr)) {
        return std::make_shared<StringValue>(str->Literal);
    }
    // 标识符
    if (const auto *id = dynamic_cast<Identifier *>(expr)) {
        return env->LookupVar(id->Name);
    }
    // this
    if (const auto *id = dynamic_cast<ThisExpression *>(expr)) {
        return env->LookupVar("this");
    }
    // 对象 {A: 1, B: 2}
    if (const auto *objLit = dynamic_cast<ObjectLiteral *>(expr)) {
        auto obj = std::make_shared<ObjectValue>();
        for (const auto &prop: objLit->Value) {
            const ValuePtr val = Evaluate(prop->Value.get(), env);
            obj->Set(prop->Key, val);
        }
        return obj;
    }
    // 数组字面量 [1, 2]
    if (const auto *arrLit = dynamic_cast<ArrayLiteral *>(expr)) {
        std::vector<ValuePtr> elements;
        for (const auto &elemExpr: arrLit->Value) {
            elements.push_back(Evaluate(elemExpr.get(), env));
        }
        return std::make_shared<ArrayValue>(elements);
    }
    // 二元运算 (1 + 1)
    if (const auto *bin = dynamic_cast<BinaryExpression *>(expr)) {
        std::string op = bin->Operator.TokenValue;
        if (op == "&&") {
            ValuePtr left = Evaluate(bin->Left.get(), env);
            if (!IsTruthy(left)) return left;
            return Evaluate(bin->Right.get(), env);
        }
        if (op == "||") {
            ValuePtr left = Evaluate(bin->Left.get(), env);
            if (IsTruthy(left)) return left;
            return Evaluate(bin->Right.get(), env);
        }
        const auto left = Evaluate(bin->Left.get(), env);
        const auto right = Evaluate(bin->Right.get(), env);
        return ApplyBinary(bin->Operator, left, right);
    }
    // 一元运算 (!a, -a, i++, ++i) ---
    if (const auto *unary = dynamic_cast<UnaryExpression *>(expr)) {
        const std::string &op = unary->Operator.TokenValue;
        // 自增/自减
        if (op == "++" || op == "--") {
            ValuePtr oldValue;
            ValuePtr newValue;
            auto calculate = [&](const ValuePtr &currentVal) {
                if (currentVal->type != ValueType::NUMBER) {
                    throw std::runtime_error("自增/自减只能作用于数字类型");
                }
                const double v = std::static_pointer_cast<NumberValue>(currentVal)->Value;
                const double change = (op == "++") ? 1.0 : -1.0;
                // 保存旧值 (为了后缀操作 i++)
                oldValue = currentVal;
                newValue = std::make_shared<NumberValue>(v + change); // 计算新值
            };
            // 情况 A: 变量 (i++)
            if (const auto *id = dynamic_cast<Identifier *>(unary->Operand.get())) {
                ValuePtr val = env->LookupVar(id->Name);
                calculate(val);
                env->AssignVar(id->Name, newValue); // 写回环境
            }
            // 情况 B: 对象属性 (obj.x++)
            else if (const auto *dot = dynamic_cast<DotExpression *>(unary->Operand.get())) {
                ValuePtr obj = Evaluate(dot->Left.get(), env);
                ValuePtr val = obj->Get(dot->Identifier->Name);
                calculate(val);
                obj->Set(dot->Identifier->Name, newValue); // 写回对象
            }
            // 情况 C: 数组/括号属性 (arr[0]++)
            else if (const auto *bracket = dynamic_cast<BracketExpression *>(unary->Operand.get())) {
                ValuePtr obj = Evaluate(bracket->Left.get(), env);
                ValuePtr key = Evaluate(bracket->Member.get(), env);
                ValuePtr val = obj->Get(key->ToString());
                calculate(val);
                obj->Set(key->ToString(), newValue); // 写回对象
            } else {
                throw std::runtime_error("非法的表达式Invalid L-Value for Prefix/Postfix operation");
            }
            // 如果是后缀 (i++)，返回旧值；如果是前缀 (++i)，返回新值
            return unary->Postfix ? oldValue : newValue;
        }
        // 普通一元运算
        ValuePtr val = Evaluate(unary->Operand.get(), env);
        if (op == "!") {
            return std::make_shared<BoolValue>(!IsTruthy(val));
        }
        if (op == "-") {
            if (val->type != ValueType::NUMBER) throw std::runtime_error("- 操作符只能用于数字");
            double v = std::static_pointer_cast<NumberValue>(val)->Value;
            return std::make_shared<NumberValue>(-v);
        }
        if (op == "+") {
            if (val->type != ValueType::NUMBER) throw std::runtime_error("+ 操作符只能用于数字");
            return val;
        }
        throw std::runtime_error("Unknown Unary Operator: " + op);
    }
    // 赋值操作 (Assignment)
    if (const auto *assign = dynamic_cast<AssignExpression *>(expr)) {
        // 计算右值
        ValuePtr rhs = Evaluate(assign->Right.get(), env);
        std::string op = assign->Operator.TokenValue;
        auto computeNewValue = [&](const ValuePtr &oldValue) -> ValuePtr {
            if (op == "=") return rhs;
            const std::string binOpStr = op.substr(0, op.length() - 1);
            const Token binOpToken(TokenType(TokenType::SYMBOL), binOpStr, 0, 0);
            return ApplyBinary(binOpToken, oldValue, rhs);
        };
        // 简单变量赋值 (a = 1, a += 1)
        if (const auto *id = dynamic_cast<Identifier *>(assign->Left.get())) {
            ValuePtr oldValue;
            if (op != "=") {
                oldValue = env->LookupVar(id->Name);
            }
            ValuePtr newValue = computeNewValue(oldValue);
            return env->AssignVar(id->Name, newValue);
        }
        // 成员赋值 (obj.x = 1, obj.x += 1)
        if (const auto *dot = dynamic_cast<DotExpression *>(assign->Left.get())) {
            const ValuePtr obj = Evaluate(dot->Left.get(), env);
            ValuePtr oldValue;
            if (op != "=") {
                oldValue = obj->Get(dot->Identifier->Name);
            }
            ValuePtr newValue = computeNewValue(oldValue);
            obj->Set(dot->Identifier->Name, newValue);
            return newValue;
        }
        // 索引赋值 (arr[0] = 1, arr[0] += 1)
        if (const auto *bracket = dynamic_cast<BracketExpression *>(assign->Left.get())) {
            const ValuePtr obj = Evaluate(bracket->Left.get(), env);
            const ValuePtr keyVal = Evaluate(bracket->Member.get(), env);
            std::string keyStr = keyVal->ToString();
            ValuePtr oldValue;
            if (op != "=") {
                oldValue = obj->Get(keyStr);
            }
            ValuePtr newValue = computeNewValue(oldValue);
            obj->Set(keyStr, newValue);
            return newValue;
        }
        throw std::runtime_error("无效的赋值目标");
    }
    // 成员访问 (读取)
    // 点号访问 (obj.x)
    if (const auto *dot = dynamic_cast<DotExpression *>(expr)) {
        const ValuePtr obj = Evaluate(dot->Left.get(), env);
        return obj->Get(dot->Identifier->Name);
    }
    // 括号访问 (arr[0])
    if (const auto *bracket = dynamic_cast<BracketExpression *>(expr)) {
        const ValuePtr obj = Evaluate(bracket->Left.get(), env);
        const ValuePtr key = Evaluate(bracket->Member.get(), env);
        return obj->Get(key->ToString());
    }
    // 函数调用
    if (const auto *call = dynamic_cast<CallExpression *>(expr)) {
        ValuePtr callee = Evaluate(call->Callee.get(), env);
        std::vector<ValuePtr> args;
        for (const auto &argExpr: call->ArgumentList) {
            args.push_back(Evaluate(argExpr.get(), env));
        }
        return CallFunction(callee, args);
    }
    if (auto *funcLit = dynamic_cast<FunctionLiteral *>(expr)) {
        return std::make_shared<FunctionValue>(funcLit, env);
    }
    return std::make_shared<NullValue>();
}

bool Interpreter::IsTruthy(const ValuePtr &v) {
    if (v->type == ValueType::BOOL) {
        return std::static_pointer_cast<BoolValue>(v)->Value;
    }
    if (v->type == ValueType::NUMBER) {
        return std::static_pointer_cast<NumberValue>(v)->Value != 0;
    }
    if (v->type == ValueType::NULL_TYPE) {
        return false;
    }
    if (v->type == ValueType::STRING) {
        return !std::static_pointer_cast<StringValue>(v)->Value.empty();
    }
    if (v->type == ValueType::ARRAY) {
        return !std::static_pointer_cast<ArrayValue>(v)->Elements.empty();
    }
    if (v->type == ValueType::OBJECT) {
        return !std::static_pointer_cast<ObjectValue>(v)->Properties.empty();
    }
    return true;
}

ValuePtr Interpreter::ApplyBinary(const Token &op, const ValuePtr &left, const ValuePtr &right) {
    const std::string &o = op.TokenValue;
    // 运算
    if (left->type == ValueType::NUMBER && right->type == ValueType::NUMBER) {
        const double l = std::static_pointer_cast<NumberValue>(left)->Value;
        const double r = std::static_pointer_cast<NumberValue>(right)->Value;
        if (o == "+") return std::make_shared<NumberValue>(l + r);
        if (o == "-") return std::make_shared<NumberValue>(l - r);
        if (o == "*") return std::make_shared<NumberValue>(l * r);
        if (o == "/") {
            if (r == 0) throw std::runtime_error("除数不能为0");
            return std::make_shared<NumberValue>(l / r);
        }
        if (o == "%") return std::make_shared<NumberValue>(fmod(l, r));
        // 比较
        if (o == "<") return std::make_shared<BoolValue>(l < r);
        if (o == ">") return std::make_shared<BoolValue>(l > r);
        if (o == "<=") return std::make_shared<BoolValue>(l <= r);
        if (o == ">=") return std::make_shared<BoolValue>(l >= r);
        if (o == "==") return std::make_shared<BoolValue>(l == r);
        if (o == "!=") return std::make_shared<BoolValue>(l != r);
    }
    // 字符串拼接
    if (o == "+") {
        if (left->type == ValueType::STRING || right->type == ValueType::STRING) {
            return std::make_shared<StringValue>(left->ToString() + right->ToString());
        }
    }
    // 通用相等性检查
    if (o == "==") return std::make_shared<BoolValue>(left->Equal(right));
    if (o == "!=") return std::make_shared<BoolValue>(!left->Equal(right));
    throw std::runtime_error("不支持的操作: " + left->ToString() + " " + o + " " + right->ToString());
}

void Interpreter::LoadModule(const ImportStatement *stmt, std::shared_ptr<Environment> env) {
    const std::string filePath = ModuleHelper::ResolvePath(stmt->Path);
    if (ModuleCache.find(filePath) != ModuleCache.end()) {
        env->DeclareVar(stmt->AliasName, ModuleCache[filePath]);
        return;
    }
    const std::string code = ModuleHelper::ReadFile(filePath);
    Parser parser(code);
    const auto programPtr = std::make_shared<Program>(parser.ParseProgram());
    ASTCache[filePath] = programPtr;
    const auto moduleEnv = std::make_shared<Environment>(env);
    EvaluateProgram(*programPtr, moduleEnv);
    const auto moduleObj = std::make_shared<ObjectValue>();
    for (const auto &pair: moduleEnv->variables) {
        moduleObj->Set(pair.first, pair.second);
    }
    ModuleCache[filePath] = moduleObj;
    env->DeclareVar(stmt->AliasName, moduleObj);
}
