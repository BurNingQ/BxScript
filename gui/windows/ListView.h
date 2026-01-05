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
 * @brief    ListView
 */
#ifndef BXSCRIPT_LISTVIEW_H
#define BXSCRIPT_LISTVIEW_H

#include "ControlBase.h"
#include <vector>
#include <map>

class ListItem {
public:
    virtual ~ListItem() = default;

    virtual std::vector<std::wstring> Text() = 0;

    virtual int ImageIndex() = 0;
};

class ListItemChecker {
public:
    virtual ~ListItemChecker() = default;

    virtual bool Checked() = 0;

    virtual void SetChecked(bool checked) = 0;
};

class ListItemSetter {
public:
    virtual ~ListItemSetter() = default;

    virtual void SetText(const std::wstring &s) = 0;
};

// --- Helper class ---

class StringListItem : public ListItem, public ListItemChecker {
public:
    int ID;
    std::wstring Data;
    bool Check;

    StringListItem(int id, const std::wstring &data, bool check)
        : ID(id), Data(data), Check(check) {
    }

    std::vector<std::wstring> Text() override { return {Data}; }
    bool Checked() override { return Check; }
    void SetChecked(bool checked) override { Check = checked; }
    int ImageIndex() override { return 0; }
};

// --- ListView Control ---

class ListView : public ControlBase {
protected:
    int lastIndex = 0;
    int cols = 0;

    std::map<ListItem *, uintptr_t> item2Handle;
    std::map<uintptr_t, ListItem *> handle2Item;

    EventManager onEndLabelEdit;
    EventManager onDoubleClick;
    EventManager onClick;
    EventManager onKeyDown;
    EventManager onItemChanging;
    EventManager onItemChanged;
    EventManager onCheckChanged;
    EventManager onViewChange;
    EventManager onEndScroll;

public:
    ListView() = default;

    virtual ~ListView() = default;

    static ListView *NewListBox(Controller *parent);

    static ListView *NewListView(Controller *parent);

    void EnableSingleSelect(bool enable);

    void EnableSortHeader(bool enable);

    void EnableSortAscending(bool enable);

    void EnableEditLabels(bool enable);

    void EnableFullRowSelect(bool enable);

    void EnableDoubleBuffer(bool enable);

    void EnableHotTrack(bool enable);

    bool SetItemCount(int count);

    int ItemCount();

    ListItem *ItemAt(int x, int y);

    std::vector<ListItem *> Items();

    void AddColumn(const std::wstring &caption, int width);

    void StretchLastColumn();

    bool CheckBoxes();

    void SetCheckBoxes(bool value);

    void AddItem(ListItem *item);

    void InsertItem(ListItem *item, int index);

    bool UpdateItem(ListItem *item);

    bool DeleteAllItems();

    void DeleteItem(ListItem *item);

    void EnsureVisible(ListItem *item);

    ListItem *SelectedItem();

    bool SetSelectedItem(ListItem *item);

    std::vector<ListItem *> SelectedItems();

    unsigned int SelectedCount();

    int SelectedIndex();

    void SetSelectedIndex(int i);

    // Event publishers
    EventManager &OnEndLabelEdit() { return onEndLabelEdit; }
    EventManager &OnDoubleClick() { return onDoubleClick; }
    EventManager &OnClick() { return onClick; }
    EventManager &OnKeyDown() { return onKeyDown; }
    EventManager &OnItemChanging() { return onItemChanging; }
    EventManager &OnItemChanged() { return onItemChanged; }
    EventManager &OnCheckChanged() { return onCheckChanged; }
    EventManager &OnViewChange() { return onViewChange; }
    EventManager &OnEndScroll() { return onEndScroll; }

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

private:
    void setItemState(int i, unsigned int state, unsigned int mask);

    void insertLvColumn(void *lvColumn, int iCol);

    void insertLvItem(void *lvItem);

    void setLvItem(void *lvItem);

