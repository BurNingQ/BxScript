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

    int Value();

    void SetValue(int v);

    void Range(int &min, int &max);

    void SetRange(int min, int max);

    void SetPage(int pagesize);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"
#include "GlobalVars.h"

Slider *Slider::New(Controller *parent) {
    Slider *tb = new Slider();

    // TRACKBAR_CLASSW is L"msctls_trackbar32"
    tb->InitControl(TRACKBAR_CLASSW, parent, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD);

    tb->SetFont(DefaultFont);
    tb->SetText(L"Slider");
    tb->SetSize(200, 32);

    tb->SetRange(0, 100);
    tb->SetPage(10);

    return tb;
}

int Slider::Value() {
    LRESULT ret = User32::W32_SendMessage((HWND) m_hwnd, TBM_GETPOS, 0, 0);
    return (int) ret;
}

void Slider::SetValue(int v) {
    m_prevPos = v;
    User32::W32_SendMessage((HWND) m_hwnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) v);
}

void Slider::Range(int &min, int &max) {
    min = (int) User32::W32_SendMessage((HWND) m_hwnd, TBM_GETRANGEMIN, 0, 0);
    max = (int) User32::W32_SendMessage((HWND) m_hwnd, TBM_GETRANGEMAX, 0, 0);
}

void Slider::SetRange(int min, int max) {
    // MAKELONG is (WORD(a) | (DWORD(WORD(b)) << 16))
    User32::W32_SendMessage((HWND) m_hwnd, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(min, max));
}

void Slider::SetPage(int pagesize) {
    User32::W32_SendMessage((HWND) m_hwnd, TBM_SETPAGESIZE, 0, (LPARAM) pagesize);
}

uintptr_t Slider::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    int newPos = this->Value();
    if (newPos != m_prevPos) {
        onScroll.Fire(Event(this, nullptr));
        m_prevPos = newPos;
    }

    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
