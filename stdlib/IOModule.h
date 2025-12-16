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
 * @brief    IO标准库
 */

#ifndef BXSCRIPT_FILEMODULE_H
#define BXSCRIPT_FILEMODULE_H
#include <iostream>

#include "evaluator/Value.h"


class IOModule {
    static void InitPrintln(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                std::string output;
                for (const auto& v: args) {
                    if (!output.empty()) {
                        output += ", ";
                    }
                    output += v->ToString();
                }
                std::cout << output << std::endl;
                return std::make_shared<NullValue>();
            });
        o->Set("println", fn);
    }

public:
    static ValuePtr CreateIOModule() {
        auto module = std::make_shared<ObjectValue>();
        InitPrintln(module);
        return module;
    }
};


#endif //BXSCRIPT_FILEMODULE_H
