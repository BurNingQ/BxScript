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
 * @brief    Menu
 */
#ifndef BXSCRIPT_MENU_H
#define BXSCRIPT_MENU_H

#include "ControlBase.h"
#include "Bitmap.h"
#include "Keyboard.h"
#include <vector>
#include <map>
#include <string>

// Forward declarations
class MenuItem;
struct RadioGroup;

class Menu {
public:
    void *hMenu = nullptr; // w32.HMENU
    void *hwnd = nullptr; // w32.HWND

    void Dispose();

    bool IsDisposed();

    void Show();

    MenuItem *AddSubMenu(const std::wstring &text);
};

class MenuItem {
public:
    void *hMenu = nullptr; // w32.HMENU
    void *hSubMenu = nullptr; // w32.HMENU

    std::wstring text;
    std::wstring toolTip;
    Bitmap *image = nullptr;
    Shortcut shortcut;
    bool enabled = false;

    bool checkable = false;
    bool checked = false;
    bool isRadio = false;

    uint16_t id = 0;

    EventManager onClick;

    EventManager &OnClick() { return onClick; }

    void AddSeparator();

    MenuItem *AddItem(const std::wstring &text, Shortcut shortcut);

    MenuItem *AddItemCheckable(const std::wstring &text, Shortcut shortcut);

    MenuItem *AddItemRadio(const std::wstring &text, Shortcut shortcut);

    MenuItem *AddItemWithBitmap(const std::wstring &text, Shortcut shortcut, Bitmap *image);

    MenuItem *AddSubMenu(const std::wstring &text);

    bool IsSeparator() const { return text == L"-"; }
    void SetSeparator() { text = L"-"; }

    bool Enabled() const { return enabled; }

    void SetEnabled(bool b);

    bool Checkable() const { return checkable; }

    void SetCheckable(bool b);

    bool Checked() const { return checked; }

    void SetChecked(bool b);

    std::wstring Text() const { return text; }

    void SetText(const std::wstring &s);

    Bitmap *Image() const { return image; }

    void SetImage(Bitmap *b);

    std::wstring ToolTip() const { return toolTip; }

    void SetToolTip(const std::wstring &s);

    void update();

    void updateRadioGroup();
};

struct RadioGroup {
    std::vector<MenuItem *> members;
    void *hwnd = nullptr; // w32.HMENU (Go source sets menuItem.hMenu here)
};

MenuItem *NewContextMenu();

// Global variables (exposed via implementation)
extern std::map<uint16_t, MenuItem *> actionsByID;
extern std::map<Shortcut, MenuItem *> shortcut2Action;

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "internal/User32.h"

static uint16_t nextMenuItemID = 3;
std::map<uint16_t, MenuItem *> actionsByID;
std::map<Shortcut, MenuItem *> shortcut2Action;
static std::map<void *, std::vector<MenuItem *> > menuItems;
static std::map<MenuItem *, RadioGroup *> radioGroups;
static bool initialised = false;

void updateRadioGroups();

static void initMenuItemInfoFromAction(MENUITEMINFOW *mii, MenuItem *a) {
    mii->cbSize = sizeof(MENUITEMINFOW);
    mii->fMask = MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING;
    if (a->image != nullptr) {
        mii->fMask |= MIIM_BITMAP;
        mii->hbmpItem = (HBITMAP) a->image->GetHandle();
    }
    if (a->IsSeparator()) {
        mii->fType = MFT_SEPARATOR;
    } else {
        mii->fType = MFT_STRING;
        std::wstring displayText;
        if (a->shortcut.Key != 0) {
            displayText = a->text + L"\t" + ShortcutToString(a->shortcut);
            shortcut2Action[a->shortcut] = a;
        } else {
            displayText = a->text;
        }
        // Note: mii->dwTypeData expects non-const pointer, but we must ensure buffer lifetime
        static std::wstring lastText;
        lastText = displayText;
        mii->dwTypeData = (LPWSTR) lastText.c_str();
        mii->cch = (UINT) displayText.length();
    }
    mii->wID = (UINT) a->id;

    if (a->Enabled()) {
        mii->fState &= ~MFS_DISABLED;
    } else {
        mii->fState |= MFS_DISABLED;
    }

    if (a->Checkable()) {
        mii->fMask |= MIIM_CHECKMARKS;
    }
    if (a->Checked()) {
        mii->fState |= MFS_CHECKED;
    }

    if (a->hSubMenu != nullptr) {
        mii->fMask |= MIIM_SUBMENU;
        mii->hSubMenu = (HMENU) a->hSubMenu;
    }
}

static MenuItem *addMenuItem(void *hMenu, void *hSubMenu, const std::wstring &text, Shortcut shortcut, Bitmap *image, bool checkable) {
    MenuItem *item = new MenuItem();
    item->hMenu = hMenu;
    item->hSubMenu = hSubMenu;
    item->text = text;
    item->shortcut = shortcut;
    item->image = image;
    item->enabled = true;
    item->id = nextMenuItemID++;
    item->checkable = checkable;
    item->isRadio = false;

    actionsByID[item->id] = item;
    menuItems[hMenu].push_back(item);

    MENUITEMINFOW mii = {0};
    initMenuItemInfoFromAction(&mii, item);

    if (!InsertMenuItemW((HMENU) hMenu, (UINT) - 1, TRUE, &mii)) {
        exit(1);
    }
    return item;
}

