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
 * @brief    MouseControl
 */
#ifndef BXSCRIPT_MOUSE_CONTROL_H
#define BXSCRIPT_MOUSE_CONTROL_H

#include "ControlBase.h"
#include "Utils.h"

class MouseControl : public ControlBase {
protected:
    bool isMouseLeft = false;

public:
    MouseControl() = default;

    virtual ~MouseControl() = default;

    void Init(Controller *parent, const std::wstring &className, unsigned int exStyle, unsigned int style);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif
