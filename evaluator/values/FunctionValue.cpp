//
// Created by BurNingLi on 2025/12/3.
//

#include "evaluator/Value.h"
ValuePtr FunctionValue::InitBuiltins() {
    auto funObj = std::make_shared<ObjectValue>();
    funObj->Set("prototype", Prototype);
    return funObj;
}
