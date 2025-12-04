//
// Created by BurNingLi on 2025/12/3.
//
#include <limits>

#include "../Value.h"
#include "../Logger.h"

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
    return RuntimeValue::Get(key);
}

void NumberValue::InitBuiltins() {
    Prototype->Set("MAX_VALUE", std::make_shared<NumberValue>(std::numeric_limits<unsigned long long>::max()));
    Prototype->Set("MIN_VALUE", std::make_shared<NumberValue>(std::numeric_limits<unsigned long long>::min()));
}