    int findIndexByItem(ListItem *item);

    ListItem *findItemByIndex(int i);
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"
#include "GlobalVars.h"
#include "EventData.h"

namespace {
    void SetStyle(HWND hwnd, bool b, DWORD styleBit) {
        DWORD style = GetWindowLongW(hwnd, GWL_STYLE);
        if (b) style |= styleBit;
        else style &= ~styleBit;
        SetWindowLongW(hwnd, GWL_STYLE, style);
    }
}

ListView *ListView::NewListBox(Controller *parent) {
    ListView *lv = new ListView();
    unsigned int style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER;
    lv->InitControl(L"SysListView32", parent, 0, style);
    lv->SetFont(DefaultFont);
    lv->SetSize(200, 400);
    lv->SetTheme(L"Explorer");
    return lv;
}

ListView *ListView::NewListView(Controller *parent) {
    ListView *lv = new ListView();
    unsigned int style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
    lv->InitControl(L"SysListView32", parent, 0, style);
    lv->SetFont(DefaultFont);
    lv->SetSize(200, 400);
    lv->SetTheme(L"Explorer");
    return lv;
}

void ListView::setItemState(int i, unsigned int state, unsigned int mask) {
    LVITEMW item = {0};
    item.state = state;
    item.stateMask = mask;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETITEMSTATE, (WPARAM) i, (LPARAM) & item);
}

void ListView::EnableSingleSelect(bool enable) { SetStyle((HWND) m_hwnd, enable, LVS_SINGLESEL); }
void ListView::EnableSortHeader(bool enable) { SetStyle((HWND) m_hwnd, enable, LVS_NOSORTHEADER); }
void ListView::EnableSortAscending(bool enable) { SetStyle((HWND) m_hwnd, enable, LVS_SORTASCENDING); }
void ListView::EnableEditLabels(bool enable) { SetStyle((HWND) m_hwnd, enable, LVS_EDITLABELS); }

void ListView::EnableFullRowSelect(bool enable) {
    LPARAM mask = LVS_EX_FULLROWSELECT;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

void ListView::EnableDoubleBuffer(bool enable) {
    LPARAM mask = LVS_EX_DOUBLEBUFFER;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

void ListView::EnableHotTrack(bool enable) {
    LPARAM mask = LVS_EX_TRACKSELECT;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, enable ? 0 : mask, enable ? mask : 0);
}

bool ListView::SetItemCount(int count) {
    return User32::W32_SendMessage((HWND) m_hwnd, LVM_SETITEMCOUNT, (WPARAM) count, 0) != 0;
}

int ListView::ItemCount() {
    return (int) User32::W32_SendMessage((HWND) m_hwnd, LVM_GETITEMCOUNT, 0, 0);
}

ListItem *ListView::ItemAt(int x, int y) {
    LVHITTESTINFO hti = {0};
    hti.pt.x = x;
    hti.pt.y = y;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_HITTEST, 0, (LPARAM) & hti);
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
    lc.pszText = (LPWSTR) caption.c_str();
    insertLvColumn(&lc, cols);
    cols++;
}

void ListView::StretchLastColumn() {
    if (cols == 0) return;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETCOLUMNWIDTH, (WPARAM)(cols - 1), LVSCW_AUTOSIZE_USEHEADER);
}

bool ListView::CheckBoxes() {
    return (User32::W32_SendMessage((HWND) m_hwnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0) & LVS_EX_CHECKBOXES) > 0;
}

void ListView::SetCheckBoxes(bool value) {
    LRESULT exStyle = User32::W32_SendMessage((HWND) m_hwnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
    LRESULT oldStyle = exStyle;
    if (value) exStyle |= LVS_EX_CHECKBOXES;
    else exStyle &= ~LVS_EX_CHECKBOXES;
    if (exStyle != oldStyle) User32::W32_SendMessage((HWND) m_hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, exStyle);

    LRESULT mask = User32::W32_SendMessage((HWND) m_hwnd, LVM_GETCALLBACKMASK, 0, 0);
    if (value) mask |= LVIS_STATEIMAGEMASK;
    else mask &= ~LVIS_STATEIMAGEMASK;
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETCALLBACKMASK, (WPARAM) mask, 0);
}

