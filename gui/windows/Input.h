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
 * @brief    Input
 */
#ifndef BXSCRIPT_INPUT_H
#define BXSCRIPT_INPUT_H

#include "ControlBase.h"

constexpr wchar_t passwordChar = L'*';
constexpr wchar_t nopasswordChar = L' ';

class Edit final : public ControlBase {
protected:
    EventManager onChange;

public:
    Edit() = default;

    ~Edit() override = default;

    static Edit *Create(Controller *parent);

    EventManager &OnChange() { return onChange; }
    // Accessors for embedded events from ControlBase
    EventManager &OnKeyUp() override { return onKeyUp; }
    EventManager &OnKeyDown() override { return onKeyDown; }

    Edit *SetReadOnly(bool isReadOnly);

    Edit *SetPassword(bool isPassword);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

class MultiEdit final : public ControlBase {
protected:
    EventManager onChange;

public:
    MultiEdit() = default;

    ~MultiEdit() override = default;

    static MultiEdit *Create(Controller *parent);

    EventManager &OnChange() { return onChange; }

    MultiEdit *SetReadOnly(bool isReadOnly);

    MultiEdit *AddLine(const std::wstring &text);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif
