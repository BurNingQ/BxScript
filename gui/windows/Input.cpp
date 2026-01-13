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
 * @brief    Input
 */
#include "Input.h"
#include <windows.h>
#include "internal/User32.h"
#include "GlobalVars.h"

Edit *Edit::Create(Controller *parent) {
    const auto edt = new Edit();
    edt->InitControl(L"EDIT", parent, WS_EX_CLIENTEDGE,
                     WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN);
    edt->SetFont(DefaultFont);
    edt->SetSize(200, 22);
    return edt;
}

Edit *Edit::SetReadOnly(bool isReadOnly) {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), EM_SETREADONLY, static_cast<uintptr_t>(isReadOnly ? TRUE : FALSE), 0);
    return this;
}

Edit *Edit::SetPassword(bool isPassword) {
    if (isPassword) {
        User32::W32_SendMessage(static_cast<HWND>(m_hwnd), EM_SETPASSWORDCHAR, (uintptr_t) passwordChar, 0);
    } else {
        User32::W32_SendMessage(static_cast<HWND>(m_hwnd), EM_SETPASSWORDCHAR, 0, 0);
    }
    return this;
}

uintptr_t Edit::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            if (HIWORD(static_cast<uint32_t>(wparam)) == EN_CHANGE) {
                onChange.Fire(Event(this, nullptr));
            }
            break;
        default: ;
    }
    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}

MultiEdit *MultiEdit::Create(Controller *parent) {
    const auto med = new MultiEdit();
    med->InitControl(L"EDIT", parent, WS_EX_CLIENTEDGE,
                     WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | WS_VSCROLL | WS_HSCROLL |
                     ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL);
    med->SetFont(DefaultFont);
    med->SetSize(200, 400);
    return med;
}

MultiEdit *MultiEdit::SetReadOnly(bool isReadOnly) {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), EM_SETREADONLY, static_cast<uintptr_t>(isReadOnly ? TRUE : FALSE), 0);
    return this;
}

MultiEdit *MultiEdit::AddLine(const std::wstring &text) {
    if (this->Text().length() == 0) {
        this->SetText(text);
    } else {
        this->SetText(this->Text() + L"\r\n" + text);
    }
    return this;
}

uintptr_t MultiEdit::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            if (HIWORD(static_cast<uint32_t>(wparam)) == EN_CHANGE) {
                onChange.Fire(Event(this, nullptr));
            }
            break;
        default: ;
    }
    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
