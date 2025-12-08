//
// Created by BurNingLi on 2025/12/8.
//

#include "evaluator/Value.h"

bool NullValue::Equal(ValuePtr v) {
    if (v->type != ValueType::NULL_TYPE) {
        return false;
    }
    return true;
}