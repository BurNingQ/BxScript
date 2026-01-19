/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/16
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    TimerModule
 */
#ifndef BXSCRIPT_TIMERMODULE_H
#define BXSCRIPT_TIMERMODULE_H
#include <atomic>
#include <mutex>
#include <unordered_set>

#include "evaluator/EventLoop.h"
#include "evaluator/Value.h"


class TimerModule {
    inline static std::unordered_set<int> activeTimers;
    inline static std::mutex timerMutex;
    inline static std::atomic<int> nextId{1};

    static ValuePtr BuildCommonTimerTick(bool isLoop) {
        auto const timerFn = std::make_shared<NativeFunctionValue>(
            [isLoop](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() < 2) return std::make_shared<NullValue>();
                if (args[0]->type != ValueType::FUNCTION || args[1]->type != ValueType::NUMBER) {
                    return std::make_shared<NullValue>();
                }
                auto cb = args[0];
                long ms = static_cast<long>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                (void) ms;
                int id = nextId++;
                {
                    std::lock_guard lock(timerMutex);
                    activeTimers.insert(id);
                }
                EventLoop::AddActiveTask();
                std::thread([id, ms, cb, isLoop]() {
                    while (true) {
                        bool shouldContinue = false;
                        std::this_thread::sleep_for(milliseconds(ms));
                        {
                            std::lock_guard lock(timerMutex);
                            if (auto it = activeTimers.find(id); it != activeTimers.end()) {
                                EventLoop::Enqueue(cb, {});
                                if (isLoop) {
                                    shouldContinue = true;
                                } else {
                                    shouldContinue = false;
                                    activeTimers.erase(it);
                                }
                            } else {
                                shouldContinue = false;
                            }
                        }
                        if (!shouldContinue) {
                            break;
                        }
                    }
                    EventLoop::RemoveActiveTask();
                }).detach();
                return std::make_shared<NumberValue>(id);
            });
        return timerFn;
    }

    static void InitSetTimeOut(std::shared_ptr<ObjectValue> &o) {
        o->Set("setTimeout", BuildCommonTimerTick(false));
    }


    static void InitSetInterval(std::shared_ptr<ObjectValue> &o) {
        o->Set("setInterval", BuildCommonTimerTick(true));
    }

    static void InitClearTicker(std::shared_ptr<ObjectValue> &o) {
        auto const clearFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() != 1 || args[0]->type != ValueType::NUMBER) return std::make_shared<BoolValue>(false);
                const auto id = std::static_pointer_cast<NumberValue>(args[0])->Value;
                std::lock_guard lock(timerMutex);
                if (const auto it = activeTimers.find(static_cast<int>(id)); it != activeTimers.end()) {
                    activeTimers.erase(it);
                    return std::make_shared<BoolValue>(true);
                }
                return std::make_shared<BoolValue>(false);
            });
        o->Set("clear", clearFn);
    }

public:
    static ValuePtr CreateTimerModule() {
        auto timer = std::make_shared<ObjectValue>();
        InitSetInterval(timer);
        InitSetTimeOut(timer);
        InitClearTicker(timer);
        return timer;
    }
};


#endif //BXSCRIPT_TIMERMODULE_H
