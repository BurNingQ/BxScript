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
 * @brief    ComboBox
 */

#include <windows.h>
#include "internal/User32.h"
#include "ComboBox.h"

#define HWND_CAST(ptr) static_cast<HWND>(ptr)

ComboBox *ComboBox::Create(ControlBase *parent, int x, int y, int w, int h) {
    auto *cb = new ComboBox();
    constexpr unsigned int style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST;
    cb->InitControl(L"COMBOBOX", parent, 0, style);
    cb->SetPos(x, y);
    cb->SetSize(w, h);
    cb->SetTheme(L"Explorer");
    return cb;
}

bool ComboBox::DeleteAllItems() {
    m_itemCount = 0;
    return User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_RESETCONTENT, 0, 0) == TRUE;
}

bool ComboBox::InsertItem(int index, const std::wstring &str) {
    LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_INSERTSTRING, (WPARAM) index, (LPARAM) str.c_str());
    if (res != CB_ERR) {
        m_itemCount++;
        return true;
    }
    return false;
}

bool ComboBox::AddItem(const std::wstring &str) {
    // 传 -1 表示追加到末尾
    return InsertItem(-1, str);
}

bool ComboBox::DeleteItem(int index) {
    LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_DELETESTRING, (WPARAM) index, 0);
    if (res != CB_ERR) {
        m_itemCount--;
        return true;
    }
    return false;
}

int ComboBox::SelectedItem() const {
    return static_cast<int>(User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_GETCURSEL, 0, 0));
}

bool ComboBox::SetSelectedItem(int index) const {
    LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_SETCURSEL, (WPARAM) index, 0);
    return static_cast<int>(res) == index;
}

uintptr_t ComboBox::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            // HIWORD(wparam) 是通知码
            if (HIWORD(static_cast<uint32_t>(wparam)) == CBN_SELCHANGE) {
                // 正确的触发方式：
                OnSelectedChange.Fire(Event(this, nullptr));
            }
            break;
        default: ;
    }
    return User32::W32_DefWindowProc(HWND_CAST(m_hwnd), msg, wparam, lparam);
}