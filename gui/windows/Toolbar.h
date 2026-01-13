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
    explicit ToolButton(Toolbar *parent) : tb(parent) {
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

    ~Toolbar() override;

    static Toolbar *NewHToolbar(Controller *parent);

    static Toolbar *NewToolbar(Controller *parent);

    void AddSeparator();

    ToolButton *AddButton(const std::wstring &text, int image, bool hasImage);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

private:
    static Toolbar *newToolbar(Controller *parent, unsigned int style);

    static void initButton(ToolButton *btn, unsigned char *state, unsigned char *style, int *image, uintptr_t *textPtr);

    void update(ToolButton *btn) const;

    friend class ToolButton;
};

#endif
