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
 * @brief    Button
 */
#include <windows.h>
#include "internal/User32.h"
#include "Icon.h"
#include "Button.h"
#include "GlobalVars.h"

// 宏定义辅助
#define HWND_CAST(ptr) static_cast<HWND>(ptr)

// ----------------------------------------------------------------------------
// Button Implementation
// ----------------------------------------------------------------------------

uintptr_t Button::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            OnClick.Fire(Event(this, nullptr));
            break;
        case WM_LBUTTONDOWN:
            User32::W32_SetCapture(HWND_CAST(m_hwnd));
            break;
        case WM_LBUTTONUP:
            User32::W32_ReleaseCapture();
            break;
        default: ;
    }
    return User32::W32_DefWindowProc(HWND_CAST(m_hwnd), msg, wparam, lparam);
}

bool Button::Checked() const {
    const LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), BM_GETCHECK, 0, 0);
    return res == BST_CHECKED;
}

void Button::SetChecked(bool checked) const {
    const WPARAM wparam = checked ? BST_CHECKED : BST_UNCHECKED;
    User32::W32_SendMessage(HWND_CAST(m_hwnd), BM_SETCHECK, wparam, 0);
}

void Button::SetIcon(Icon *icon) const {
    if (icon) {
        User32::W32_SendMessage(static_cast<HWND>(m_hwnd), BM_SETIMAGE, IMAGE_ICON, (LPARAM) icon->Handle());
    }
}

// ----------------------------------------------------------------------------
// PushButton
// ----------------------------------------------------------------------------

PushButton *PushButton::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    auto *pb = new PushButton();
    // BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD
    DWORD style = BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD;
    pb->InitControl(L"BUTTON", parent, 0, style);

    pb->SetFont(DefaultFont);
    pb->SetText(text);
    pb->SetPos(x, y);
    pb->SetSize(w, h);

    pb->SetTheme(L"Explorer");

    return pb;
}

void PushButton::SetDefault() const {
    const auto hwnd = HWND_CAST(m_hwnd);
    LONG_PTR style = User32::W32_GetWindowLongPtr(hwnd, GWL_STYLE);

    style &= ~BS_PUSHBUTTON;
    style |= BS_DEFPUSHBUTTON;

    User32::W32_SetWindowLongPtr(hwnd, GWL_STYLE, style);
    User32::W32_InvalidateRect(hwnd, nullptr, TRUE);
}

// ----------------------------------------------------------------------------
// CheckBox
// ----------------------------------------------------------------------------

CheckBox *CheckBox::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    auto *cb = new CheckBox();
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX;
    cb->SetFont(DefaultFont);
    cb->InitControl(L"BUTTON", parent, 0, style);
    cb->SetText(text);
    cb->SetPos(x, y);
    cb->SetSize(w, h);
    cb->SetTheme(L"Explorer");
    return cb;
}

// ----------------------------------------------------------------------------
// RadioButton
// ----------------------------------------------------------------------------

RadioButton *RadioButton::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    auto *rb = new RadioButton();
    constexpr DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
    rb->SetFont(DefaultFont);
    rb->InitControl(L"BUTTON", parent, 0, style);
    rb->SetText(text);
    rb->SetPos(x, y);
    rb->SetSize(w, h);
    rb->SetTheme(L"Explorer");
    return rb;
}

// ----------------------------------------------------------------------------
// GroupBox
// ----------------------------------------------------------------------------

GroupBox *GroupBox::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    auto *gb = new GroupBox();
    constexpr DWORD style = WS_VISIBLE | WS_CHILD | WS_GROUP | BS_GROUPBOX;
    gb->SetFont(DefaultFont);
    gb->InitControl(L"BUTTON", parent, 0, style);
    gb->SetText(text);
    gb->SetPos(x, y);
    gb->SetSize(w, h);
    gb->SetTheme(L"Explorer");
    return gb;
}

// ----------------------------------------------------------------------------
// IconButton
// ----------------------------------------------------------------------------

IconButton *IconButton::Create(ControlBase *parent, Icon *icon, int x, int y, int w, int h) {
    const auto ib = new IconButton();
    // BS_ICON
    DWORD style = BS_ICON | WS_TABSTOP | WS_VISIBLE | WS_CHILD;
    ib->InitControl(L"BUTTON", parent, 0, style);
    ib->SetFont(DefaultFont);
    if (icon) {
        ib->SetIcon(icon);
    }
    ib->SetPos(x, y);
    ib->SetSize(w, h);
    ib->SetTheme(L"Explorer");
    return ib;
}