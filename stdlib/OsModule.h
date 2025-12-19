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
 * @brief    OsModule
 */
#ifndef BXSCRIPT_OSMODULE_H
#define BXSCRIPT_OSMODULE_H
#include "evaluator/Logger.h"
#include "evaluator/Value.h"


class OsModule {
    static void InitExec(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) return std::make_shared<StringValue>("");
                const std::string cmd = args[0]->ToString();
                std::string result;
                std::array<char, 128> buffer{};
#ifdef _WIN32
                std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
#else
                std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
#endif
                if (!pipe) {
                    Logger::Error("执行命令失败: " + cmd);
                }
                while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                    result += buffer.data();
                }
                return std::make_shared<StringValue>(result);
            }
        );
        o->Set("exec", fn);
    }

    static void InitGetEnv(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) return std::make_shared<NullValue>();
                const char *val = std::getenv(args[0]->ToString().c_str());
                if (val == nullptr) return std::make_shared<NullValue>();
                return std::make_shared<StringValue>(val);
            }
        );
        o->Set("getEnv", fn);
    }

public:
    static ValuePtr CreateOSModule() {
        auto os = std::make_shared<ObjectValue>();
        InitExec(os);
        InitGetEnv(os);
#ifdef _WIN32
        os->Set("platform", std::make_shared<StringValue>("windows"));
#else
        os->Set("platform", std::make_shared<StringValue>("linux"));
#endif
        return os;
    }
};


#endif //BXSCRIPT_OSMODULE_H
