//
// Created by BurNingLi on 2025/12/3.
//

#include "../Value.h"

// ObjectValue
ValuePtr ObjectValue::Get(const std::string &key) {
    if (Properties.find(key) != Properties.end()) return Properties[key];
    return std::make_shared<NullValue>();
}

void ObjectValue::Set(const std::string &key, ValuePtr value) {
    Properties[key] = value;
}

void ObjectValue::InitBuiltins() {

}
