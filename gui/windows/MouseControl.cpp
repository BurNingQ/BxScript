/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    MouseControl
 */

#include "MouseControl.h"
#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "GlobalVars.h"

void MouseControl::Init(Controller *parent, const std::wstring &className, unsigned int exStyle, unsigned int style) {
    RegClassOnlyOnce(className);

    m_hwnd = User32::W32_CreateWindowEx(exStyle, className.c_str(), L"", style,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        parent ? static_cast<HWND>(parent->Handle()) : nullptr,
                                        nullptr, static_cast<HINSTANCE>(G_AppInstance), nullptr);
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

    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
