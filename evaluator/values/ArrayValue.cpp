//
// Created by BurNingLi on 2025/12/3.
//
#include "../Value.h"
#include <cmath>
#include "../Logger.h"

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

void ArrayValue::InitBuiltins() {

}
