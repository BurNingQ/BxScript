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
 * @brief    字符串对象原型链和静态函数
 */

#include <algorithm>

#include "../Value.h"
#include <cmath>
#include "../Logger.h"
#include "common/StringKit.h"
#include "../Environment.h"

class NumberValue;

bool StringValue::Equal(ValuePtr v) {
    if (v->type != ValueType::STRING) {
        return false;
    }
    auto other = std::static_pointer_cast<StringValue>(v);
    return this->Value == other->Value;
}

StringValue::StringValue(std::string v) : RuntimeValue(ValueType::STRING) {
    U32Value = StringKit::Utf8ToU32(v);
    Value = std::move(v);
}

StringValue::StringValue(char32_t v) : RuntimeValue(ValueType::STRING) {
    U32Value = std::u32string(1, v);
    Value = StringKit::Char32ToUtf8(v);
}

ValuePtr StringValue::Get(const std::string &key) {
    if (!key.empty() && std::all_of(key.begin(), key.end(), ::isdigit)) {
        const auto index = std::stoul(key);
        if (index >= this->U32Value.size()) {
            return std::make_shared<NullValue>();
        }
        return std::make_shared<StringValue>(this->U32Value[index]);
    }
    if (key == "length") {
        return std::make_shared<NumberValue>(this->U32Value.length());
    }
    if (key == "charCodeAt") {
        auto fn = [self = std::static_pointer_cast<StringValue>(shared_from_this())](
            const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::NUMBER) {
                return std::make_shared<NumberValue>(NAN);
            }
            const size_t index = static_cast<size_t>(std::static_pointer_cast<NumberValue>(args[0])->Value);
            if (index >= self->U32Value.size()) {
                return std::make_shared<NumberValue>(NAN);
            }
            return std::make_shared<NumberValue>(self->U32Value[index]);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "substr") {
        auto fn = [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.size() != 2) Logger::Error("参数错误: substr(start, end)");
            if (args.at(0)->type != ValueType::NUMBER || args.at(1)->type != ValueType::NUMBER) {
                Logger::Error("参数错误: substr(Number, Number)");
            }
            const auto arg1 = std::static_pointer_cast<NumberValue>(args.at(0));
            const auto arg2 = std::static_pointer_cast<NumberValue>(args.at(1));
            auto start = static_cast<long long>(arg1->Value);
            auto end = static_cast<long long>(arg2->Value);
            const size_t len = self->U32Value.size();
            if (start > len) start = len;
            if (end > len) end = len;
            if (start > end) {
                Logger::Error("参数错误: substr(start, end), start 不能大于 end");
            }
            size_t count = end - start;
            if (count == 0) {
                return std::make_shared<StringValue>("");
            }
            std::u32string subU32 = self->U32Value.substr(start, count);
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(subU32));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (Prototype) {
        ValuePtr method = Prototype->Get(key);
        if (method) {
            if (method->type == ValueType::FUNCTION) {
                auto originalFn = std::static_pointer_cast<FunctionValue>(method);
                auto thisEnv = std::make_shared<Environment>(originalFn->Closure);
                thisEnv->DeclareVar("this", shared_from_this());
                return std::make_shared<FunctionValue>(originalFn->Declaration, thisEnv);
            }
            return method;
        }
    }

    return RuntimeValue::Get(key);
}


ValuePtr StringValue::InitBuiltins() {
    auto stringObj = std::make_shared<ObjectValue>();
    stringObj->Set("prototype", Prototype);
    const auto fromCharCodeFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::string resultUtf8;
            for (const auto &arg: args) {
                if (arg->type != ValueType::NUMBER) continue;
                resultUtf8 += StringKit::Char32ToUtf8(
                    static_cast<char32_t>(std::static_pointer_cast<NumberValue>(arg)->Value));
            }
            return std::make_shared<StringValue>(resultUtf8);
        }
    );
    stringObj->Set("fromCharCode", fromCharCodeFn);
    return stringObj;
}
