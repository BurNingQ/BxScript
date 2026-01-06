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

#include "GlobalVars.h"

class ControlBase;

class WindowRegistry {
public:
    static void Register(void* hwnd, Controller* ctrl) {
        if (hwnd) gControllerRegistry[hwnd] = ctrl;
    }
    static void Unregister(void* hwnd) {
        if (hwnd) gControllerRegistry.erase(hwnd);
    }
    static ControlBase* Get(void* hwnd) {
        auto it = gControllerRegistry.find(hwnd);
        return (it != gControllerRegistry.end()) ? reinterpret_cast<ControlBase *>(it->second) : nullptr;
    }
};

#endif //BXSCRIPT_WINDOW_REGISTRY_H
