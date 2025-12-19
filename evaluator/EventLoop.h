/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/17
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    事件循环，所有异步回调均发送任务到主事件队列,交给主线程适合的适合执行
 */
#ifndef BXSCRIPT_EVENTLOOP_H
#define BXSCRIPT_EVENTLOOP_H

#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include "Value.h"
#include "Interpreter.h"
#include "Logger.h"
#include <thread>

using namespace std::chrono;

struct Task {
    ValuePtr callback;
    std::vector<ValuePtr> args;
};

class EventLoop {
    inline static std::deque<Task> taskQueue;
    inline static std::mutex queueMutex;
    inline static std::atomic<int> activeTasks{0};

public:
    static void AddActiveTask() { ++activeTasks; }

    static void RemoveActiveTask() { --activeTasks; }

    static bool ShouldKeepAlive() {
        return activeTasks > 0 || HasPending();
    }

    static void Reset() {
        std::lock_guard lock(queueMutex);
        std::deque<Task> empty;
        std::swap(taskQueue, empty);
        activeTasks = 0;
    }

    static void RunLoop() {
        while (ShouldKeepAlive()) {
            bool hasWork = Dispatch(0);
            if (!hasWork && ShouldKeepAlive()) {
                std::this_thread::sleep_for(milliseconds(10));
            }
        }
    }

    static void Enqueue(ValuePtr callback, std::vector<ValuePtr> args) {
        std::lock_guard lock(queueMutex);
        taskQueue.push_back({std::move(callback), std::move(args)});
    }

    static bool Dispatch(int maxDurationMs = 0) {
        auto start = steady_clock::now();
        std::vector<Task> localQueue{};
        {
            std::lock_guard lock(queueMutex);
            if (taskQueue.empty()) return false;
            while (!taskQueue.empty()) {
                localQueue.push_back(std::move(taskQueue.front()));
                taskQueue.pop_front();
            }
        }
        for (size_t i = 0; i < localQueue.size(); ++i) {
            try {
                Interpreter::CallFunction(localQueue[i].callback, localQueue[i].args);
            } catch (const std::exception &e) {
                Logger::Error(std::string("事件循环错误: ") + e.what());
            }
            if (maxDurationMs > 0) {
                auto now = steady_clock::now();
                auto elapsed = std::chrono::duration_cast<milliseconds>(now - start).count();
                if (elapsed >= maxDurationMs) {
                    std::lock_guard lock(queueMutex);
                    for (size_t x = localQueue.size(); x > i + 1; --x) {
                        taskQueue.push_front(std::move(localQueue[x - 1]));
                    }
                    return true;
                }
            }
        }
        return false;
    }

    static bool HasPending() {
        std::lock_guard lock(queueMutex);
        return !taskQueue.empty();
    }
};


#endif //BXSCRIPT_EVENTLOOP_H
