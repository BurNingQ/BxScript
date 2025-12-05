//
// Created by BurNingLi on 2025/12/5.
//

#include "../Value.h"

ValuePtr BoolValue::InitBuiltins() {
    auto boolObj = std::make_shared<ObjectValue>();
    boolObj->Set("prototype", Prototype);
    return boolObj;
}
