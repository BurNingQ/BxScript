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
#include <utility>
#include <vector>
#include <map>
#include "ListInterfaces.h"

class StringListItem : public ListItem, public ListItemChecker {
public:
    int ID;
    std::wstring Data;
    bool Check;

    StringListItem(const int id, std::wstring data, const bool check)
        : ID(id), Data(std::move(data)), Check(check) {
    }

    std::vector<std::wstring> Text() override { return {Data}; }
    bool Checked() override { return Check; }
    void SetChecked(bool checked) override { Check = checked; }
    int ImageIndex() override { return 0; }
};

// --- ListView Control ---

class ListView final : public ControlBase {
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

    void EnableSingleSelect(bool enable) const;

    void EnableSortHeader(bool enable) const;

    void EnableSortAscending(bool enable) const;

    void EnableEditLabels(bool enable) const;

    void EnableFullRowSelect(bool enable) const;

    void EnableDoubleBuffer(bool enable) const;

    void EnableHotTrack(bool enable) const;

    bool SetItemCount(int count) const;

    int ItemCount() const;

    ListItem *ItemAt(int x, int y);

    std::vector<ListItem *> Items();

    void AddColumn(const std::wstring &caption, int width);

    void StretchLastColumn() const;

    bool CheckBoxes() const;

    void SetCheckBoxes(bool value) const;

    void AddItem(ListItem *item);

    void InsertItem(ListItem *item, int index);

    bool UpdateItem(ListItem *item);

    bool DeleteAllItems();

    void DeleteItem(ListItem *item);

    void EnsureVisible(ListItem *item);

    ListItem *SelectedItem();

    bool SetSelectedItem(ListItem *item);

    std::vector<ListItem *> SelectedItems();

    unsigned int SelectedCount() const;

    int SelectedIndex() const;

    void SetSelectedIndex(int i) const;

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
    void setItemState(int i, unsigned int state, unsigned int mask) const;

    void insertLvColumn(void *lvColumn, int iCol) const;

    void insertLvItem(void *lvItem) const;

    void setLvItem(void *lvItem) const;

    int findIndexByItem(ListItem *item);

    ListItem *findItemByIndex(int i);
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION



#endif