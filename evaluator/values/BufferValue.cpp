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
#include "error/RuntimeError.h"

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

bool BufferValue::Equal(ValuePtr v) {
    if (v->type != ValueType::BUFFER) {
        return false;
    }
    const auto other = std::static_pointer_cast<BufferValue>(v);
    return this->Buffer == other->Buffer;
}

ValuePtr BufferValue::InitBuiltins() {
    auto bufferObj = std::make_shared<ObjectValue>();
    bufferObj->Set("prototype", Prototype);

    // Buffer.alloc(n) —— 创建 n 字节的空缓冲区（全 0）
    const auto allocFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::NUMBER) {
                throw RuntimeError("参数错误: Buffer.alloc(size)");
            }
            const double n = std::static_pointer_cast<NumberValue>(args[0])->Value;
            if (n < 0) {
                throw RuntimeError("Buffer.alloc: 大小不能为负");
            }
            return std::make_shared<BufferValue>(static_cast<size_t>(n));
        });
    bufferObj->Set("alloc", allocFn);

    // Buffer.from(str | number[]) —— 字符串按 UTF-8 字节，数组按字节值
    const auto fromFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty()) {
                throw RuntimeError("参数错误: Buffer.from(...)");
            }
            const auto &src = args[0];

            if (src->type == ValueType::STRING) {
                // StringValue::Value 本身就是 UTF-8 字节串，直接取
                const auto &s = std::static_pointer_cast<StringValue>(src)->Value;
                return std::make_shared<BufferValue>(
                    std::vector<unsigned char>(s.begin(), s.end()));
            }
            if (src->type == ValueType::ARRAY) {
                const auto arr = std::static_pointer_cast<ArrayValue>(src);
                std::vector<unsigned char> bytes;
                bytes.reserve(arr->Elements.size());
                for (const auto &e: arr->Elements) {
                    if (e->type != ValueType::NUMBER) {
                        throw RuntimeError("Buffer.from: 数组元素必须是数字");
                    }
                    bytes.push_back(static_cast<unsigned char>(
                        std::static_pointer_cast<NumberValue>(e)->Value));
                }
                return std::make_shared<BufferValue>(std::move(bytes));
            }
            throw RuntimeError("Buffer.from 只接受字符串或数字数组");
        });
    bufferObj->Set("from", fromFn);

    return bufferObj;
}