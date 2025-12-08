//
// Created by BurNingLi on 2025/12/5.
//

#include "../Value.h"

bool BoolValue::Equal(ValuePtr v) {
    if (v->type != ValueType::BOOL) {
        return false;
    }
    auto other = std::static_pointer_cast<BoolValue>(v);
    return this->Value == other->Value;
}

ValuePtr BoolValue::InitBuiltins() {
    auto boolObj = std::make_shared<ObjectValue>();
    boolObj->Set("prototype", Prototype);
    return boolObj;
}
