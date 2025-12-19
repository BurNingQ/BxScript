/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/18
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    RegexModule
 */
#ifndef BXSCRIPT_REGEXMODULE_H
#define BXSCRIPT_REGEXMODULE_H
#include <memory>
#include <regex>

#include "evaluator/Logger.h"
#include "evaluator/Value.h"


class RegexModule {
    static void InitMatch(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() < 2) return std::make_shared<BoolValue>(false);
                const std::string text = args[0]->ToString();
                const std::string pattern = args[1]->ToString();
                try {
                    const std::regex re(pattern);
                    return std::make_shared<BoolValue>(std::regex_search(text, re));
                } catch (const std::regex_error &e) {
                    Logger::Error("正则表达式错误: " + std::string(e.what()));
                }
                return std::make_shared<BoolValue>(false);
            }
        );
        o->Set("match", fn);
    }

    static void InitReplace(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                // 格式Regex.replace(source, pattern, target)
                if (args.size() < 3) return std::make_shared<BoolValue>(false);
                const auto source = args[0]->ToString();
                const auto pattern = args[1]->ToString();
                const auto target = args[2]->ToString();
                try {
                    const std::regex re(pattern);
                    auto r = std::regex_replace(source, re, target);
                    return std::make_shared<StringValue>(r);
                } catch (const std::regex_error &e) {
                    Logger::Error("正则表达式错误: " + std::string(e.what()));
                }
                return std::make_shared<StringValue>("");
            });
        o->Set("replace", fn);
    }

public:
    static ValuePtr CreateRegexModule() {
        auto module = std::make_shared<ObjectValue>();
        InitMatch(module);
        InitReplace(module);
        return module;
    }
};


#endif //BXSCRIPT_REGEXMODULE_H
