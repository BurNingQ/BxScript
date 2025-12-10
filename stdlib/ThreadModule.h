//
// Created by BurNingLi on 2025/12/10.
//

#ifndef BXSCRIPT_THREADMODULE_H
#define BXSCRIPT_THREADMODULE_H
#include "evaluator/Value.h"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std::chrono;

// 消息模型
struct Message {
    ValuePtr data;
};

inline std::queue<Message> msgQueue{};
inline std::mutex queueMutex{};
inline std::condition_variable g_cv{};
// 回调
inline ValuePtr onMessageCallback = nullptr;

class ThreadModule {
private:
    static void initSleep(std::shared_ptr<ObjectValue> &o) {
        o->Set("sleep", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       if (!args.empty() && args[0]->type == ValueType::NUMBER) {
                           const long ms = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->
                               Value);
                           std::this_thread::sleep_for(milliseconds(ms));
                       }
                       return std::make_shared<NullValue>();
                   }
               ));
    }

    static void initInvoke(std::shared_ptr<ObjectValue> &o) {
        o->Set("invoke", std::make_shared<NativeFunctionValue>(
                   [](const std::vector<ValuePtr> &args) -> ValuePtr {
                       if (!args.empty() && args[0]->type == ValueType::FUNCTION) {
                           auto fn = std::static_pointer_cast<FunctionValue>(args[0]);
                           std::thread thread([fn]() {

                           });
                       }
                   }));
    }

public:
    static ValuePtr CreateThreadModule() {
        auto threadObj = std::make_shared<ObjectValue>();
        initSleep(threadObj);
        initInvoke(threadObj);
        return threadObj;
    }
};


#endif //BXSCRIPT_THREADMODULE_H
