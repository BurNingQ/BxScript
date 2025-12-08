//
// Created by BurNingLi on 2025/12/3.
//

#include <algorithm>

#include "../Value.h"
#include "../Environment.h"

bool ObjectValue::Equal(ValuePtr v) {
    if (v->type != ValueType::OBJECT) {
        return false;
    }
    auto obj = std::static_pointer_cast<ObjectValue>(v);
    if (this->Properties.size() != obj->Properties.size()) return false;
    for (const auto& [key, val] : this->Properties) {
        if (obj->Properties.find(key) == obj->Properties.end()) return false;
        if (!val->Equal(obj->Properties.at(key))) return false;
    }
    return true;
}

// ObjectValue
ValuePtr ObjectValue::Get(const std::string &key) {
    if (Properties.find(key) != Properties.end()) return Properties[key];

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
    return std::make_shared<NullValue>();
}

void ObjectValue::Set(const std::string &key, ValuePtr value) {
    Properties[key] = value;
}

ValuePtr ObjectValue::InitBuiltins() {
    auto objObj = std::make_shared<ObjectValue>();
    objObj->Set("prototype", Prototype);
    return objObj;
}
