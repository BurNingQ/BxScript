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
 * @brief    值对象定义
 */

#ifndef BXSCRIPT_VALUE_H
#define BXSCRIPT_VALUE_H

#include <functional>
#include <iomanip>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class RuntimeValue;
class ObjectValue;
class FunctionLiteral;
class Environment;

using ValuePtr = std::shared_ptr<RuntimeValue>;

enum class ValueType {
    NULL_TYPE, NUMBER, STRING, BOOL, OBJECT, FUNCTION, NATIVE_FUNCTION, ARRAY, RETURN, BREAK, CONTINUE
};

class BxScriptException : public std::exception {
public:
    ValuePtr ErrorValue;

    explicit BxScriptException(ValuePtr v) : ErrorValue(std::move(v)) {
    }

    const char *what() const noexcept override { return "BxScript Runtime Exception"; }
};

// 基类
class RuntimeValue : public std::enable_shared_from_this<RuntimeValue> {
public:
    ValueType type;

    explicit RuntimeValue(const ValueType t) : type(t) {
    }

    virtual ~RuntimeValue() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    virtual ValuePtr Get(const std::string &key);

    virtual void Set(const std::string &key, ValuePtr value);

    virtual bool Equal(ValuePtr v);
};

// 原生函数
using NativeFunctionType = std::function<ValuePtr(const std::vector<ValuePtr> &)>;

class NativeFunctionValue : public RuntimeValue {
public:
    NativeFunctionType Function;

    explicit NativeFunctionValue(NativeFunctionType func)
        : RuntimeValue(ValueType::NATIVE_FUNCTION), Function(std::move(func)) {
    }

    [[nodiscard]] std::string ToString() const override { return "[native code]"; }
};

class NullValue : public RuntimeValue {
public:
    explicit NullValue() : RuntimeValue(ValueType::NULL_TYPE) {
    }

    bool Equal(ValuePtr v) override;

    [[nodiscard]] std::string ToString() const override { return "null"; }
};

class NumberValue : public RuntimeValue {
public:
    double Value;
    static std::shared_ptr<ObjectValue> Prototype;

    static ValuePtr InitBuiltins();

    explicit NumberValue(double v) : RuntimeValue(ValueType::NUMBER), Value(v) {
    }

    [[nodiscard]] std::string ToString() const override {
        std::string s = std::to_string(Value);
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }

    ValuePtr Get(const std::string &key) override;

    bool Equal(ValuePtr v) override;
};

class StringValue : public RuntimeValue {
public:
    std::string Value;
    std::u32string U32Value;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit StringValue(std::string v);

    explicit StringValue(char32_t v);

    static ValuePtr InitBuiltins();

    ValuePtr Get(const std::string &key) override;

    [[nodiscard]] std::string ToString() const override { return Value; }

    bool Equal(ValuePtr v) override;
};

class BoolValue : public RuntimeValue {
public:
    bool Value;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit BoolValue(const bool v) : RuntimeValue(ValueType::BOOL), Value(v) {
    }

    static ValuePtr InitBuiltins();

    bool Equal(ValuePtr v) override;

    [[nodiscard]] std::string ToString() const override { return Value ? "true" : "false"; }
};

class ArrayValue : public RuntimeValue {
public:
    std::vector<ValuePtr> Elements;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit ArrayValue(std::vector<ValuePtr> elements)
        : RuntimeValue(ValueType::ARRAY), Elements(std::move(elements)) {
    }

    static ValuePtr InitBuiltins();

    [[nodiscard]] std::string ToString() const override {
        std::string str = "[";
        for (size_t i = 0; i < Elements.size(); ++i) {
            str += Elements[i]->ToString();
            if (i < Elements.size() - 1) str += ", ";
        }
        str += "]";
        return str;
    }

    ValuePtr Get(const std::string &key) override;

    void Set(const std::string &key, ValuePtr value) override;

    bool Equal(ValuePtr v) override;
};

class ObjectValue final : public RuntimeValue {
public:
    std::unordered_map<std::string, ValuePtr> Properties;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit ObjectValue() : RuntimeValue(ValueType::OBJECT) {
    }

    static ValuePtr InitBuiltins();

    [[nodiscard]] std::string ToString() const override { return "[object Object]"; }

    ValuePtr Get(const std::string &key) override;

    void Set(const std::string &key, ValuePtr value) override;

    bool Equal(ValuePtr v) override;
};

class ReturnValue : public RuntimeValue {
public:
    ValuePtr Value;

    explicit ReturnValue(ValuePtr v) : RuntimeValue(ValueType::RETURN), Value(std::move(v)) {
    }

    [[nodiscard]] std::string ToString() const override { return Value->ToString(); }

};

class BreakValue : public RuntimeValue {
public:
    explicit BreakValue() : RuntimeValue(ValueType::BREAK) {
    }

    [[nodiscard]] std::string ToString() const override { return "break"; }

};

class ContinueValue : public RuntimeValue {
public:
    explicit ContinueValue() : RuntimeValue(ValueType::CONTINUE) {
    }

    [[nodiscard]] std::string ToString() const override { return "continue"; }
};

class FunctionValue : public RuntimeValue {
public:
    FunctionLiteral *Declaration;
    std::shared_ptr<Environment> Closure;
    static std::shared_ptr<ObjectValue> Prototype;

    static ValuePtr InitBuiltins();

    explicit FunctionValue(FunctionLiteral *decl, std::shared_ptr<Environment> closure)
        : RuntimeValue(ValueType::FUNCTION), Declaration(decl), Closure(std::move(closure)) {
    }

    [[nodiscard]] std::string ToString() const override { return "[function]"; }
};

#endif //BXSCRIPT_VALUE_H
