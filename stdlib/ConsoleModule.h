/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/3/11
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    ConsoleModule
 */
#ifndef BXSCRIPT_CONSOLEMODULE_H
#define BXSCRIPT_CONSOLEMODULE_H
#include <iostream>
#include <string>
#include "../evaluator/Value.h"
#include "../common/StringKit.h"
#include "gui/windows/Utils.h"

class ConsoleModule {
    static void InitPrint(std::shared_ptr<ObjectValue> &o) {
        o->Set("print", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args)-> ValuePtr {
            std::string output;
            for (const auto &v: args) {
                if (!output.empty()) output += " ";
                output += v->ToString();
            }
            std::cout << output;
            std::cout.flush();
            return std::make_shared<NullValue>();
        }));
    }

    static void InitPrintln(std::shared_ptr<ObjectValue> &o) {
        o->Set("println", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args)-> ValuePtr {
            std::string output;
            for (const auto &v: args) {
                if (!output.empty()) output += " ";
                output += v->ToString();
            }
            std::cout << output << std::endl;
            return std::make_shared<NullValue>();
        }));
    }

    static void InitInput(std::shared_ptr<ObjectValue> &o) {
        o->Set("input", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &args)-> ValuePtr {
            if (!args.empty()) {
                std::cout << args[0]->ToString();
                std::cout.flush();
            }
            std::string input;
            std::getline(std::cin, input);
            return std::make_shared<StringValue>(input);
        }));
    }

    static void InitClear(std::shared_ptr<ObjectValue> &o) {
        o->Set("clear", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &)-> ValuePtr {
            std::cout << "\033[2J\033[1;1H";
            std::cout.flush();
            return std::make_shared<NullValue>();
        }));
    }

    static void InitShow(std::shared_ptr<ObjectValue> &o) {
        o->Set("show", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &)-> ValuePtr {
#ifdef _WIN32
            showConsole();
#endif
            return std::make_shared<NullValue>();
        }));
    }

    static void InitHide(std::shared_ptr<ObjectValue> &o) {
        o->Set("hide", std::make_shared<NativeFunctionValue>([](const std::vector<ValuePtr> &)-> ValuePtr {
#ifdef _WIN32
            hideConsole();
#endif
            return std::make_shared<NullValue>();
        }));
    }

    static void InitTitle(std::shared_ptr<ObjectValue> &o) {
        o->Set("title", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args)-> ValuePtr {
                       if (!args.empty()) {
#ifdef _WIN32
                           const std::wstring wTitle = StringKit::U8ToU16(args[0]->ToString());
                           setConsoleTitle(wTitle);
#endif
                       }
                       return std::make_shared<NullValue>();
                   }));
    }

public:
    static ValuePtr CreateConsoleModule() {
        auto module = std::make_shared<ObjectValue>();
        InitPrint(module);
        InitPrintln(module);
        InitInput(module);
        InitShow(module);
        InitHide(module);
        InitClear(module);
        InitTitle(module);
        return module;
    }
};


#endif //BXSCRIPT_CONSOLEMODULE_H
