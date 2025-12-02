#include "Value.h"
#include <sstream>
#include <cmath>

#include "Logger.h"
#include "common/StringKit.h"

ValuePtr RuntimeValue::Get(const std::string &key) {
    Logger::Error("类型错误,不能获取属性'" + key + "' 来自: " + this->ToString());
    return std::make_shared<NullValue>();
}

void RuntimeValue::Set(const std::string &key, ValuePtr value) {
    Logger::Error("类型错误,不能设置属性'" + key + "' 来自: " + this->ToString());
}

// NumberValue
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
    return RuntimeValue::Get(key);
}

// ArrayValue
ValuePtr ArrayValue::Get(const std::string &key) {
    try {
        const size_t index = std::stoul(key);
        if (index < Elements.size()) return Elements[index];
        return std::make_shared<NullValue>();
    } catch (...) {
        if (key == "length") return std::make_shared<NumberValue>(static_cast<double>(Elements.size()));
        if (key == "push") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<ArrayValue>(shared_from_this())](
            const std::vector<ValuePtr> &args) -> ValuePtr {
                    for (const auto &arg: args) self->Elements.push_back(arg);
                    return std::make_shared<NumberValue>(static_cast<double>(self->Elements.size()));
                }
            );
        }
        if (key == "pop") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<ArrayValue>(shared_from_this())](
            const std::vector<ValuePtr> &args) -> ValuePtr {
                    if (self->Elements.empty()) return std::make_shared<NullValue>();
                    ValuePtr last = self->Elements.back();
                    self->Elements.pop_back();
                    return last;
                }
            );
        }
    }
    return RuntimeValue::Get(key);
}

void ArrayValue::Set(const std::string &key, const ValuePtr value) {
    try {
        const size_t index = std::stoul(key);
        if (index >= Elements.size()) Elements.resize(index + 1, std::make_shared<NullValue>());
        Elements[index] = value;
    } catch (...) {
        Logger::Error("数组索引必须是整数: " + key);
    }
}

// ObjectValue
ValuePtr ObjectValue::Get(const std::string &key) {
    if (Properties.find(key) != Properties.end()) return Properties[key];
    return std::make_shared<NullValue>();
}

void ObjectValue::Set(const std::string &key, ValuePtr value) {
    Properties[key] = value;
}

// StringValue
StringValue::StringValue(std::string v) : RuntimeValue(ValueType::STRING) {
    U32Value = StringKit::Utf8ToU32(v);
    Value = std::move(v);
}

StringValue::StringValue(char32_t v) : RuntimeValue(ValueType::STRING) {
    U32Value = std::u32string(1, v);
    Value = StringKit::Char32ToUtf8(v);
}

ValuePtr StringValue::Get(const std::string &key) {
    try {
        const auto index = std::stoul(key);
        if (index >= this->U32Value.size()) {
            return std::make_shared<NullValue>();
        }
        return std::make_shared<StringValue>(this->U32Value[index]);
    } catch (...) {
        if (key == "length") {
            return std::make_shared<NumberValue>(this->U32Value.length());
        }
        if (key == "indexOf") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
                    if (args.size() != 1 || args.at(0)->type != ValueType::STRING) {
                        Logger::Error("参数错误: String.indexOf(str)");
                    }
                    const auto arg = std::static_pointer_cast<StringValue>(args.at(0));
                    const auto index = self->U32Value.find(arg->U32Value);
                    if (index != std::string::npos) {
                        return std::make_shared<NumberValue>(index);
                    }
                    return std::make_shared<NumberValue>(-1);
                });
        }
        if (key == "endsWith") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
                    if (args.size() != 1 || args.at(0)->type != ValueType::STRING) {
                        Logger::Error("参数错误: String.endsWith(str)");
                    }
                    const auto arg = std::static_pointer_cast<StringValue>(args.at(0));
                    if (arg->U32Value.empty()) {
                        return std::make_shared<BoolValue>(true);
                    }
                    if (self->U32Value.empty()) {
                        return std::make_shared<BoolValue>(false);
                    }
                    return std::make_shared<BoolValue>(
                        self->U32Value.rfind(arg->U32Value) == self->U32Value.length() - arg->U32Value.length());
                });
        }
        if (key == "startsWith") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
                    if (args.size() != 1 || args.at(0)->type != ValueType::STRING) {
                        Logger::Error("参数错误: String.startsWith(str)");
                    }
                    const auto arg = std::static_pointer_cast<StringValue>(args.at(0));
                    if (arg->U32Value.empty()) {
                        return std::make_shared<BoolValue>(true);
                    }
                    if (self->U32Value.empty() || self->U32Value.length() < arg->U32Value.length()) {
                        return std::make_shared<BoolValue>(false);
                    }
                    return std::make_shared<BoolValue>(
                        self->U32Value.substr(0, arg->U32Value.length()) == arg->U32Value);
                });
        }
        if (key == "lastIndexOf") {
            return std::make_shared<NativeFunctionValue>(
                [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
                    if (args.size() != 1 || args.at(0)->type != ValueType::STRING) {
                        Logger::Error("参数错误: String.lastIndexOf(str)");
                    }
                    if (self->U32Value.empty()) {
                        return std::make_shared<NumberValue>(-1);
                    }
                    const auto arg = std::static_pointer_cast<StringValue>(args.at(0));
                    if (arg->U32Value.empty()) {
                        return std::make_shared<NumberValue>(self->U32Value.length() - 1);
                    }
                    return std::make_shared<NumberValue>(self->U32Value.find_last_of(arg->U32Value));
                });
        }
    }
    return RuntimeValue::Get(key);
}
