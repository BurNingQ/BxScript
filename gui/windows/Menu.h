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
#include <windef.h>

class MenuItem;
struct RadioGroup;

class Menu {
public:
    void *hMenu = nullptr; // w32.HMENU
    void *hwnd = nullptr; // w32.HWND

    void Dispose();

    bool IsDisposed() const;

    void Show() const;

    MenuItem *AddSubMenu(const std::wstring &text) const;
};

class MenuItem {
public:
    void *hMenu = nullptr; // w32.HMENU
    HMENU hSubMenu = nullptr; // w32.HMENU

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

    void AddSeparator() const;

    MenuItem *AddItem(const std::wstring &text, Shortcut shortcut) const;

    MenuItem *AddItemCheckable(const std::wstring &text, Shortcut shortcut) const;

    MenuItem *AddItemRadio(const std::wstring &text, Shortcut shortcut) const;

    MenuItem *AddItemWithBitmap(const std::wstring &text, Shortcut shortcut, Bitmap *image) const;

    MenuItem *AddSubMenu(const std::wstring &text) const;

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