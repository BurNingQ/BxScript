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
 * @brief    Slider
 */
#ifndef BXSCRIPT_SLIDER_H
#define BXSCRIPT_SLIDER_H

#include "ControlBase.h"

class Slider : public ControlBase {
protected:
    int m_prevPos = 0;
    EventManager onScroll;

public:
    Slider() = default;

    virtual ~Slider() = default;

    static Slider *New(Controller *parent);

    EventManager &OnScroll() { return onScroll; }

    int Value() const;

    void SetValue(int v);

    void Range(int &min, int &max) const;

    void SetRange(int min, int max) const;

    void SetPage(int pagesize) const;

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif
