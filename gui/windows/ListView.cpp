/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @brief    ListView (Clean Double-Buffered Version)
 */
#include "ListView.h"
#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"
#include "GlobalVars.h"
#include "EventData.h"
#include "Utils.h"

// 宏定义补全
#ifndef LVM_INSERTCOLUMNW
#define LVM_INSERTCOLUMNW (LVM_FIRST + 97)
#endif
#ifndef LVM_INSERTITEMW
#define LVM_INSERTITEMW (LVM_FIRST + 77)
#endif
#ifndef LVM_SETITEMW
#define LVM_SETITEMW (LVM_FIRST + 76)
#endif
#ifndef LVM_SETBKCOLOR
#define LVM_SETBKCOLOR (LVM_FIRST + 1)
#endif
#ifndef LVM_SETTEXTBKCOLOR
#define LVM_SETTEXTBKCOLOR (LVM_FIRST + 38)
#endif

// ============================================================================
// 工厂方法
// ============================================================================

ListView *ListView::NewListView(Controller *parent) {
    auto *lv = new ListView();
    unsigned int style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | WS_CLIPSIBLINGS;
    lv->InitControl(L"SysListView32", parent, 0, style);
    lv->EnableDoubleBuffer(true);
    DWORD sysColor = GetSysColor(COLOR_WINDOW);
    User32::W32_SendMessage(static_cast<HWND>(lv->Handle()), LVM_SETBKCOLOR, 0, sysColor);
    User32::W32_SendMessage(static_cast<HWND>(lv->Handle()), LVM_SETTEXTBKCOLOR, 0, sysColor);
    lv->EnableFullRowSelect(true);
    lv->SetFont(DefaultFont);
    lv->SetSize(200, 400);
    lv->SetTheme(L"Explorer");
    return lv;
}

ListView *ListView::NewListBox(Controller *parent) {
    auto *lv = NewListView(parent);
    SetStyle(lv->Handle(), true, LVS_NOCOLUMNHEADER);
    return lv;
}

// ============================================================================
// 辅助方法
// ============================================================================

void ListView::setItemState(int i, unsigned int state, unsigned int mask) const {
    LVITEMW item = {0};
    item.state = state;
    item.stateMask = mask;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETITEMSTATE, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(&item));
}

bool ListView::DeleteAllColumns() {
    while (User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_DELETECOLUMN, 0, 0)) {
        cols--;
    }
    if (cols < 0) cols = 0;
    return true;
}

void ListView::EnableSingleSelect(bool enable) const { SetStyle(m_hwnd, enable, LVS_SINGLESEL); }
void ListView::EnableSortHeader(bool enable) const { SetStyle(m_hwnd, enable, LVS_NOSORTHEADER); }
void ListView::EnableSortAscending(bool enable) const { SetStyle(m_hwnd, enable, LVS_SORTASCENDING); }
void ListView::EnableEditLabels(bool enable) const { SetStyle(m_hwnd, enable, LVS_EDITLABELS); }

void ListView::EnableFullRowSelect(bool enable) const {
    LPARAM mask = LVS_EX_FULLROWSELECT;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

void ListView::EnableDoubleBuffer(bool enable) const {
    LPARAM mask = LVS_EX_DOUBLEBUFFER;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

void ListView::EnableHotTrack(bool enable) const {
    LPARAM mask = LVS_EX_TRACKSELECT;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

bool ListView::SetItemCount(int count) const {
    return User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETITEMCOUNT, (WPARAM) count, 0) != 0;
}

int ListView::ItemCount() const {
    return (int) User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETITEMCOUNT, 0, 0);
}

ListItem *ListView::ItemAt(int x, int y) {
    LVHITTESTINFO hti = {0};
    hti.pt.x = x;
    hti.pt.y = y;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_HITTEST, 0, (LPARAM) &hti);
    return findItemByIndex(hti.iItem);
}

std::vector<ListItem *> ListView::Items() {
    std::vector<ListItem *> list;
    for (auto const &[item, handle]: item2Handle) {
        list.push_back(item);
    }
    return list;
}

void ListView::AddColumn(const std::wstring &caption, int width) {
    LVCOLUMNW lc = {0};
    lc.mask = LVCF_TEXT;
    if (width != 0) {
        lc.mask |= LVCF_WIDTH;
        lc.cx = width;
    }
    lc.pszText = const_cast<LPWSTR>(caption.c_str());
    insertLvColumn(&lc, cols);
    cols++;
}

void ListView::StretchLastColumn() const {
    if (cols == 0) return;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETCOLUMNWIDTH, static_cast<WPARAM>(cols - 1), LVSCW_AUTOSIZE_USEHEADER);
}

bool ListView::CheckBoxes() const {
    return (User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0) & LVS_EX_CHECKBOXES) > 0;
}

void ListView::SetCheckBoxes(bool value) const {
    LRESULT exStyle = User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
    LRESULT oldStyle = exStyle;
    if (value) exStyle |= LVS_EX_CHECKBOXES;
    else exStyle &= ~LVS_EX_CHECKBOXES;
    if (exStyle != oldStyle) User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, exStyle);
    LRESULT mask = User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETCALLBACKMASK, 0, 0);
    if (value) mask |= LVIS_STATEIMAGEMASK;
    else mask &= ~LVIS_STATEIMAGEMASK;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETCALLBACKMASK, static_cast<WPARAM>(mask), 0);
}

void ListView::AddItem(ListItem *item) { InsertItem(item, ItemCount()); }

void ListView::InsertItem(ListItem *item, int index) {
    auto text = item->Text();
    LVITEMW li = {0};
    li.mask = LVIF_TEXT | LVIF_PARAM;
    if (text.empty()) {
        li.pszText = const_cast<LPWSTR>(L"");
    } else {
        li.pszText = const_cast<LPWSTR>(text[0].c_str());
    }
    li.iItem = index;

    lastIndex++;
    li.lParam = static_cast<LPARAM>(lastIndex);
    handle2Item[li.lParam] = item;
    item2Handle[item] = li.lParam;

    insertLvItem(&li);

    for (size_t i = 1; i < text.size(); ++i) {
        li.mask = LVIF_TEXT;
        li.pszText = const_cast<LPWSTR>(text[i].c_str());
        li.iSubItem = static_cast<int>(i);
        setLvItem(&li);
    }
}

bool ListView::UpdateItem(ListItem *item) {
    if (item2Handle.find(item) == item2Handle.end()) return false;
    uintptr_t lparam = item2Handle[item];
    int index = findIndexByItem(item);
    if (index == -1) return false;
    auto text = item->Text();
    LVITEMW li = {0};
    li.mask = LVIF_TEXT | LVIF_PARAM;
    if (text.empty()) li.pszText = const_cast<LPWSTR>(L"");
    else li.pszText = const_cast<LPWSTR>(text[0].c_str());
    li.lParam = lparam;
    li.iItem = index;
    setLvItem(&li);
    for (size_t i = 1; i < text.size(); ++i) {
        li.mask = LVIF_TEXT;
        li.pszText = const_cast<LPWSTR>(text[i].c_str());
        li.iSubItem = static_cast<int>(i);
        setLvItem(&li);
    }
    return true;
}

void ListView::insertLvColumn(void *lvColumn, int iCol) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_INSERTCOLUMNW, static_cast<WPARAM>(iCol), reinterpret_cast<LPARAM>(lvColumn));
}

void ListView::insertLvItem(void *lvItem) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(lvItem));
}

void ListView::setLvItem(void *lvItem) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_SETITEMW, 0, reinterpret_cast<LPARAM>(lvItem));
}

bool ListView::DeleteAllItems() {
    if (User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_DELETEALLITEMS, 0, 0) == TRUE) {
        item2Handle.clear();
        handle2Item.clear();
        return true;
    }
    return false;
}

void ListView::DeleteItem(ListItem *item) {
    int index = findIndexByItem(item);
    if (index == -1) return;
    if (User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_DELETEITEM, static_cast<WPARAM>(index), 0) != 0) {
        uintptr_t h = item2Handle[item];
        item2Handle.erase(item);
        handle2Item.erase(h);
    }
}

int ListView::findIndexByItem(ListItem *item) {
    if (item2Handle.find(item) == item2Handle.end()) return -1;
    LVFINDINFOW it = {0};
    it.flags = LVFI_PARAM;
    it.lParam = item2Handle[item];
    return static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_FINDITEM, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(&it)));
}

ListItem *ListView::findItemByIndex(int i) {
    LVITEMW it = {0};
    it.mask = LVIF_PARAM;
    it.iItem = i;
    if (User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETITEM, 0, reinterpret_cast<LPARAM>(&it)) == TRUE) {
        if (handle2Item.find(it.lParam) != handle2Item.end()) return handle2Item[it.lParam];
    }
    return nullptr;
}

void ListView::EnsureVisible(ListItem *item) {
    int i = findIndexByItem(item);
    if (i != -1) User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_ENSUREVISIBLE, static_cast<WPARAM>(i), FALSE);
}

ListItem *ListView::SelectedItem() {
    auto items = SelectedItems();
    return items.empty() ? nullptr : items[0];
}

bool ListView::SetSelectedItem(ListItem *item) {
    int i = findIndexByItem(item);
    if (i > -1) {
        SetSelectedIndex(i);
        return true;
    }
    return false;
}

