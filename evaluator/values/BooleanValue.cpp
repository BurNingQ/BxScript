/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Bool内置定义原型链等
 */

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
