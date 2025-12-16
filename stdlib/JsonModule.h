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
 * @brief    JSON标准库(主要使用nlohmann/json)
 */

#ifndef BXSCRIPT_JSONMODULE_H
#define BXSCRIPT_JSONMODULE_H
#include "evaluator/Value.h"
#include <libs/json.hpp>

#include "evaluator/Logger.h"

class JsonModule {
    static ValuePtr JsonToValue(const nlohmann::json &j) {
        if (j.is_null()) {
            return std::make_shared<NullValue>();
        }
        if (j.is_boolean()) {
            return std::make_shared<BoolValue>(j.get<bool>());
        }
        if (j.is_number()) {
            return std::make_shared<NumberValue>(j.get<double>());
        }
        if (j.is_string()) {
            return std::make_shared<StringValue>(j.get<std::string>());
        }
        if (j.is_array()) {
            std::vector<ValuePtr> elements;
            elements.reserve(j.size());
            for (const auto &element: j) {
                elements.push_back(JsonToValue(element));
            }
            return std::make_shared<ArrayValue>(std::move(elements));
        }
        if (j.is_object()) {
            auto obj = std::make_shared<ObjectValue>();
            for (auto &[key, val]: j.items()) {
                obj->Set(key, JsonToValue(val));
            }
            return obj;
        }
        return std::make_shared<NullValue>();
    }

    static nlohmann::json ValueToJson(const ValuePtr &v) {
        if (v->type == ValueType::NULL_TYPE) return nullptr;
        if (v->type == ValueType::BOOL) {
            return std::static_pointer_cast<BoolValue>(v)->Value;
        }
        if (v->type == ValueType::NUMBER) {
            return std::static_pointer_cast<NumberValue>(v)->Value;
        }
        if (v->type == ValueType::STRING) {
            return std::static_pointer_cast<StringValue>(v)->Value;
        }
        if (v->type == ValueType::ARRAY) {
            const auto arr = std::static_pointer_cast<ArrayValue>(v);
            nlohmann::json j = nlohmann::json::array();
            for (const auto &elem: arr->Elements) {
                j.push_back(ValueToJson(elem));
            }
            return j;
        }
        if (v->type == ValueType::OBJECT) {
            const auto obj = std::static_pointer_cast<ObjectValue>(v);
            nlohmann::json j = nlohmann::json::object();
            for (const auto &[key, val]: obj->Properties) {
                if (val->type != ValueType::FUNCTION && val->type != ValueType::NATIVE_FUNCTION) {
                    j[key] = ValueToJson(val);
                }
            }
            return j;
        }
        return nullptr;
    }

    static void initParse(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::STRING) {
                    Logger::Error("参数错误: JSON.parse(jsonString)");
                }
                try {
                    auto arg = std::static_pointer_cast<StringValue>(args[0]);
                    auto j = nlohmann::json::parse(arg->Value);
                    return JsonToValue(j);
                } catch (const nlohmann::json::parse_error &e) {
                    std::stringstream ss;
                    ss << "JSON 解析失败 (byte " << e.byte << "): " << e.what();
                    Logger::Error(ss.str());
                }
                return std::make_shared<NullValue>();
            });
        o->Set("parse", fn);
    }

    static void initStringify(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) return std::make_shared<StringValue>("");
                const nlohmann::json j = ValueToJson(args[0]);
                return std::make_shared<StringValue>(j.dump());
            }
        );
        o->Set("stringify", fn);
    }

public:
    static ValuePtr CreateJsonModule() {
        auto jsonObj = std::make_shared<ObjectValue>();
        initParse(jsonObj);
        initStringify(jsonObj);
        return jsonObj;
    }
};
#endif //BXSCRIPT_JSONMODULE_H
