/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/14
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    RuntimeError
 */
#ifndef BXSCRIPT_RUNTIMEERROR_H
#define BXSCRIPT_RUNTIMEERROR_H
#include <exception>
#include <string>
#include <utility>

#include "evaluator/Value.h"
#include "parser/Expression.h"

class RuntimeError : public std::exception {
public:
    std::string Message;
    mutable const Expression *Node;
    ValuePtr ActualValue;

    explicit RuntimeError(std::string msg, const Expression *node = nullptr, ValuePtr val = nullptr)
        : Message(std::move(msg)), Node(node), ActualValue(std::move(val)) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return Message.c_str();
    }
};

static std::string FormatRuntimeError(const RuntimeError &e) {
    std::stringstream ss;
    ss << "[运行错误] " << e.Message;
    if (e.Node) {
        if (const auto dot = dynamic_cast<const DotExpression *>(e.Node)) {
            ss << ": 属性 '" << dot->Identifier->Name << "'";
        } else if (const auto id = dynamic_cast<const Identifier *>(e.Node)) {
            ss << ": 变量 '" << id->Name << "'";
        } else if (dynamic_cast<const BracketExpression *>(e.Node)) {
            ss << ": 索引 ";
        } else if (const auto call = dynamic_cast<const CallExpression *>(e.Node)) {
            std::string funcName = call->Callee->ToSource();
            if (funcName.length() > 20) funcName = funcName.substr(0, 17) + "...";
            ss << ": 调用 '" << funcName << "(...)'";
        } else if (const auto bin = dynamic_cast<const BinaryExpression *>(e.Node)) {
            ss << ": 运算 '" << bin->Operator.TokenValue << "'";
        } else {
            if (!e.Node->ToSource().empty()) {
                ss << " [触发节点: " << e.Node->ToSource() << "]";
            } else {
                ss << " (在表达式处)";
            }
        }
    }
    if (e.ActualValue) {
        ss << " (实际类型: " << e.ActualValue->ToString() << ")";
    }
    return ss.str();
}


#endif //BXSCRIPT_RUNTIMEERROR_H
