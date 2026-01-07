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

using EventHandler = std::function<void(const Event&)>;

class EventManager {

    EventHandler m_handler = nullptr;

public:
    void Bind(EventHandler handler) {
        m_handler = handler;
    }

    void Fire(const Event& arg) const {
        if (m_handler) {
            m_handler(arg);
        }
    }

    bool IsBound() const {
        return m_handler != nullptr;
    }

    void Clear() {
        m_handler = nullptr;
    }
};

#endif //BXSCRIPT_EVENT_MANAGER_H