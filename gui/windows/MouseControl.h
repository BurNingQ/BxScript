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
 * @brief    MouseControl
 */
#ifndef BXSCRIPT_MOUSE_CONTROL_H
#define BXSCRIPT_MOUSE_CONTROL_H

#include "ControlBase.h"
#include "Utils.h"

class MouseControl : public ControlBase {
protected:
    bool isMouseLeft = false;

public:
    MouseControl() = default;

    virtual ~MouseControl() = default;

    void Init(Controller *parent, const std::wstring &className, unsigned int exStyle, unsigned int style);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "internal/ComCtl32.h"
#include "GlobalVars.h"

void MouseControl::Init(Controller *parent, const std::wstring &className, unsigned int exStyle, unsigned int style) {
    RegClassOnlyOnce(className);

    m_hwnd = User32::W32_CreateWindowEx(exStyle, className.c_str(), L"", style,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        parent ? (HWND) parent->Handle() : nullptr,
                                        nullptr, (HINSTANCE) gAppInstance, nullptr);
    m_parent = parent;
    WindowRegistry::Register(m_hwnd, this);

    isMouseLeft = true;
    this->SetFont(DefaultFont);
}

uintptr_t MouseControl::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    ControlBase *sender = WindowRegistry::Get(m_hwnd);

    switch (msg) {
        case WM_CREATE:
            internalTrackMouseEvent(m_hwnd);
            onCreate.Fire(Event(sender, nullptr));
            break;

        case WM_CLOSE:
            onClose.Fire(Event(sender, nullptr));
            break;

        case WM_MOUSEMOVE:
            onMouseHover.Fire(Event(sender, nullptr));
            isMouseLeft = false;
            break;

        case WM_MOUSELEAVE:
            onMouseLeave.Fire(Event(sender, nullptr));
            isMouseLeft = true;
            break;
    }

    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
