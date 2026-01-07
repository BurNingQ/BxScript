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
 * @brief    Event
 */
#ifndef BXSCRIPT_EVENT_H
#define BXSCRIPT_EVENT_H

#include <any>
#include <utility>

class Event {
public:

    ControlBase *Sender;

    std::any Data;

    explicit Event(ControlBase *sender, std::any data = nullptr)
        : Sender(sender), Data(std::move(data)) {
    }

    static Event FromInt(ControlBase* sender, unsigned int val) {
        return Event(sender, std::make_any<unsigned int>(val));
    }

    /**
     * 辅助函数：快速获取强类型数据
     */
    template<typename T>
    T GetData() const {
        return std::any_cast<T>(Data);
    }
};

#endif //BXSCRIPT_EVENT_H
