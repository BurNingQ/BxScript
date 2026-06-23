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
 * @brief    字符串对象原型链和静态函数
 */

#include <algorithm>

#include "../Value.h"
#include <cmath>
#include "error/RuntimeError.h"
#include "common/StringKit.h"
#include "../Environment.h"

class NumberValue;

bool StringValue::Equal(ValuePtr v) {
    if (v->type != ValueType::STRING) {
        return false;
    }
    auto other = std::static_pointer_cast<StringValue>(v);
    return this->Value == other->Value;
}

StringValue::StringValue(std::string v) : RuntimeValue(ValueType::STRING) {
    U32Value = StringKit::Utf8ToU32(v);
    Value = std::move(v);
}

StringValue::StringValue(char32_t v) : RuntimeValue(ValueType::STRING) {
    U32Value = std::u32string(1, v);
    Value = StringKit::Char32ToUtf8(v);
}

ValuePtr StringValue::Get(const std::string &key) {
    if (!key.empty() && std::all_of(key.begin(), key.end(), ::isdigit)) {
        const auto index = std::stoul(key);
        if (index >= this->U32Value.size()) {
            return std::make_shared<NullValue>();
        }
        return std::make_shared<StringValue>(this->U32Value[index]);
    }
    if (key == "length") {
        return std::make_shared<NumberValue>(this->U32Value.length());
    }
    if (key == "split") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::vector<ValuePtr> elements{};
            if (args.empty()) {
                elements.push_back(self);
                return std::make_shared<ArrayValue>(std::move(elements));
            }
            const std::u32string separator = StringKit::Utf8ToU32(args[0]->ToString());
            const std::u32string &target = self->U32Value;
            if (separator.empty()) {
                for (char32_t c : target) {
                    elements.push_back(std::make_shared<StringValue>(c));
                }
                return std::make_shared<ArrayValue>(std::move(elements));
            }
            size_t start = 0;
            size_t end = target.find(separator);

            while (end != std::u32string::npos) {
                std::u32string sub = target.substr(start, end - start);
                elements.push_back(std::make_shared<StringValue>(StringKit::U32ToUtf8(sub)));
                start = end + separator.length();
                end = target.find(separator, start);
            }
            std::u32string const lastSub = target.substr(start);
            elements.push_back(std::make_shared<StringValue>(StringKit::U32ToUtf8(lastSub)));
            return std::make_shared<ArrayValue>(std::move(elements));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "indexOf") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::u32string target{};
            if (args.empty()) {
                return std::make_shared<NumberValue>(-1);
            }
            if (args[0]->type == ValueType::STRING) {
                auto const arg1 = std::static_pointer_cast<StringValue>(args[0]);
                target = StringKit::Utf8ToU32(arg1->Value);
            }
            std::u32string::size_type pos = self->U32Value.find(target);
            if (pos != std::u32string::npos) {
                return std::make_shared<NumberValue>(pos);
            }
            return std::make_shared<NumberValue>(-1);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "lastIndexOf") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::u32string target{};
            if (args.empty()) {
                return std::make_shared<NumberValue>(-1);
            }
            if (args[0]->type == ValueType::STRING) {
                auto const arg1 = std::static_pointer_cast<StringValue>(args[0]);
                target = StringKit::Utf8ToU32(arg1->Value);
            }
            // 可选的 fromIndex 参数
            std::u32string::size_type startPos = std::u32string::npos;
            if (args.size() > 1 && args[1]->type == ValueType::NUMBER) {
                auto const idx = static_cast<long long>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                if (idx < 0) {
                    return std::make_shared<NumberValue>(-1);
                }
                startPos = static_cast<std::u32string::size_type>(idx);
            }
            std::u32string::size_type pos = self->U32Value.rfind(target, startPos);
            if (pos != std::u32string::npos) {
                return std::make_shared<NumberValue>(pos);
            }
            return std::make_shared<NumberValue>(-1);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "charCodeAt") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](
            const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::NUMBER) {
                return std::make_shared<NumberValue>(NAN);
            }
            const size_t index = static_cast<size_t>(std::static_pointer_cast<NumberValue>(args[0])->Value);
            if (index >= self->U32Value.size()) {
                return std::make_shared<NumberValue>(NAN);
            }
            return std::make_shared<NumberValue>(self->U32Value[index]);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "endsWith") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::STRING) {
                return std::make_shared<BoolValue>(false);
            }
            auto const target = StringKit::Utf8ToU32(std::static_pointer_cast<StringValue>(args[0])->Value);
            if (target.size() > self->U32Value.size()) {
                return std::make_shared<BoolValue>(false);
            }
            size_t start = self->U32Value.size() - target.size();
            bool result = (self->U32Value.compare(start, target.size(), target) == 0);
            return std::make_shared<BoolValue>(result);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "startsWith") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty() || args[0]->type != ValueType::STRING) {
                return std::make_shared<BoolValue>(false);
            }
            auto const target = StringKit::Utf8ToU32(std::static_pointer_cast<StringValue>(args[0])->Value);
            if (target.size() > self->U32Value.size()) {
                return std::make_shared<BoolValue>(false);
            }
            bool result = (self->U32Value.compare(0, target.size(), target) == 0);
            return std::make_shared<BoolValue>(result);
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "replace") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.size() < 2) {
                return self;
            }
            std::u32string search = StringKit::Utf8ToU32(args[0]->ToString());
            std::u32string replacement = StringKit::Utf8ToU32(args[1]->ToString());
            if (search.empty()) {
                return self;
            }
            std::u32string result = self->U32Value;
            size_t pos = 0;
            while ((pos = result.find(search, pos)) != std::u32string::npos) {
                result.replace(pos, search.size(), replacement);
                pos += replacement.size();
            }
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(result));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "toLower") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::u32string result = self->U32Value;
            for (auto &c : result) {
                if (c >= 'A' && c <= 'Z') {
                    c = c + ('a' - 'A');
                }
            }
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(result));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "toUpper") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::u32string result = self->U32Value;
            for (auto &c : result) {
                if (c >= 'a' && c <= 'z') {
                    c = c - ('a' - 'A');
                }
            }
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(result));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "trim") {
        auto self = std::static_pointer_cast<StringValue>(shared_from_this());
        auto fn = [self](const std::vector<ValuePtr> &args) -> ValuePtr {
            const std::u32string &s = self->U32Value;
            size_t start = 0;
            while (start < s.size()) {
                char32_t c = s[start];
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
                    c == '\f' || c == '\v' || c == 0x00A0 || c == 0x1680 ||
                    c == 0x2000 || c == 0x2001 || c == 0x2002 || c == 0x2003 ||
                    c == 0x2004 || c == 0x2005 || c == 0x2006 || c == 0x2007 ||
                    c == 0x2008 || c == 0x2009 || c == 0x200A || c == 0x2028 ||
                    c == 0x2029 || c == 0x202F || c == 0x205F || c == 0x3000 ||
                    c == 0xFEFF) {
                    start++;
                } else {
                    break;
                }
            }
            size_t end = s.size();
            while (end > start) {
                char32_t c = s[end - 1];
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
                    c == '\f' || c == '\v' || c == 0x00A0 || c == 0x1680 ||
                    c == 0x2000 || c == 0x2001 || c == 0x2002 || c == 0x2003 ||
                    c == 0x2004 || c == 0x2005 || c == 0x2006 || c == 0x2007 ||
                    c == 0x2008 || c == 0x2009 || c == 0x200A || c == 0x2028 ||
                    c == 0x2029 || c == 0x202F || c == 0x205F || c == 0x3000 ||
                    c == 0xFEFF) {
                    end--;
                } else {
                    break;
                }
            }
            if (start >= end) {
                return std::make_shared<StringValue>("");
            }
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(s.substr(start, end - start)));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (key == "substr") {
        auto fn = [self = std::static_pointer_cast<StringValue>(shared_from_this())]
        (const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.size() != 2) throw RuntimeError("参数错误: substr(start, end)");
            if (args.at(0)->type != ValueType::NUMBER || args.at(1)->type != ValueType::NUMBER) {
                throw RuntimeError("参数错误: substr(Number, Number)");
            }
            const auto arg1 = std::static_pointer_cast<NumberValue>(args.at(0));
            const auto arg2 = std::static_pointer_cast<NumberValue>(args.at(1));
            auto start = static_cast<long long>(arg1->Value);
            auto end = static_cast<long long>(arg2->Value);
            const size_t len = self->U32Value.size();
            if (start > len) start = len;
            if (end > len) end = len;
            if (start > end) {
                throw RuntimeError("参数错误: substr(start, end), start 不能大于 end");
            }
            size_t count = end - start;
            if (count == 0) {
                return std::make_shared<StringValue>("");
            }
            std::u32string subU32 = self->U32Value.substr(start, count);
            return std::make_shared<StringValue>(StringKit::U32ToUtf8(subU32));
        };
        return std::make_shared<NativeFunctionValue>(fn);
    }
    if (Prototype) {
        ValuePtr method = Prototype->Get(key);
        if (method) {
            if (method->type == ValueType::FUNCTION) {
                auto originalFn = std::static_pointer_cast<FunctionValue>(method);
                auto thisEnv = std::make_shared<Environment>(originalFn->Closure);
                thisEnv->DeclareVar("this", shared_from_this());
                return std::make_shared<FunctionValue>(originalFn->Declaration, thisEnv);
            }
            return method;
        }
    }

    return RuntimeValue::Get(key);
}


ValuePtr StringValue::InitBuiltins() {
    auto stringObj = std::make_shared<ObjectValue>();
    stringObj->Set("prototype", Prototype);
    const auto fromCharCodeFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            std::string resultUtf8;
            for (const auto &arg: args) {
                if (arg->type != ValueType::NUMBER) continue;
                resultUtf8 += StringKit::Char32ToUtf8(static_cast<char32_t>(std::static_pointer_cast<NumberValue>(arg)->Value));
            }
            return std::make_shared<StringValue>(resultUtf8);
        }
    );
    stringObj->Set("fromCharCode", fromCharCodeFn);
    return stringObj;
}
