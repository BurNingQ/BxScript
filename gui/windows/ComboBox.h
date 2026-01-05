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
 * @brief    ComboBox
 */
#ifndef BXSCRIPT_COMBOBOX_H
#define BXSCRIPT_COMBOBOX_H

#include "ControlBase.h"

class ComboBox : public ControlBase {
    int m_itemCount = 0;

public:
    // 事件回调：当用户更改了下拉框的选择时触发
    EventCallback OnSelectedChange;

    ComboBox() = default;

    virtual ~ComboBox() = default;

    /**
     * 工厂方法：创建一个下拉列表样式的组合框。
     */
    static ComboBox *Create(ControlBase *parent, int x = 0, int y = 0, int w = 200, int h = 150);

    // ======================== 项管理 ========================

    /**
     * 清空所有项。
     */
    bool DeleteAllItems();

    /**
     * 在指定索引处插入项。
     */
    bool InsertItem(int index, const std::wstring &str);

    /**
     * 在末尾追加项。
     */
    bool AddItem(const std::wstring &str);

    /**
     * 删除指定索引的项。
     */
    bool DeleteItem(int index);

    // ======================== 选择管理 ========================

    /**
     * 获取当前选中项的索引。
     * @return 成功返回索引(0起)，未选中返回 -1。
     */
    int SelectedItem();

    /**
     * 设置当前选中项。
     */
    bool SetSelectedItem(int index);

    /**
     * 获取项的总数。
     */
    int GetItemCount() const { return m_itemCount; }

    // ======================== 消息处理 ========================

    virtual bool ProcessMessage(unsigned int msg, uintptr_t wParam, uintptr_t lParam, uintptr_t &result);
};

#endif // BXSCRIPT_COMBOBOX_H


// ============================================================================
// Implementation
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"

#define HWND_CAST(ptr) static_cast<HWND>(ptr)

ComboBox *ComboBox::Create(ControlBase *parent, int x, int y, int w, int h) {
    ComboBox *cb = new ComboBox();

    // CBS_DROPDOWNLIST: 只读下拉列表
    // WS_VSCROLL: 允许垂直滚动
    unsigned int style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST;

    cb->InitControl(L"COMBOBOX", parent, 0, style);

    // 设置初始大小和位置
    cb->SetPos(x, y);
    cb->SetSize(w, h); // 注意：Win32 ComboBox 的高度包含了下拉列表展开后的高度

    cb->SetTheme(L"Explorer");
    return cb;
}

bool ComboBox::DeleteAllItems() {
    m_itemCount = 0;
    // CB_RESETCONTENT: 移除所有项
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

int ComboBox::SelectedItem() {
    // CB_GETCURSEL: 获取当前选择索引
    return (int) User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_GETCURSEL, 0, 0);
}

bool ComboBox::SetSelectedItem(int index) {
    // CB_SETCURSEL: 设置当前选择
    LRESULT res = User32::W32_SendMessage(HWND_CAST(m_hwnd), CB_SETCURSEL, (WPARAM) index, 0);
    return (int) res == index;
}

bool ComboBox::ProcessMessage(unsigned int msg, uintptr_t wParam, uintptr_t lParam, uintptr_t &result) {
    if (msg == WM_COMMAND) {
        // HIWORD(wParam) 是通知码
        unsigned short code = HIWORD(static_cast<DWORD>(wParam));
        if (code == CBN_SELCHANGE) {
            if (OnSelectedChange) {
                OnSelectedChange();
            }
            return true;
        }
    }
    return false;
}

#endif // BXSCRIPT_IMPLEMENTATION
