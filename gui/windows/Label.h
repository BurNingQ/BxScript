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
 * @brief    Label
 */
#ifndef BXSCRIPT_LABEL_H
#define BXSCRIPT_LABEL_H

#include "ControlBase.h"

class Label : public ControlBase {
protected:
    EventManager onClick;

public:
    Label() = default;

    virtual ~Label() = default;

    static Label *Create(Controller *parent);

    EventManager &OnClick() { return onClick; }

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif