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
 * @brief    ControlBase
 */
#include "ControlBase.h"

#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "internal/UxTheme.h"
#include "internal/ShCore.h"
#include "Utils.h"
#include "Rect.h"

static int _min(int a, int b) { return a < b ? a : b; }
static int _max(int a, int b) { return a > b ? a : b; }

ControlBase::~ControlBase() {
    if (m_hwnd) {
        WindowRegistry::Unregister(m_hwnd);
    }
}

void ControlBase::InitControl(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style) {
    m_parent = parent;
    const HWND hParent = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
    m_hwnd = User32::W32_CreateWindowEx(exstyle, className.c_str(), L"", style,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        hParent, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!m_hwnd) exit(1);
    WindowRegistry::Register(m_hwnd, this);
}

void ControlBase::InitWindow(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style) {
    RegClassOnlyOnce(className);
    InitControl(className, parent, exstyle, style);
}

void ControlBase::SetTheme(const std::wstring& appName) const {
    UxTheme::W32_SetWindowTheme(static_cast<HWND>(m_hwnd), appName.c_str(), nullptr);
}

std::wstring ControlBase::Text() {
    const auto hwnd = static_cast<HWND>(m_hwnd);
    const int len = User32::W32_GetWindowTextLength(hwnd);
    std::wstring buf(len + 1, L'\0');
    User32::W32_GetWindowText(hwnd, &buf[0], len + 1);
    buf.resize(len);
    return buf;
}

ControlBase *ControlBase::SetText(const std::wstring &caption) {
    User32::W32_SetWindowText(static_cast<HWND>(m_hwnd), caption.c_str());
    return this;
}

bool ControlBase::Enabled() { return User32::W32_IsWindowEnabled(static_cast<HWND>(m_hwnd)); }

ControlBase *ControlBase::SetEnabled(bool b) {
    User32::W32_EnableWindow(static_cast<HWND>(m_hwnd), b);
    return this;
}

void ControlBase::SetFocus() { User32::W32_SetFocus(static_cast<HWND>(m_hwnd)); }
void ControlBase::Invalidate(const bool erase) { User32::W32_InvalidateRect(static_cast<HWND>(m_hwnd), nullptr, erase); }

void ControlBase::Pos(int &x, int &y) {
    RECT rc;
    User32::W32_GetWindowRect(static_cast<HWND>(m_hwnd), &rc);
    x = rc.left;
    y = rc.top;
    if (!m_isForm && m_parent) {
        POINT pt = {(LONG) rc.left, (LONG) rc.top};
        ScreenToClient(static_cast<HWND>(m_parent->Handle()), &pt);
        x = pt.x;
        y = pt.y;
    }
}

ControlBase *ControlBase::SetPos(int x, int y) {
    // 逻辑根据 GetMonitorInfo 实现位置偏移，这里简化
    User32::W32_SetWindowPos(static_cast<HWND>(m_hwnd), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    return this;
}

void ControlBase::Size(int &w, int &h) {
    RECT rc;
    User32::W32_GetWindowRect(static_cast<HWND>(m_hwnd), &rc);
    w = rc.right - rc.left;
    h = rc.bottom - rc.top;
    scaleToDefaultDPI(w, h);
}

ControlBase *ControlBase::SetSize(int w, int h) {
    int x, y;
    Pos(x, y);
    clampSize(w, h);
    scaleWithWindowDPI(w, h);
    User32::W32_MoveWindow(static_cast<HWND>(m_hwnd), x, y, w, h, TRUE);
    return this;
}

int ControlBase::Width() {
    int w, h;
    Size(w, h);
    return w;
}

int ControlBase::Height() {
    int w, h;
    Size(w, h);
    return h;
}

bool ControlBase::Visible() { return User32::W32_IsWindowVisible(static_cast<HWND>(m_hwnd)); }

ControlBase *ControlBase::Show() {
    User32::W32_ShowWindow(static_cast<HWND>(m_hwnd), SW_SHOW);
    return this;
}

ControlBase *ControlBase::Hide() {
    User32::W32_ShowWindow(static_cast<HWND>(m_hwnd), SW_HIDE);
    return this;
}

ControlBase *ControlBase::EnableDragAcceptFiles(bool b) {
    DragAcceptFiles(static_cast<HWND>(m_hwnd), b);
    return this;
}

void ControlBase::SetFont(Font *font) {
    if (font && font->GetHFONT()) {
        User32::W32_SendMessage(static_cast<HWND>(m_hwnd), WM_SETFONT, reinterpret_cast<uintptr_t>(font->GetHFONT()), 1);
    }
    m_font = font;
}

bool ControlBase::InvokeRequired() {
    return GetWindowThreadProcessId(static_cast<HWND>(m_hwnd), nullptr) != GetCurrentThreadId();
}

void ControlBase::Invoke(std::function<void()> f) {
    if (!InvokeRequired()) {
        f();
        return;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_dispatchq.push_back(f);
    // PostMessage(m_hwnd, wmInvokeCallback, 0, 0);
}

void ControlBase::invokeCallbacks() {
    if (InvokeRequired()) return;
    std::vector<std::function<void()> > q;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        q.swap(m_dispatchq);
    }
    for (auto &v: q) v();
}

uintptr_t ControlBase::WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) {
    return User32::W32_DefWindowProc(static_cast<HWND>(m_hwnd), msg, wparam, lparam);
}

