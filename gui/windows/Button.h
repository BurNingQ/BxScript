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
 * @brief    Button
 */
#ifndef BXSCRIPT_BUTTON_H
#define BXSCRIPT_BUTTON_H

#include "ControlBase.h"
#include "Icon.h"

// 前置声明 Icon 类

// ============================================================================
// 1. Button 基类
// ============================================================================
class Button : public ControlBase {
public:
    // 事件回调
    EventCallback OnClick{};

    Button() = default;

    virtual ~Button() = default;

    // 核心功能
    bool Checked();

    void SetChecked(bool checked);

    // 设置图标 (需要 Icon 类)
    void SetIcon(Icon *icon);

    // 处理消息 (供消息循环调用)
    // 返回 true 表示消息已处理
    virtual bool ProcessMessage(unsigned int msg, uintptr_t wParam, uintptr_t lParam, uintptr_t &result) override;
};

// ============================================================================
// 2. PushButton (普通按钮)
// ============================================================================
class PushButton final : public Button {
public:
    // 工厂方法：创建并初始化
    static PushButton *Create(ControlBase *parent, const std::wstring &text = L"Button", int x = 0, int y = 0, int w = 100, int h = 25);

    // 设为默认按钮 (回车键触发)
    void SetDefault();
};

// ============================================================================
// 3. CheckBox (复选框)
// ============================================================================
class CheckBox final : public Button {
public:
    static CheckBox *Create(ControlBase *parent, const std::wstring &text = L"CheckBox", int x = 0, int y = 0, int w = 100, int h = 22);
};

// ============================================================================
// 4. RadioButton (单选框)
// ============================================================================
class RadioButton final : public Button {
public:
    static RadioButton *Create(ControlBase *parent, const std::wstring &text = L"RadioButton", int x = 0, int y = 0, int w = 100, int h = 22);
};

// ============================================================================
// 5. GroupBox (分组框)
// ============================================================================
class GroupBox final : public Button {
public:
    static GroupBox *Create(ControlBase *parent, const std::wstring &text = L"GroupBox", int x = 0, int y = 0, int w = 200, int h = 100);
};

// ============================================================================
// 6. IconButton (纯图标按钮)
// ============================================================================
class IconButton final : public Button {
public:
    static IconButton *Create(ControlBase *parent, Icon *icon, int x = 0, int y = 0, int w = 32, int h = 32);
};

#endif // BXSCRIPT_BUTTON_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"
#include "internal/Gdi32.h" // 也许需要用到字体

// #include "Icon.h"

#ifndef ICON_CLASS_DEFINED
class Icon {
public:
    HICON GetHandle() const { return nullptr; }
};
#endif

// 宏定义辅助
#define HWND_CAST(ptr) static_cast<HWND>(ptr)

// ----------------------------------------------------------------------------
// Button Implementation
// ----------------------------------------------------------------------------

bool Button::ProcessMessage(unsigned int msg, uintptr_t wParam, uintptr_t lParam, uintptr_t &result) {
    switch (msg) {
        case WM_LBUTTONDOWN:
            User32::W32_SetCapture(HWND_CAST(m_hwnd));
            break;

        case WM_LBUTTONUP:
            User32::W32_ReleaseCapture();
            // 在这里触发 Click 并不是最标准的做法，标准做法是处理 WM_COMMAND(BN_CLICKED)
            // 但为了兼容 Go 代码的逻辑，可以在这里或者 WM_COMMAND 里触发
            break;

        case WM_COMMAND:
            // HIWORD(wParam) 是通知码
            if (HIWORD(wParam) == BN_CLICKED) {
                if (OnClick) {
                    OnClick(); // 触发回调
                }
                return true;
            }
            break;
    }

    // 调用默认处理
    // 注意：ControlBase::PreTranslateMessage 返回 false，
    // 真正的 DefWindowProc 是在全局 WndProc 里调用的，这里只负责拦截自定义逻辑
    return false;
}

bool Button::Checked() {
    LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), BM_GETCHECK, 0, 0);
    return res == BST_CHECKED;
}

void Button::SetChecked(bool checked) {
    WPARAM wparam = checked ? BST_CHECKED : BST_UNCHECKED;
    User32::W32_SendMessage(HWND_CAST(m_hwnd), BM_SETCHECK, wparam, 0);
}

void Button::SetIcon(Icon *icon) {
    if (icon) {
        User32::W32_SendMessage((HWND) m_hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM) icon->Handle());
    }
}

// ----------------------------------------------------------------------------
// PushButton
// ----------------------------------------------------------------------------

PushButton *PushButton::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    PushButton *pb = new PushButton();
    // BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD
    DWORD style = BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD;
    pb->InitControl(L"BUTTON", parent, 0, style);

    pb->SetFont(Font::DefaultFont);
    pb->SetText(text);
    pb->SetPos(x, y);
    pb->SetSize(w, h);

    // 应用现代化主题
    pb->SetTheme(L"Explorer");

    return pb;
}

void PushButton::SetDefault() {
    HWND hwnd = HWND_CAST(m_hwnd);
    LONG_PTR style = User32::W32_GetWindowLongPtr(hwnd, GWL_STYLE);

    // 清除 BS_PUSHBUTTON, 设置 BS_DEFPUSHBUTTON
    style &= ~BS_PUSHBUTTON;
    style |= BS_DEFPUSHBUTTON;

    User32::W32_SetWindowLongPtr(hwnd, GWL_STYLE, style);
    User32::W32_InvalidateRect(hwnd, nullptr, TRUE);
}

// ----------------------------------------------------------------------------
// CheckBox
// ----------------------------------------------------------------------------

CheckBox *CheckBox::Create(ControlBase *parent, const std::wstring &text, int x, int y, int w, int h) {
    CheckBox *cb = new CheckBox();
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX;
    cb->SetFont(Font::DefaultFont);
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
    RadioButton *rb = new RadioButton();
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
    rb->SetFont(Font::DefaultFont);
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
    GroupBox *gb = new GroupBox();
    DWORD style = WS_VISIBLE | WS_CHILD | WS_GROUP | BS_GROUPBOX;
    gb->SetFont(Font::DefaultFont);
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
    IconButton *ib = new IconButton();
    // BS_ICON
    DWORD style = BS_ICON | WS_TABSTOP | WS_VISIBLE | WS_CHILD;
    ib->InitControl(L"BUTTON", parent, 0, style);
    ib->SetFont(Font::DefaultFont);
    if (icon) {
        ib->SetIcon(icon);
    }
    ib->SetPos(x, y);
    ib->SetSize(w, h);
    ib->SetTheme(L"Explorer");
    return ib;
}

#endif // BXSCRIPT_IMPLEMENTATION
