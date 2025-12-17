/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/16
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    BufferValue
 */

#include "../Value.h"

ValuePtr BufferValue::Get(const std::string &key) {
    try {
        size_t index = std::stoul(key);
        if (index < Buffer.size()) {
            return std::make_shared<NumberValue>(Buffer[index]);
        }
    } catch (...) {
        if (key == "length" || key == "size") {
            return std::make_shared<NumberValue>(static_cast<double>(Buffer.size()));
        }
    }
    return RuntimeValue::Get(key);
}

void BufferValue::Set(const std::string &key, ValuePtr value) {
    try {
        size_t index = std::stoul(key);
        if (index < Buffer.size() && value->type == ValueType::NUMBER) {
            const double val = std::static_pointer_cast<NumberValue>(value)->Value;
            Buffer[index] = static_cast<unsigned char>(val);
        }
    } catch (...) {}
}