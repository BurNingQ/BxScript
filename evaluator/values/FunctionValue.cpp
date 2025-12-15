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
 * @brief    函数对象和原型链
 */

#include "evaluator/Value.h"

ValuePtr FunctionValue::InitBuiltins() {
    auto funObj = std::make_shared<ObjectValue>();
    funObj->Set("prototype", Prototype);
    return funObj;
}
