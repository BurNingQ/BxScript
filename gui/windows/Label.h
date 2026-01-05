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
 * @brief    Label
 */
#ifndef BXSCRIPT_LABEL_H
#define BXSCRIPT_LABEL_H

#include "ControlBase.h"

class Label : public ControlBase {
protected:
    EventManager onClick;

public:
    Label() = default;

    virtual ~Label() = default;

    static Label *Create(Controller *parent);

    EventManager &OnClick() { return onClick; }

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"
#include "GlobalVars.h"

Label *Label::Create(Controller *parent) {
    Label *lb = new Label();

    // WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP
    lb->InitControl(L"STATIC", parent, 0, WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP);

    // RegMsgHandler(lb) is handled inside InitControl via WindowRegistry::Register

    lb->SetFont(DefaultFont);
    lb->SetText(L"Label");
    lb->SetSize(100, 25);

    return lb;
}

uintptr_t Label::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            onClick.Fire(Event(this, nullptr));
            break;

        case WM_LBUTTONDOWN:
            User32::W32_SetCapture((HWND) m_hwnd);
            break;

        case WM_LBUTTONUP:
            User32::W32_ReleaseCapture();
            break;
    }

    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif