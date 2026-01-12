/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    ProgressBar
 */
#include "ProgressBar.h"
#include <windows.h>
#include <commctrl.h>
#include "internal/User32.h"

ProgressBar *ProgressBar::New(Controller *parent) {
    ProgressBar *pb = new ProgressBar();
    pb->InitControl(PROGRESS_CLASSW, parent, 0, WS_CHILD | WS_VISIBLE | PBS_SMOOTH);
    pb->SetSize(200, 22);
    return pb;
}

int ProgressBar::Value() const {
    return static_cast<int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), PBM_GETPOS, 0, 0));
}

void ProgressBar::SetValue(int v) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), PBM_SETPOS, static_cast<WPARAM>(v), 0);
}

void ProgressBar::Range(unsigned int &min, unsigned int &max) const {
    min = static_cast<unsigned int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), PBM_GETRANGE, (WPARAM) TRUE, 0));
    max = static_cast<unsigned int>(User32::W32_SendMessage(static_cast<HWND>(m_hwnd), PBM_GETRANGE, (WPARAM) FALSE, 0));
}

void ProgressBar::SetRange(int min, int max) const {
    User32::W32_SendMessage(static_cast<HWND>(m_hwnd), PBM_SETRANGE32, static_cast<WPARAM>(min), (LPARAM) max);
}

uintptr_t ProgressBar::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}
