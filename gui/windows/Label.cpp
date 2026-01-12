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
 * @brief    Label
 */
#include "Label.h"
#include <windows.h>
#include "internal/User32.h"
#include "GlobalVars.h"

inline Label *Label::Create(Controller *parent) {
    const auto lb = new Label();
    // WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP
    lb->InitControl(L"STATIC", parent, 0, WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP);
    lb->SetFont(DefaultFont);
    lb->SetText(L"Label");
    lb->SetSize(100, 25);
    return lb;
}

inline uintptr_t Label::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            onClick.Fire(Event(this, nullptr));
            break;

        case WM_LBUTTONDOWN:
            User32::W32_SetCapture(static_cast<HWND>(m_hwnd));
            break;

        case WM_LBUTTONUP:
            User32::W32_ReleaseCapture();
            break;
        default: ;
    }

    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
