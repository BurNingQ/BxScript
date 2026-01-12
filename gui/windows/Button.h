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

// ============================================================================
// Button 基类
// ============================================================================
class Button : public ControlBase {
public:
    // 事件回调
    EventManager OnClick{};

    Button() = default;

    ~Button() override = default;

    bool Checked() const;

    void SetChecked(bool checked) const;

    void SetIcon(Icon *icon) const;

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

// ============================================================================
// PushButton (普通按钮)
// ============================================================================
class PushButton final : public Button {
public:
    static PushButton *Create(ControlBase *parent, const std::wstring &text = L"Button", int x = 0, int y = 0, int w = 100, int h = 25);

    void SetDefault() const;
};

// ============================================================================
// CheckBox (复选框)
// ============================================================================
class CheckBox final : public Button {
public:
    static CheckBox *Create(ControlBase *parent, const std::wstring &text = L"CheckBox", int x = 0, int y = 0, int w = 100, int h = 22);
};

// ============================================================================
// RadioButton (单选框)
// ============================================================================
class RadioButton final : public Button {
public:
    static RadioButton *Create(ControlBase *parent, const std::wstring &text = L"RadioButton", int x = 0, int y = 0, int w = 100, int h = 22);
};

// ============================================================================
// GroupBox (分组框)
// ============================================================================
class GroupBox final : public Button {
public:
    static GroupBox *Create(ControlBase *parent, const std::wstring &text = L"GroupBox", int x = 0, int y = 0, int w = 200, int h = 100);
};

// ============================================================================
// IconButton (纯图标按钮)
// ============================================================================
class IconButton final : public Button {
public:
    static IconButton *Create(ControlBase *parent, Icon *icon, int x = 0, int y = 0, int w = 32, int h = 32);
};

#endif // BXSCRIPT_BUTTON_H
