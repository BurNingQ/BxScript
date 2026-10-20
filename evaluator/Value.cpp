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
 * @brief    通用值实现
 */

#include "Value.h"
#include "error/RuntimeError.h"

std::shared_ptr<ObjectValue> StringValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> NumberValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> BoolValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> ArrayValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> FunctionValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> ObjectValue::Prototype = std::make_shared<ObjectValue>();
std::shared_ptr<ObjectValue> BufferValue::Prototype = std::make_shared<ObjectValue>();
ScriptExecutorType ObjectValue::GlobalExecutor = nullptr;

ValuePtr RuntimeValue::Get(const std::string &key) {
    throw RuntimeError("类型错误,不能获取属性'" + key + "' 来自: " + this->ToString());
    return std::make_shared<NullValue>();
}

void RuntimeValue::Set(const std::string &key, ValuePtr value) {
    throw RuntimeError("类型错误,不能设置属性'" + key + "' 来自: " + this->ToString());
}

bool RuntimeValue::Equal(ValuePtr v) {
    return false;
}

namespace {
    constexpr int kMaxEqualDepth = 1000;

    bool EqualImpl(const ValuePtr &a, const ValuePtr &b, const int depth) {
        if (a.get() == b.get()) return true;
        if (!a || !b) return false;
        if (a->type != b->type) return false;

        if (depth > kMaxEqualDepth) {
            throw RuntimeError("比较嵌套过深（可能存在循环引用）");
        }

        if (a->type == ValueType::OBJECT) {
            const auto x = std::static_pointer_cast<ObjectValue>(a);
            const auto y = std::static_pointer_cast<ObjectValue>(b);
            if (x->Properties.size() != y->Properties.size()) return false;
            for (const auto &[key, val]: x->Properties) {
                const auto it = y->Properties.find(key);
                if (it == y->Properties.end()) return false;
                if (!EqualImpl(val, it->second, depth + 1)) return false;
            }
            return true;
        }

        if (a->type == ValueType::ARRAY) {
            const auto x = std::static_pointer_cast<ArrayValue>(a);
            const auto y = std::static_pointer_cast<ArrayValue>(b);
            if (x->Elements.size() != y->Elements.size()) return false;
            for (size_t i = 0; i < x->Elements.size(); ++i) {
                if (!EqualImpl(x->Elements[i], y->Elements[i], depth + 1)) return false;
            }
            return true;
        }

        return a->Equal(b);
    }
}

bool ValueEqual(const ValuePtr &a, const ValuePtr &b) {
    return EqualImpl(a, b, 0);
}