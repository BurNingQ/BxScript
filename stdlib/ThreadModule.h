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
 * @brief    线程标准库
 */

#ifndef BXSCRIPT_THREADMODULE_H
#define BXSCRIPT_THREADMODULE_H

#include "evaluator/Value.h"
#include <thread>
#include <chrono>
#include <complex>

#include "evaluator/Interpreter.h"
#include "evaluator/Logger.h"

using namespace std::chrono;

inline ValuePtr onMessageCallback = nullptr;

class ThreadModule {
    static void initSleep(const std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (!args.empty() && args[0]->type == ValueType::NUMBER) {
                    const long ms = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                    std::this_thread::sleep_for(milliseconds(ms));
                }
                return std::make_shared<NullValue>();
            }
        );
        o->Set("sleep", fn);
    }

    static void initInvoke(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::FUNCTION) {
                    Logger::Error("参数错误: Thread.invoke(fn, [args])");
                }
                auto threadFn = args[0];
                auto threadArgs = std::vector(args.begin() + 1, args.end());
                EventLoop::AddActiveTask();
                std::thread t([threadFn,threadArgs] {
                    Interpreter::CallFunction(threadFn, threadArgs);
                    EventLoop::RemoveActiveTask();
                });
                t.detach();
                std::stringstream ss;
                ss << t.get_id();
                return std::make_shared<StringValue>(ss.str());
            });
        o->Set("invoke", fn);
    }

    static void initOnMessage(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (!args.empty() && args[0]->type == ValueType::FUNCTION) {
                    onMessageCallback = args[0];
                }
                return std::make_shared<NullValue>();
            });
        o->Set("onMessage", fn);
    }

    static void initPostMessage(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<NullValue>();
                }
                EventLoop::Enqueue(onMessageCallback, args);
                return std::make_shared<NullValue>();
            });
        o->Set("postMessage", fn);
    }

public:
    static ValuePtr CreateThreadModule() {
        auto threadObj = std::make_shared<ObjectValue>();
        initSleep(threadObj);
        initInvoke(threadObj);
        initOnMessage(threadObj);
        initPostMessage(threadObj);
        return threadObj;
    }
};


#endif //BXSCRIPT_THREADMODULE_H
