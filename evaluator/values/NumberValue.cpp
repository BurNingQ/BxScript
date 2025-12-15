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
 * @brief    数字对象原型链和静态函数
 */

#include <limits>

#include "../Value.h"
#include "../Logger.h"
#include "../Environment.h"

bool NumberValue::Equal(ValuePtr v) {
    if (v->type != ValueType::NUMBER) {
        return false;
    }
    auto other = std::static_pointer_cast<NumberValue>(v);
    return this->Value == other->Value;
}

ValuePtr NumberValue::Get(const std::string &key) {
    if (key == "toFixed") {
        return std::make_shared<NativeFunctionValue>(
            [self = std::static_pointer_cast<NumberValue>(shared_from_this())]
    (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() != 1) Logger::Error("toFixed参数错误: 需要1个参数");
                if (args.at(0)->type != ValueType::NUMBER) Logger::Error("toFixed参数错误: 参数必须是数字");
                const auto arg = std::static_pointer_cast<NumberValue>(args.at(0));
                const int precision = static_cast<int>(arg->Value);
                if (precision < 0 || precision > 100) Logger::Error("toFixed参数错误: 参数范围0~100");
                std::ostringstream ss;
                ss << std::fixed << std::setprecision(precision) << self->Value;
                return std::make_shared<StringValue>(ss.str());
            }
        );
    }
    if (key == "toString") {
        return std::make_shared<NativeFunctionValue>(
            [self = std::static_pointer_cast<NumberValue>(shared_from_this())]
    (const std::vector<ValuePtr> &args)-> ValuePtr {
                return std::make_shared<StringValue>(self->ToString());
            }
        );
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

ValuePtr NumberValue::InitBuiltins() {
    auto numberObj = std::make_shared<ObjectValue>();
    numberObj->Set("prototype", Prototype);
    numberObj->Set("MAX_VALUE", std::make_shared<NumberValue>(std::numeric_limits<unsigned long long>::max()));
    numberObj->Set("MIN_VALUE", std::make_shared<NumberValue>(std::numeric_limits<unsigned long long>::min()));
    return numberObj;
}
