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
 * @brief    Toolbar
 */
#ifndef BXSCRIPT_TOOLBAR_H
#define BXSCRIPT_TOOLBAR_H

#include "ControlBase.h"
#include <vector>
#include <string>

class Toolbar;

class ToolButton {
private:
    Toolbar *tb = nullptr;
    std::wstring text;
    bool enabled = true;
    bool checkable = false;
    bool checked = false;
    int image = 0;

    EventManager onClick;

public:
    ToolButton(Toolbar *parent) : tb(parent) {
    }

    EventManager &OnClick() { return onClick; }

    void update();

    bool IsSeparator() const { return text == L"-"; }
    void SetSeparator() { text = L"-"; }

    bool Enabled() const { return enabled; }

    void SetEnabled(bool b) {
        enabled = b;
        update();
    }

    bool Checkable() const { return checkable; }

    void SetCheckable(bool b) {
        checkable = b;
        update();
    }

    bool Checked() const { return checked; }

    void SetChecked(bool b) {
        checked = b;
        update();
    }

    std::wstring Text() const { return text; }

    void SetText(const std::wstring &s) {
        text = s;
        update();
    }

    int Image() const { return image; }

    void SetImage(int i) {
        image = i;
        update();
    }

    // Internal helpers for implementation
    friend class Toolbar;
};

class Toolbar : public ControlBase {
protected:
    std::vector<ToolButton *> buttons;

public:
    Toolbar() = default;

    virtual ~Toolbar();

    static Toolbar *NewHToolbar(Controller *parent);

    static Toolbar *NewToolbar(Controller *parent);

    void AddSeparator();

    ToolButton *AddButton(const std::wstring &text, int image, bool hasImage);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

private:
    static Toolbar *newToolbar(Controller *parent, unsigned int style);

    void initButton(ToolButton *btn, unsigned char *state, unsigned char *style, int *image, uintptr_t *textPtr);

    void update(ToolButton *btn);

    friend class ToolButton;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"
#include "GlobalVars.h"

Toolbar::~Toolbar() {
    for (auto btn: buttons) {
        delete btn;
    }
}

Toolbar *Toolbar::NewHToolbar(Controller *parent) {
    return newToolbar(parent, CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE |
                              WS_CHILD | TBSTYLE_LIST);
}

Toolbar *Toolbar::NewToolbar(Controller *parent) {
    return newToolbar(parent, CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE |
                              WS_CHILD);
}

Toolbar *Toolbar::newToolbar(Controller *parent, unsigned int style) {
    Toolbar *tb = new Toolbar();

    // ToolbarWindow32
    tb->InitControl(TOOLBARCLASSNAMEW, parent, 0, style);

    LRESULT exStyle = User32::W32_SendMessage((HWND) tb->m_hwnd, TB_GETEXTENDEDSTYLE, 0, 0);
    exStyle |= TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS;
    User32::W32_SendMessage((HWND) tb->m_hwnd, TB_SETEXTENDEDSTYLE, 0, (LPARAM) exStyle);

    // Register registry entry handled in InitControl
    tb->SetFont(DefaultFont);
    tb->SetPos(0, 0);
    tb->SetSize(200, 40);
    tb->Show();

    return tb;
}

void Toolbar::initButton(ToolButton *btn, unsigned char *state, unsigned char *style, int *image, uintptr_t *textPtr) {
    *style |= BTNS_AUTOSIZE;

    if (btn->checked) {
        *state |= TBSTATE_CHECKED;
    }

    if (btn->enabled) {
        *state |= TBSTATE_ENABLED;
    }

    if (btn->checkable) {
        *style |= BTNS_CHECK;
    }

    if (!btn->text.empty()) {
        *style |= BTNS_SHOWTEXT;
    }

    if (btn->IsSeparator()) {
        *style = BTNS_SEP;
    }

    *image = btn->image;
    // Note: We cast the pointer to uintptr_t to match the internal structure requirements
    *textPtr = reinterpret_cast<uintptr_t>(btn->text.c_str());
}

void Toolbar::update(ToolButton *btn) {
    TBBUTTONINFOW tbbi = {0};
    tbbi.dwMask = TBIF_IMAGE | TBIF_STATE | TBIF_STYLE | TBIF_TEXT;
    tbbi.cbSize = sizeof(tbbi);

    int index = -1;
    for (int i = 0; i < (int) buttons.size(); ++i) {
        if (buttons[i] == btn) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        uintptr_t textPtr = 0;
        initButton(btn, &tbbi.fsState, &tbbi.fsStyle, &tbbi.iImage, &textPtr);
        tbbi.pszText = (LPWSTR) textPtr;

        if (User32::W32_SendMessage((HWND) m_hwnd, TB_SETBUTTONINFOW, (WPARAM) index, (LPARAM) & tbbi) == 0) {
            exit(1); // panic
        }
    }
}

void ToolButton::update() {
    tb->update(this);
}

void Toolbar::AddSeparator() {
    this->AddButton(L"-", 0, false);
}

ToolButton *Toolbar::AddButton(const std::wstring &text, int image, bool hasImage) {
    ToolButton *bt = new ToolButton(this);
    bt->text = text;
    bt->image = image;
    bt->enabled = true;

    buttons.push_back(bt);
    int index = (int) buttons.size() - 1;

    TBBUTTON tbb = {0};
    tbb.idCommand = index;

    uintptr_t textPtr = 0;
    initButton(bt, &tbb.fsState, &tbb.fsStyle, &tbb.iBitmap, &textPtr);
    tbb.iString = (INT_PTR) bt->text.c_str();

    User32::W32_SendMessage((HWND) m_hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

    if (!hasImage) {
        User32::W32_SendMessage((HWND) m_hwnd, TB_SETBITMAPSIZE, 0, 0);
        User32::W32_SendMessage((HWND) m_hwnd, TB_SETBUTTONSIZE, 0, 0);
    }

    if (User32::W32_SendMessage((HWND) m_hwnd, TB_INSERTBUTTONW, (WPARAM) index, (LPARAM) & tbb) == FALSE) {
        exit(1); // panic
    }

    User32::W32_SendMessage((HWND) m_hwnd, TB_AUTOSIZE, 0, 0);
    return bt;
}

uintptr_t Toolbar::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    switch (msg) {
        case WM_COMMAND:
            if (HIWORD((uint32_t) wparam) == BN_CLICKED) {
                uint16_t id = LOWORD((uint32_t) wparam);
                if (id < buttons.size()) {
                    buttons[id]->onClick.Fire(Event(this, nullptr));
                }
            }
            break;
    }
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
