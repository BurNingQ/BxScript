/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/30
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    MouseModule
 */
#ifndef BXSCRIPT_MOUSEMODULE_H
#define BXSCRIPT_MOUSEMODULE_H
#include <memory>

#include "evaluator/Value.h"
#include "gui/windows/Utils.h"

class MouseModule {
    static void InitMove(std::shared_ptr<ObjectValue> o) {
        o->Set("move", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       if (args.size() < 2) return std::make_shared<NullValue>();
                       if (args[0]->type != ValueType::NUMBER || args[1]->type != ValueType::NUMBER) {
                           return std::make_shared<NullValue>();
                       }
#if defined(_WIN32)
                       const int x = static_cast<int>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                       const int y = static_cast<int>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                       Mouse::move(x, y);
#endif
                       return std::make_shared<NullValue>();
                   }
               ));
    }

    static void InitClick(std::shared_ptr<ObjectValue> o) {
        o->Set("click", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       int x, y;
#if defined(_WIN32)
                       if (args.size() >= 2) {
                           if (args[0]->type != ValueType::NUMBER || args[1]->type != ValueType::NUMBER) {
                               return std::make_shared<NullValue>();
                           }
                           x = static_cast<int>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                           y = static_cast<int>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                       } else {
                           auto [px,py] = Mouse::pos();
                           x = px;
                           y = py;
                       }
                       Mouse::click(x, y);
#endif
                       return std::make_shared<NullValue>();
                   }
               ));
    }

    static void InitDownUp(std::shared_ptr<ObjectValue> o) {
        o->Set("down", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       std::string btn = "left";
                       if (!args.empty()) btn = args[0]->ToString();
#if defined(_WIN32)
                       auto [x,y] = Mouse::pos();
                       Mouse::down(x, y, btn);
#endif
                       return std::make_shared<NullValue>();
                   }
               ));

        o->Set("up", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       std::string btn = "left";
                       if (!args.empty()) btn = args[0]->ToString();
#if defined(_WIN32)
                       auto [x,y] = Mouse::pos();
                       Mouse::up(x, y, btn);
#endif
                       return std::make_shared<NullValue>();
                   }
               ));
    }

    static void InitScroll(std::shared_ptr<ObjectValue> o) {
        o->Set("scroll", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       int delta = -120;
                       if (!args.empty() && args[0]->type == ValueType::NUMBER)
                           delta = static_cast<int>(std::static_pointer_cast<NumberValue>(args[0])->Value);
#if defined(_WIN32)
                       Mouse::scroll(delta);
#endif
                       return std::make_shared<NullValue>();
                   }
               ));
    }

    static void InitGetPos(std::shared_ptr<ObjectValue> o) {
        o->Set("getPos", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       int x, y;
#if defined(_WIN32)
                       auto [px,py] = Mouse::pos();
                       x = px;
                       y = py;
#endif
                       auto ret = std::make_shared<ObjectValue>();
                       ret->Set("x", std::make_shared<NumberValue>(x));
                       ret->Set("y", std::make_shared<NumberValue>(y));
                       return ret;
                   }
               ));
    }

public:
    static ValuePtr CreateMouseModule() {
        auto m = std::make_shared<ObjectValue>();
        InitMove(m);
        InitClick(m);
        InitDownUp(m);
        InitScroll(m);
        InitGetPos(m);
        return m;
    }
};


#endif //BXSCRIPT_MOUSEMODULE_H
