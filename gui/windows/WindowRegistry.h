/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/4
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    WindowRegistry
 */
#ifndef BXSCRIPT_WINDOW_REGISTRY_H
#define BXSCRIPT_WINDOW_REGISTRY_H

#include <windows.h>
#include <unordered_map>

class ControlBase;

class WindowRegistry {

    inline static std::unordered_map<void*, ControlBase *> s_registry;

public:
    static void Register(void* hwnd, ControlBase *ctrl) {
        if (hwnd) {
            s_registry[hwnd] = ctrl;
        }
    }

    static void Unregister(void* hwnd) {
        if (hwnd) {
            s_registry.erase(hwnd);
        }
    }

    static ControlBase *Get(void* hwnd) {
        auto it = s_registry.find(hwnd);
        if (it != s_registry.end()) {
            return it->second;
        }
        return nullptr;
    }
};

#endif //BXSCRIPT_WINDOW_REGISTRY_H
