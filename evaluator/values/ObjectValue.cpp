//
// Created by BurNingLi on 2025/12/3.
//

#include <algorithm>

#include "../Value.h"
#include "../Environment.h"
#include "evaluator/Logger.h"

bool ObjectValue::Equal(ValuePtr v) {
    if (v->type != ValueType::OBJECT) {
        return false;
    }
    auto obj = std::static_pointer_cast<ObjectValue>(v);
    if (this->Properties.size() != obj->Properties.size()) return false;
    for (const auto &[key, val]: this->Properties) {
        if (obj->Properties.find(key) == obj->Properties.end()) return false;
        if (!val->Equal(obj->Properties.at(key))) return false;
    }
    return true;
}

// ObjectValue
ValuePtr ObjectValue::Get(const std::string &key) {
    if (Properties.find(key) != Properties.end()) return Properties[key];

    if (Prototype && this != Prototype.get()) {
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
    return std::make_shared<NullValue>();
}

void ObjectValue::Set(const std::string &key, ValuePtr value) {
    Properties[key] = value;
}

ValuePtr ObjectValue::InitBuiltins() {
    auto objObj = std::make_shared<ObjectValue>();
    objObj->Set("prototype", Prototype);
    const auto keysFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::OBJECT) {
                Logger::Error("参数错误: Object.keys(obj)");
            }
            const auto target = std::static_pointer_cast<ObjectValue>(args[0]);
            std::vector<ValuePtr> keys;
            keys.reserve(target->Properties.size());
            if (target->Properties.empty()) {
                return std::make_shared<ArrayValue>(std::move(keys));
            }
            for (auto &[k, v]: target->Properties) {
                keys.push_back(std::make_shared<StringValue>(k));
            }
            return std::make_shared<ArrayValue>(std::move(keys));
        });
    objObj->Set("keys", keysFn);
    const auto removeKeyFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::OBJECT) {
                Logger::Error("参数错误: Object.remove(obj, [str...])");
            }
            const auto target = std::static_pointer_cast<ObjectValue>(args[0]);
            if (args.size() > 1) {
                for (size_t i = 1; i < args.size(); ++i) {
                    const auto &arg = args[i];
                    if (arg->type == ValueType::STRING) {
                        const auto argStr = std::static_pointer_cast<StringValue>(arg);
                        target->Properties.erase(argStr->Value);
                    }
                }
            }
            return std::make_shared<NullValue>();
        });
    objObj->Set("remove", removeKeyFn);
    return objObj;
}