void ListView::AddItem(ListItem *item) { InsertItem(item, ItemCount()); }

void ListView::InsertItem(ListItem *item, int index) {
    auto text = item->Text();
    LVITEMW li = {0};
    li.mask = LVIF_TEXT | LVIF_PARAM;
    li.pszText = (LPWSTR) text[0].c_str();
    li.iItem = index;

    lastIndex++;
    li.lParam = (LPARAM) lastIndex;
    handle2Item[li.lParam] = item;
    item2Handle[item] = li.lParam;

    insertLvItem(&li);

    for (size_t i = 1; i < text.size(); ++i) {
        li.mask = LVIF_TEXT;
        li.pszText = (LPWSTR) text[i].c_str();
        li.iSubItem = (int) i;
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
    li.pszText = (LPWSTR) text[0].c_str();
    li.lParam = lparam;
    li.iItem = index;

    setLvItem(&li);

    for (size_t i = 1; i < text.size(); ++i) {
        li.mask = LVIF_TEXT;
        li.pszText = (LPWSTR) text[i].c_str();
        li.iSubItem = (int) i;
        setLvItem(&li);
    }
    return true;
}

void ListView::insertLvColumn(void *lvColumn, int iCol) {
    User32::W32_SendMessage((HWND) m_hwnd, LVM_INSERTCOLUMN, (WPARAM) iCol, (LPARAM) lvColumn);
}

void ListView::insertLvItem(void *lvItem) {
    User32::W32_SendMessage((HWND) m_hwnd, LVM_INSERTITEM, 0, (LPARAM) lvItem);
}

void ListView::setLvItem(void *lvItem) {
    User32::W32_SendMessage((HWND) m_hwnd, LVM_SETITEM, 0, (LPARAM) lvItem);
}

bool ListView::DeleteAllItems() {
    if (User32::W32_SendMessage((HWND) m_hwnd, LVM_DELETEALLITEMS, 0, 0) == TRUE) {
        item2Handle.clear();
        handle2Item.clear();
        return true;
    }
    return false;
}

void ListView::DeleteItem(ListItem *item) {
    int index = findIndexByItem(item);
    if (index == -1) return;
    if (User32::W32_SendMessage((HWND) m_hwnd, LVM_DELETEITEM, (WPARAM) index, 0) != 0) {
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
    return (int) User32::W32_SendMessage((HWND) m_hwnd, LVM_FINDITEM, (WPARAM) - 1, (LPARAM) & it);
}

ListItem *ListView::findItemByIndex(int i) {
    LVITEMW it = {0};
    it.mask = LVIF_PARAM;
    it.iItem = i;
    if (User32::W32_SendMessage((HWND) m_hwnd, LVM_GETITEM, 0, (LPARAM) & it) == TRUE) {
        if (handle2Item.find(it.lParam) != handle2Item.end()) return handle2Item[it.lParam];
    }
    return nullptr;
}

void ListView::EnsureVisible(ListItem *item) {
    int i = findIndexByItem(item);
    if (i != -1) User32::W32_SendMessage((HWND) m_hwnd, LVM_ENSUREVISIBLE, (WPARAM) i, FALSE);
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
        i = (int) User32::W32_SendMessage((HWND) m_hwnd, LVM_GETNEXTITEM, (WPARAM) i, LVNI_SELECTED);
        if (i == -1) break;
        ListItem *item = findItemByIndex(i);
        if (item) items.push_back(item);
    }
    return items;
}

unsigned int ListView::SelectedCount() {
    return (unsigned int) User32::W32_SendMessage((HWND) m_hwnd, LVM_GETSELECTEDCOUNT, 0, 0);
}

int ListView::SelectedIndex() {
    return (int) User32::W32_SendMessage((HWND) m_hwnd, LVM_GETNEXTITEM, (WPARAM) - 1, LVNI_SELECTED);
}

void ListView::SetSelectedIndex(int i) { setItemState(i, LVIS_SELECTED, LVIS_SELECTED); }

uintptr_t ListView::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    if (msg == WM_NOTIFY) {
        NMHDR *nm = (NMHDR *) lparam;
        switch (nm->code) {
            case LVN_ENDLABELEDITW: {
                NMLVDISPINFO *nmdi = (NMLVDISPINFO *) lparam;
                if (nmdi->item.pszText != nullptr) {
                    if (handle2Item.count(nmdi->item.lParam)) {
                        ListItem *item = handle2Item[nmdi->item.lParam];
                        LabelEditEventData data;
                        data.Item = item; // This needs to be compatible with EventData.h types
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
                NMITEMACTIVATE *ac = (NMITEMACTIVATE *) lparam;
                LVHITTESTINFO hti = {0};
                hti.pt = ac->ptAction;
                User32::W32_SendMessage((HWND) m_hwnd, LVM_HITTEST, 0, (LPARAM) & hti);
                if (hti.flags == LVHT_ONITEMSTATEICON) {
                    ListItem *item = findItemByIndex(hti.iItem);
                    ListItemChecker *checker = dynamic_cast<ListItemChecker *>(item);
                    if (checker) {
                        bool checked = !checker->Checked();
                        checker->SetChecked(checked);
                        onCheckChanged.Fire(Event(this, item));
                        User32::W32_SendMessage((HWND) m_hwnd, LVM_UPDATE, (WPARAM) hti.iItem, 0);
                    }
                }
                hti.pt = ac->ptAction;
                User32::W32_SendMessage((HWND) m_hwnd, LVM_SUBITEMHITTEST, 0, (LPARAM) & hti);
                onClick.Fire(Event(this, (int) hti.iSubItem));
                break;
            }
            case LVN_KEYDOWN: {
                NMLVKEYDOWN *nmkey = (NMLVKEYDOWN *) lparam;
                if (nmkey->wVKey == VK_SPACE && CheckBoxes()) {
                    ListItem *item = SelectedItem();
                    ListItemChecker *checker = dynamic_cast<ListItemChecker *>(item);
                    if (checker) {
                        bool checked = !checker->Checked();
                        checker->SetChecked(checked);
                        onCheckChanged.Fire(Event(this, item));
                        User32::W32_SendMessage((HWND) m_hwnd, LVM_UPDATE, (WPARAM) findIndexByItem(item), 0);
                    }
                }
                onKeyDown.Fire(Event(this, (int) nmkey->wVKey));
                User32::W32_SendMessage((HWND) m_parent->Handle(), WM_KEYDOWN, nmkey->wVKey, 0);
                break;
            }
            case LVN_ITEMCHANGING: {
                NMLISTVIEW *nmlv = (NMLISTVIEW *) lparam;
                onItemChanging.Fire(Event(this, findItemByIndex(nmlv->iItem)));
                break;
            }
            case LVN_ITEMCHANGED: {
                NMLISTVIEW *nmlv = (NMLISTVIEW *) lparam;
                onItemChanged.Fire(Event(this, findItemByIndex(nmlv->iItem)));
                break;
            }
            case LVN_GETDISPINFO: {
                NMLVDISPINFO *nmdi = (NMLVDISPINFO *) lparam;
                if (nmdi->item.mask & LVIF_STATE && nmdi->item.stateMask & LVIS_STATEIMAGEMASK) {
                    if (handle2Item.count(nmdi->item.lParam)) {
                        ListItem *item = handle2Item[nmdi->item.lParam];
                        ListItemChecker *checker = dynamic_cast<ListItemChecker *>(item);
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
        }
    }
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif