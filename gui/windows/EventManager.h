/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/5
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    EventManager
 */
#ifndef BXSCRIPT_EVENT_MANAGER_H
#define BXSCRIPT_EVENT_MANAGER_H

#include <functional>
#include "Event.h"

// 定义回调函数原型
using EventHandler = std::function<void(const Event&)>;

class EventManager {
private:
    EventHandler m_handler = nullptr;

public:
    // 绑定回调
    void Bind(EventHandler handler) {
        m_handler = handler;
    }

    // 触发事件
    void Fire(const Event& arg) const {
        if (m_handler) {
            m_handler(arg);
        }
    }

    // 检查是否有绑定
    bool IsBound() const {
        return m_handler != nullptr;
    }

    // 快速清除
    void Clear() {
        m_handler = nullptr;
    }
};

#endif //BXSCRIPT_EVENT_MANAGER_H