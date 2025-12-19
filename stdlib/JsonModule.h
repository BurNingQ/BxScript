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

#include "common/JsonKit.h"
#include "evaluator/Logger.h"

class JsonModule {
    static void initParse(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::STRING) {
                    Logger::Error("参数错误: JSON.parse(jsonString)");
                }
                try {
                    auto arg = std::static_pointer_cast<StringValue>(args[0]);
                    auto j = nlohmann::json::parse(arg->Value);
                    return JsonKit::JsonToValue(j);
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
                const nlohmann::json j = JsonKit::ValueToJson(args[0]);
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