MenuItem *NewContextMenu() {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) exit(1);
    MenuItem *item = new MenuItem();
    item->hMenu = hMenu;
    item->hSubMenu = hMenu;
    return item;
}

void Menu::Dispose() {
    if (hMenu) {
        DestroyMenu((HMENU) hMenu);
        hMenu = nullptr;
    }
}

bool Menu::IsDisposed() {
    return hMenu == nullptr;
}

void Menu::Show() {
    initialised = true;
    updateRadioGroups();
    if (!DrawMenuBar((HWND) hwnd)) {
        exit(1);
    }
}

MenuItem *Menu::AddSubMenu(const std::wstring &text) {
    HMENU hSubMenu = CreateMenu();
    if (!hSubMenu) exit(1);
    return addMenuItem(hMenu, hSubMenu, text, Shortcut{0, 0}, nullptr, false);
}

void updateRadioGroups() {
    if (!initialised) return;

    std::vector<MenuItem *> radioItemsChecked;
    radioGroups.clear();
    std::vector<MenuItem *> currentRadioGroupMembers;

    for (auto const &[hMenu, menu]: menuItems) {
        size_t menuLength = menu.size();
        for (size_t index = 0; index < menuLength; ++index) {
            MenuItem *menuItem = menu[index];
            if (menuItem->isRadio) {
                currentRadioGroupMembers.push_back(menuItem);
                if (menuItem->checked) {
                    radioItemsChecked.push_back(menuItem);
                }

                if (index == menuLength - 1) {
                    RadioGroup *rg = new RadioGroup();
                    rg->members = currentRadioGroupMembers;
                    rg->hwnd = menuItem->hMenu;
                    for (auto *member: currentRadioGroupMembers) {
                        radioGroups[member] = rg;
                    }
                    currentRadioGroupMembers.clear();
                }
                continue;
            }

            if (!currentRadioGroupMembers.empty()) {
                RadioGroup *rg = new RadioGroup();
                rg->members = currentRadioGroupMembers;
                rg->hwnd = menuItem->hMenu;
                for (auto *member: currentRadioGroupMembers) {
                    radioGroups[member] = rg;
                }
                currentRadioGroupMembers.clear();
            }
        }
    }

    for (auto *item: radioItemsChecked) {
        RadioGroup *rg = radioGroups[item];
        uint16_t startID = rg->members[0]->id;
        uint16_t endID = rg->members.back()->id;
        CheckMenuRadioItem((HMENU) rg->hwnd, startID, endID, item->id, MF_BYCOMMAND);
    }
}

void MenuItem::AddSeparator() { addMenuItem(hSubMenu, nullptr, L"-", Shortcut{0, 0}, nullptr, false); }
MenuItem *MenuItem::AddItem(const std::wstring &text, Shortcut shortcut) { return addMenuItem(hSubMenu, nullptr, text, shortcut, nullptr, false); }
MenuItem *MenuItem::AddItemCheckable(const std::wstring &text, Shortcut shortcut) {
    return addMenuItem(hSubMenu, nullptr, text, shortcut, nullptr, true);
}
MenuItem *MenuItem::AddItemRadio(const std::wstring &text, Shortcut shortcut) {
    MenuItem *mi = addMenuItem(hSubMenu, nullptr, text, shortcut, nullptr, true);
    mi->isRadio = true;
    return mi;
}
MenuItem *MenuItem::AddItemWithBitmap(const std::wstring &text, Shortcut shortcut, Bitmap *image) {
    return addMenuItem(hSubMenu, nullptr, text, shortcut, image, false);
}
MenuItem *MenuItem::AddSubMenu(const std::wstring &text) {
    HMENU hSub = CreatePopupMenu();
    if (!hSub) exit(1);
    return addMenuItem(hSubMenu, hSub, text, Shortcut{0, 0}, nullptr, false);
}

static int indexInObserver(MenuItem *a) {
    auto &list = menuItems[a->hMenu];
    for (int i = 0; i < (int) list.size(); ++i) {
        if (list[i] == a) return i;
    }
    return -1;
}

void MenuItem::update() {
    MENUITEMINFOW mii = {0};
    initMenuItemInfoFromAction(&mii, this);
    if (!SetMenuItemInfoW((HMENU) hMenu, (UINT) indexInObserver(this), TRUE, &mii)) {
        exit(1);
    }
    if (isRadio) updateRadioGroup();
}

void MenuItem::SetEnabled(bool b) {
    enabled = b;
    update();
}
void MenuItem::SetCheckable(bool b) {
    checkable = b;
    update();
}
void MenuItem::SetChecked(bool b) {
    if (isRadio) {
        if (radioGroups.count(this)) {
            for (auto *member: radioGroups[this]->members) member->checked = false;
        }
    }
    checked = b;
    update();
}
void MenuItem::SetText(const std::wstring &s) {
    text = s;
    update();
}
void MenuItem::SetImage(Bitmap *b) {
    image = b;
    update();
}
void MenuItem::SetToolTip(const std::wstring &s) {
    toolTip = s;
    update();
}

void MenuItem::updateRadioGroup() {
    if (radioGroups.count(this)) {
        RadioGroup *rg = radioGroups[this];
        CheckMenuRadioItem((HMENU) rg->hwnd, rg->members[0]->id, rg->members.back()->id, id, MF_BYCOMMAND);
    }
}

#endif