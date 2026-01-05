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
 * @brief    ProgressBar
 */
#ifndef BXSCRIPT_PROGRESSBAR_H
#define BXSCRIPT_PROGRESSBAR_H

#include "ControlBase.h"

class ProgressBar : public ControlBase {
public:
    ProgressBar() = default;

    virtual ~ProgressBar() = default;

    static ProgressBar *New(Controller *parent);

    int Value();

    void SetValue(int v);

    void Range(unsigned int &min, unsigned int &max);

    void SetRange(int min, int max);

    virtual uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;
};

#endif

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"

ProgressBar *ProgressBar::New(Controller *parent) {
    ProgressBar *pb = new ProgressBar();

    // PROGRESS_CLASS is L"msctls_progress32"
    pb->InitControl(PROGRESS_CLASSW, parent, 0, WS_CHILD | WS_VISIBLE | PBS_SMOOTH);

    // WindowRegistry::Register is called inside InitControl

    pb->SetSize(200, 22);

    return pb;
}

int ProgressBar::Value() {
    return (int) User32::W32_SendMessage((HWND) m_hwnd, PBM_GETPOS, 0, 0);
}

void ProgressBar::SetValue(int v) {
    User32::W32_SendMessage((HWND) m_hwnd, PBM_SETPOS, (WPARAM) v, 0);
}

void ProgressBar::Range(unsigned int &min, unsigned int &max) {
    // PBM_GETRANGE: wParam is TRUE to return min, FALSE to return max
    min = (unsigned int) User32::W32_SendMessage((HWND) m_hwnd, PBM_GETRANGE, (WPARAM) TRUE, 0);
    max = (unsigned int) User32::W32_SendMessage((HWND) m_hwnd, PBM_GETRANGE, (WPARAM) FALSE, 0);
}

void ProgressBar::SetRange(int min, int max) {
    User32::W32_SendMessage((HWND) m_hwnd, PBM_SETRANGE32, (WPARAM) min, (LPARAM) max);
}

uintptr_t ProgressBar::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    return User32::W32_DefWindowProc((HWND) m_hwnd, msg, wparam, lparam);
}

#endif