std::vector<ListItem *> ListView::SelectedItems() {
    std::vector<ListItem *> items;
    int i = -1;
    while (true) {
        i = static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETNEXTITEM, static_cast<WPARAM>(i), LVNI_SELECTED));
        if (i == -1) break;
        ListItem *item = findItemByIndex(i);
        if (item) items.push_back(item);
    }
    return items;
}

unsigned int ListView::SelectedCount() const {
    return static_cast<unsigned int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETSELECTEDCOUNT, 0, 0));
}

int ListView::SelectedIndex() const {
    return static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_GETNEXTITEM, (WPARAM) -1, LVNI_SELECTED));
}

void ListView::SetSelectedIndex(int i) const { setItemState(i, LVIS_SELECTED, LVIS_SELECTED); }

// ============================================================================
// 消息处理
// ============================================================================

uintptr_t ListView::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    if (msg == WM_NOTIFY) {
        auto *nm = reinterpret_cast<NMHDR *>(lparam);
        switch (nm->code) {
            case LVN_ENDLABELEDITW: {
                auto *nmdi = reinterpret_cast<NMLVDISPINFOW *>(lparam);
                if (nmdi->item.pszText != nullptr) {
                    if (handle2Item.count(nmdi->item.lParam)) {
                        ListItem *item = handle2Item[nmdi->item.lParam];
                        LabelEditEventData data;
                        data.Item = item;
                        data.Text = nmdi->item.pszText;
                        onEndLabelEdit.Fire(Event(this, data));
                    }
                    return TRUE;
                }
                break;
            }
            case NM_DBLCLK:
                onDoubleClick.Fire(Event(this, nullptr));
                break;
            case NM_CLICK: {
                auto ac = reinterpret_cast<NMITEMACTIVATE *>(lparam);
                LVHITTESTINFO hti = {0};
                hti.pt = ac->ptAction;
                User32::W32_SendMessage((HWND) m_hwnd, LVM_HITTEST, 0, (LPARAM) &hti);
                if (hti.flags == LVHT_ONITEMSTATEICON) {
                    ListItem *item = findItemByIndex(hti.iItem);
                    if (item) {
                        auto checker = dynamic_cast<ListItemChecker *>(item);
                        if (checker) {
                            bool checked = !checker->Checked();
                            checker->SetChecked(checked);
                            onCheckChanged.Fire(Event(this, item));
                            User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_UPDATE, static_cast<WPARAM>(hti.iItem), 0);
                        }
                    }
                }
                onClick.Fire(Event(this, (int) hti.iSubItem));
                break;
            }
            case LVN_KEYDOWN: {
                auto nmkey = reinterpret_cast<NMLVKEYDOWN *>(lparam);
                if (nmkey->wVKey == VK_SPACE && CheckBoxes()) {
                    ListItem *item = SelectedItem();
                    if (item) {
                        auto checker = dynamic_cast<ListItemChecker *>(item);
                        if (checker) {
                            const bool checked = !checker->Checked();
                            checker->SetChecked(checked);
                            onCheckChanged.Fire(Event(this, item));
                            User32::W32_SendMessage(static_cast<HWND>(m_hwnd), LVM_UPDATE, static_cast<WPARAM>(findIndexByItem(item)), 0);
                        }
                    }
                }
                onKeyDown.Fire(Event(this, (int) nmkey->wVKey));
                if (parentHwnd) {
                    User32::W32_SendMessage(static_cast<HWND>(parentHwnd->Handle()), WM_KEYDOWN, nmkey->wVKey, 0);
                }
                break;
            }
            case LVN_ITEMCHANGING: {
                auto nmlv = reinterpret_cast<NMLISTVIEW *>(lparam);
                onItemChanging.Fire(Event(this, findItemByIndex(nmlv->iItem)));
                break;
            }
            case LVN_ITEMCHANGED: {
                auto nmlv = reinterpret_cast<NMLISTVIEW *>(lparam);
                onItemChanged.Fire(Event(this, findItemByIndex(nmlv->iItem)));
                break;
            }
            case LVN_GETDISPINFO: {
                NMLVDISPINFO *nmdi = reinterpret_cast<NMLVDISPINFOA *>(lparam);
                if (nmdi->item.mask & LVIF_STATE && nmdi->item.stateMask & LVIS_STATEIMAGEMASK) {
                    if (handle2Item.count(nmdi->item.lParam)) {
                        ListItem *item = handle2Item[nmdi->item.lParam];
                        auto checker = dynamic_cast<ListItemChecker *>(item);
                        if (checker) {
                            nmdi->item.state = checker->Checked() ? 0x2000 : 0x1000;
                        }
                    }
                }
                onViewChange.Fire(Event(this, nullptr));
                break;
            }
            case LVN_ENDSCROLL:
                onEndScroll.Fire(Event(this, nullptr));
                break;
            default: ;
        }
    }
    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}