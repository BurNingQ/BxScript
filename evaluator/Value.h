// Value.h
#ifndef BXSCRIPT_VALUE_H
#define BXSCRIPT_VALUE_H

#include <functional>
#include <iomanip>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "common/StringKit.h"

class ObjectValue;
// 前置声明
class RuntimeValue;
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

    [[nodiscard]] std::string ToString() const override { return "null"; }
};

class NumberValue : public RuntimeValue {
public:
    double Value;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit NumberValue(double v) : RuntimeValue(ValueType::NUMBER), Value(v) {
    }

    [[nodiscard]] std::string ToString() const override {
        std::string s = std::to_string(Value);
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }

    ValuePtr Get(const std::string &key) override;
};

class StringValue : public RuntimeValue {
public:
    std::string Value;
    std::u32string U32Value;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit StringValue(std::string v);

    explicit StringValue(char32_t v);

    ValuePtr Get(const std::string &key) override;

    [[nodiscard]] std::string ToString() const override { return Value; }
};

class BoolValue : public RuntimeValue {
public:
    bool Value;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit BoolValue(const bool v) : RuntimeValue(ValueType::BOOL), Value(v) {
    }

    [[nodiscard]] std::string ToString() const override { return Value ? "true" : "false"; }
};

class ArrayValue : public RuntimeValue {
public:
    std::vector<ValuePtr> Elements;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit ArrayValue(std::vector<ValuePtr> elements)
        : RuntimeValue(ValueType::ARRAY), Elements(std::move(elements)) {
    }

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
};

class ObjectValue final : public RuntimeValue {
public:
    std::unordered_map<std::string, ValuePtr> Properties;
    static std::shared_ptr<ObjectValue> Prototype;

    explicit ObjectValue() : RuntimeValue(ValueType::OBJECT) {
    }

    [[nodiscard]] std::string ToString() const override { return "[object Object]"; }

    ValuePtr Get(const std::string &key) override;

    void Set(const std::string &key, ValuePtr value) override;
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

    explicit FunctionValue(FunctionLiteral *decl, std::shared_ptr<Environment> closure)
        : RuntimeValue(ValueType::FUNCTION), Declaration(decl), Closure(std::move(closure)) {
    }

    [[nodiscard]] std::string ToString() const override { return "[function]"; }
};

#endif //BXSCRIPT_VALUE_H
