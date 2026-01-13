/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/13
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Slider
 */
#include "Slider.h"
#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"
#include "GlobalVars.h"

Slider *Slider::New(Controller *parent) {
    auto *tb = new Slider();
    tb->InitControl(TRACKBAR_CLASSW, parent, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD);
    tb->SetFont(DefaultFont);
    tb->SetText(L"Slider");
    tb->SetSize(200, 32);
    tb->SetRange(0, 100);
    tb->SetPage(10);
    return tb;
}

int Slider::Value() const {
    const LRESULT ret = User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_GETPOS, 0, 0);
    return static_cast<int>(ret);
}

void Slider::SetValue(int v) {
    m_prevPos = v;
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_SETPOS, (WPARAM) TRUE, (LPARAM) v);
}

void Slider::Range(int &min, int &max) const {
    min = static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_GETRANGEMIN, 0, 0));
    max = static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_GETRANGEMAX, 0, 0));
}

void Slider::SetRange(int min, int max) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG(min, max));
}

void Slider::SetPage(int pagesize) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), TBM_SETPAGESIZE, 0, (LPARAM) pagesize);
}

uintptr_t Slider::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    int newPos = this->Value();
    if (newPos != m_prevPos) {
        onScroll.Fire(Event(this, nullptr));
        m_prevPos = newPos;
    }

    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}