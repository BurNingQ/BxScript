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
#include <atomic>

#include "evaluator/Value.h"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "evaluator/Interpreter.h"
#include "evaluator/Logger.h"

using namespace std::chrono;

// 消息模型
struct Message {
    ValuePtr data;
};

inline std::queue<Message> msgQueue{};
inline std::mutex queueMutex{};
inline std::condition_variable noticer{};
inline std::atomic<bool> isRunning{true};
// 回调
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

    static void initStart(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::FUNCTION) {
                    Logger::Error("参数错误: Thread.start(fn, [args])");
                }
                auto threadFn = args[0];
                auto threadArgs = std::vector(args.begin() + 1, args.end());
                std::thread t([threadFn,threadArgs] {
                    Interpreter::CallFunction(threadFn, threadArgs);
                });
                t.detach();
                std::stringstream ss;
                ss << t.get_id();
                return std::make_shared<StringValue>(ss.str());
            });
        o->Set("start", fn);
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
                {
                    std::lock_guard lock(queueMutex);
                    msgQueue.push({args[0]});
                }
                noticer.notify_one();
                return std::make_shared<NullValue>();
            });
        o->Set("postMessage", fn);
    }

    static void initWait(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                isRunning = true;
                while (true) {
                    Message msg{};
                    {
                        std::unique_lock lock(queueMutex);
                        noticer.wait(lock, [] { return !msgQueue.empty() || !isRunning; });
                        if (!isRunning && msgQueue.empty()) {
                            break;
                        }
                        if (!msgQueue.empty()) {
                            msg = msgQueue.front();
                            msgQueue.pop();
                        }
                    }
                    if (msg.data && onMessageCallback) {
                        std::vector callbackArgs = {msg.data};
                        Interpreter::CallFunction(onMessageCallback, callbackArgs);
                    }
                }
                return std::make_shared<NullValue>();
            });
        o->Set("wait", fn);
    }

    static void initDispatchMessage(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                const auto start = steady_clock::now();
                constexpr long long budgetNs = 5 * 1000 * 1000;
                while (true) {
                    auto now = steady_clock::now();
                    if ((now - start).count() > budgetNs) {
                        break;
                    }
                    Message msg{};
                    {
                        std::lock_guard lock(queueMutex);
                        if (!msgQueue.empty()) {
                            msg = msgQueue.front();
                            msgQueue.pop();
                        } else {
                            break;
                        }
                    }
                    if (onMessageCallback) {
                        std::vector callbackArgs = {msg.data};
                        Interpreter::CallFunction(onMessageCallback, callbackArgs);
                    }
                }
                return std::make_shared<NullValue>();
            });
        o->Set("dispatchMessage", fn);
    }

    static void initExit(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                isRunning = false;
                noticer.notify_all();
                return std::make_shared<NullValue>();
            });
        o->Set("exit", fn);
    }

public:
    static ValuePtr CreateThreadModule() {
        auto threadObj = std::make_shared<ObjectValue>();
        initSleep(threadObj);
        initStart(threadObj);
        initOnMessage(threadObj);
        initPostMessage(threadObj);
        initDispatchMessage(threadObj);
        initWait(threadObj);
        initExit(threadObj);
        return threadObj;
    }
};


#endif //BXSCRIPT_THREADMODULE_H