bool ControlBase::PreTranslateMessage(void *msg) { return false; }

void ControlBase::clampSize(int &width, int &height) const {
    if (m_minWidth != 0) width = _max(width, m_minWidth);
    if (m_maxWidth != 0) width = _min(width, m_maxWidth);
    if (m_minHeight != 0) height = _max(height, m_minHeight);
    if (m_maxHeight != 0) height = _min(height, m_maxHeight);
}

void ControlBase::scaleWithWindowDPI(int &width, int &height) const {
    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    width = ScaleWithDPI(width, dx);
    height = ScaleWithDPI(height, dy);
}

void ControlBase::scaleToDefaultDPI(int &width, int &height) const {
    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    width = User32::W32_MulDiv(width, 96, dx);
    height = User32::W32_MulDiv(height, 96, dy);
}

void ControlBase::GetWindowDPI(unsigned int &dpiX, unsigned int &dpiY) const {
    dpiX = User32::W32_GetDpiForWindow(static_cast<HWND>(m_hwnd));
    if (dpiX == 96 && ShCore::IsAvailable()) {
        HMONITOR monitor = MonitorFromWindow(static_cast<HWND>(m_hwnd), MONITOR_DEFAULTTONEAREST);
        if (monitor) {
            ShCore::W32_GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            return;
        }
    }
    if (dpiX == 96) {
        const HDC screen = GetDC(nullptr);
        dpiX = GetDeviceCaps(screen, LOGPIXELSX);
        dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(nullptr, screen);
        return;
    }
    dpiY = dpiX;
}

void ControlBase::SetAndClearStyleBits(unsigned int set, unsigned int clear) const {
    const auto hwnd = static_cast<HWND>(m_hwnd);
    const unsigned int style = User32::W32_GetWindowLongPtr(hwnd, GWL_STYLE);
    unsigned int newStyle = (style & ~clear) | set;
    if (newStyle != style) User32::W32_SetWindowLongPtr(hwnd, GWL_STYLE, newStyle);
}

// 占位
Rect *ControlBase::Bounds() {
    RECT rc;
    User32::W32_GetWindowRect(static_cast<HWND>(m_hwnd), &rc);
    if (m_isForm) {
        return new Rect(rc.left, rc.top, rc.right, rc.bottom);
    }
    return new Rect(rc.left, rc.top, rc.right, rc.bottom);
}

Rect *ControlBase::ClientRect() {
    RECT rc;
    User32::W32_GetClientRect(static_cast<HWND>(m_hwnd), &rc);
    return new Rect(rc.left, rc.top, rc.right, rc.bottom);
}

void ControlBase::SetTranslucentBackground() {
}

void ControlBase::SetMinSize(int width, int height) {
    m_minWidth = width;
    m_minHeight = height;
}

void ControlBase::SetMaxSize(int width, int height) {
    m_maxWidth = width;
    m_maxHeight = height;
}

void ControlBase::SetAlwaysOnTop(bool b) const {
    User32::W32_SetWindowPos(static_cast<HWND>(m_hwnd), b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void ControlBase::ShowTop() const {
    SetAlwaysOnTop(true);
    SetAlwaysOnTop(false);
    User32::W32_SetForegroundWindow(static_cast<HWND>(m_hwnd));
}

bool ControlBase::ToggleVisible() {
    if (Visible()) Hide();
    else Show();
    return Visible();
}