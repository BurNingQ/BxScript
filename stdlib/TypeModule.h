/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/3/31
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    TypeModule
 */
#ifndef BXSCRIPT_TYPEMODULE_H
#define BXSCRIPT_TYPEMODULE_H
#include "error/RuntimeError.h"
#include "evaluator/Value.h"

class TypeModule {
    static void initOf(const std::shared_ptr<ObjectValue> &o) {
        const auto ofFn = std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args)-> ValuePtr {
            if (args.empty()) {
                throw RuntimeError("Type.of(),参数不能为空");
            }
            switch (args[0]->type) {
                case ValueType::NUMBER: return std::make_shared<StringValue>("number");
                case ValueType::STRING: return std::make_shared<StringValue>("string");
                case ValueType::BOOL: return std::make_shared<StringValue>("boolean");
                case ValueType::ARRAY: return std::make_shared<StringValue>("array");
                case ValueType::OBJECT: return std::make_shared<StringValue>("object");
                case ValueType::FUNCTION: return std::make_shared<StringValue>("function");
                case ValueType::NATIVE_FUNCTION: return std::make_shared<StringValue>("native function");
                case ValueType::NULL_TYPE: return std::make_shared<StringValue>("null");
                case ValueType::BUFFER: return std::make_shared<StringValue>("buffer");
                default: return std::make_shared<StringValue>("unknown");
            }
        });
        o->Set("of", ofFn);
    }

    static std::shared_ptr<NativeFunctionValue> isFuncFactory(const ValueType targetType, const std::string &funcName) {
        const auto isFunc = std::make_shared<NativeFunctionValue>([funcName,targetType](const std::vector<ValuePtr> &args)-> ValuePtr {
            if (args.empty()) {
                throw RuntimeError("Type." + funcName + "(),参数不能为空");
            }
            if (targetType == ValueType::FUNCTION) {
                bool isFuncResult = (args[0]->type == ValueType::FUNCTION || args[0]->type == ValueType::NATIVE_FUNCTION);
                return std::make_shared<BoolValue>(isFuncResult);
            }
            return std::make_shared<BoolValue>(args[0]->type == targetType);
        });
        return isFunc;
    }

    static void initIsFunc(const std::shared_ptr<ObjectValue> &o, const std::string &funcName, const ValueType targetType) {
        o->Set(funcName, isFuncFactory(targetType, funcName));
    }

public:
    static ValuePtr CreateTypeModule() {
        auto typeObj = std::make_shared<ObjectValue>();
        initOf(typeObj);

        initIsFunc(typeObj,"isString", ValueType::STRING);
        initIsFunc(typeObj,"isBool", ValueType::BOOL);
        initIsFunc(typeObj,"isNull", ValueType::NULL_TYPE);
        initIsFunc(typeObj,"isBuffer", ValueType::BUFFER);
        initIsFunc(typeObj,"isNumber", ValueType::NUMBER);
        initIsFunc(typeObj,"isArray", ValueType::ARRAY);
        initIsFunc(typeObj,"isObject", ValueType::OBJECT);

        initIsFunc(typeObj,"isFunction", ValueType::FUNCTION);
        initIsFunc(typeObj,"isNativeFunction", ValueType::NATIVE_FUNCTION);
        return typeObj;
    }
};


#endif //BXSCRIPT_TYPEMODULE_H
