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
 * @brief    ControlBase
 */
#ifndef BXSCRIPT_CONTROL_BASE_H
#define BXSCRIPT_CONTROL_BASE_H

#include "Controller.h"
#include "Event.h"
#include "EventManager.h"
#include <vector>
#include <mutex>
#include "Utils.h"

class ControlBase : public Controller {
protected:
    void *m_hwnd = nullptr;
    Font *m_font = nullptr;
    Controller *m_parent = nullptr;
    MenuItem *m_contextMenu = nullptr;

    bool m_isForm = false;

    int m_minWidth = 0, m_minHeight = 0;
    int m_maxWidth = 0, m_maxHeight = 0;

    // Events
    EventManager onCreate;
    EventManager onClose;
    EventManager onKillFocus;
    EventManager onSetFocus;
    EventManager onDropFiles;
    EventManager onLBDown;
    EventManager onLBUp;
    EventManager onLBDbl;
    EventManager onMBDown;
    EventManager onMBUp;
    EventManager onRBDown;
    EventManager onRBUp;
    EventManager onRBDbl;
    EventManager onMouseMove;
    EventManager onMouseHover;
    EventManager onMouseLeave;
    EventManager onKeyUp;
    EventManager onKeyDown;
    EventManager onPaint;
    EventManager onSize;

    std::mutex m_mutex;
    std::vector<std::function<void()> > m_dispatchq;

public:
    ~ControlBase() override;

    std::wstring Text() override;

    bool Enabled() override;

    void SetFocus() override;

    void *Handle() override { return m_hwnd; }

    void Invalidate(bool erase) override;

    Controller *Parent() override { return m_parent; }

    void Pos(int &x, int &y) override;

    void Size(int &w, int &h) override;

    int Height() override;

    int Width() override;

    bool Visible() override;

    Rect *Bounds() override;

    Rect *ClientRect() override;

    ControlBase *SetText(const std::wstring &s) override;

    ControlBase *SetEnabled(bool b) override;

    ControlBase *SetPos(int x, int y) override;

    ControlBase *SetSize(int w, int h) override;

    ControlBase *EnableDragAcceptFiles(bool b) override;

    ControlBase *Show() override;

    ControlBase *Hide() override;

    MenuItem *ContextMenu() override { return m_contextMenu; }
    void SetContextMenu(MenuItem *menu) override { m_contextMenu = menu; }

    Font *GetFont() override { return m_font; }

    void SetFont(Font *font) override;

    bool InvokeRequired() override;

    void Invoke(std::function<void()> f) override;

    bool PreTranslateMessage(void *msg) override;

    uintptr_t WndProc(unsigned int msg, uintptr_t wparam, uintptr_t lparam) override;

    // Event Accessors
    EventManager &OnCreate() override { return onCreate; }
    EventManager &OnClose() override { return onClose; }
    EventManager &OnKillFocus() override { return onKillFocus; }
    EventManager &OnSetFocus() override { return onSetFocus; }
    EventManager &OnDropFiles() override { return onDropFiles; }
    EventManager &OnLBDown() override { return onLBDown; }
    EventManager &OnLBUp() override { return onLBUp; }
    EventManager &OnLBDbl() override { return onLBDbl; }
    EventManager &OnMBDown() override { return onMBDown; }
    EventManager &OnMBUp() override { return onMBUp; }
    EventManager &OnRBDown() override { return onRBDown; }
    EventManager &OnRBUp() override { return onRBUp; }
    EventManager &OnRBDbl() override { return onRBDbl; }
    EventManager &OnMouseMove() override { return onMouseMove; }
    EventManager &OnMouseHover() override { return onMouseHover; }
    EventManager &OnMouseLeave() override { return onMouseLeave; }
    EventManager &OnKeyUp() override { return onKeyUp; }
    EventManager &OnKeyDown() override { return onKeyDown; }
    EventManager &OnPaint() override { return onPaint; }
    EventManager &OnSize() override { return onSize; }

    void invokeCallbacks() override;

    // 初始化方法
    void InitControl(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style);

    void InitWindow(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style);

    void SetTheme(const std::wstring &appName);

    void SetHandle(void *hwnd) { m_hwnd = hwnd; }

    void GetWindowDPI(unsigned int &dpiX, unsigned int &dpiY);

    void SetAndClearStyleBits(unsigned int set, unsigned int clear);

    void SetIsForm(bool isform) { m_isForm = isform; }

    void SetTranslucentBackground();

    void SetMinSize(int width, int height);

    void SetMaxSize(int width, int height);

    void SetAlwaysOnTop(bool b);

    void ShowTop();

    bool ToggleVisible();

private:
    void clampSize(int &width, int &height);

    void scaleWithWindowDPI(int &width, int &height);

    void scaleToDefaultDPI(int &width, int &height);
};

#endif

// ============================================================================
// Implementation 部分 (BXSCRIPT_IMPLEMENTATION)
// ============================================================================
#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "internal/Gdi32.h"
#include "internal/UxTheme.h"
#include "internal/ShCore.h"

// 对应 Go 里的辅助函数
static int _min(int a, int b) { return a < b ? a : b; }
static int _max(int a, int b) { return a > b ? a : b; }

ControlBase::~ControlBase() {
    if (m_hwnd) {
        WindowRegistry::Unregister(m_hwnd);
    }
}

void ControlBase::InitControl(const std::wstring &className, Controller *parent, unsigned int exstyle, unsigned int style) {
    m_parent = parent;
    HWND hParent = parent ? static_cast<HWND>(parent->Handle()) : nullptr;
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

void ControlBase::SetTheme(const std::wstring& appName) {
    HRESULT hr = UxTheme::W32_SetWindowTheme(static_cast<HWND>(m_hwnd), appName.c_str(), nullptr);
}

std::wstring ControlBase::Text() {
    HWND hwnd = static_cast<HWND>(m_hwnd);
    int len = User32::W32_GetWindowTextLength(hwnd);
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
void ControlBase::Invalidate(bool erase) { User32::W32_InvalidateRect(static_cast<HWND>(m_hwnd), nullptr, erase); }

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

void ControlBase::clampSize(int &width, int &height) {
    if (m_minWidth != 0) width = _max(width, m_minWidth);
    if (m_maxWidth != 0) width = _min(width, m_maxWidth);
    if (m_minHeight != 0) height = _max(height, m_minHeight);
    if (m_maxHeight != 0) height = _min(height, m_maxHeight);
}

void ControlBase::scaleWithWindowDPI(int &width, int &height) {
    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    width = ScaleWithDPI(width, dx);
    height = ScaleWithDPI(height, dy);
}

void ControlBase::scaleToDefaultDPI(int &width, int &height) {
    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    width = User32::W32_MulDiv(width, 96, dx);
    height = User32::W32_MulDiv(height, 96, dy);
}

void ControlBase::GetWindowDPI(unsigned int &dpiX, unsigned int &dpiY) {
    dpiX = User32::W32_GetDpiForWindow(static_cast<HWND>(m_hwnd));
    if (dpiX == 96 && ShCore::IsAvailable()) {
        HMONITOR monitor = MonitorFromWindow(static_cast<HWND>(m_hwnd), MONITOR_DEFAULTTONEAREST);
        if (monitor) {
            ShCore::W32_GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            return;
        }
    }
    if (dpiX == 96) {
        HDC screen = GetDC(NULL);
        dpiX = GetDeviceCaps(screen, LOGPIXELSX);
        dpiY = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);
        return;
    }
    dpiY = dpiX;
}

void ControlBase::SetAndClearStyleBits(unsigned int set, unsigned int clear) {
    HWND hwnd = static_cast<HWND>(m_hwnd);
    unsigned int style = User32::W32_GetWindowLongPtr(hwnd, GWL_STYLE);
    unsigned int newStyle = (style & ~clear) | set;
    if (newStyle != style) User32::W32_SetWindowLongPtr(hwnd, GWL_STYLE, newStyle);
}

// 占位
Rect *ControlBase::Bounds() {
    RECT rc;
    User32::W32_GetWindowRect((HWND)m_hwnd, &rc);
    if (m_isForm) {
        return new Rect(rc.left, rc.top, rc.right, rc.bottom);
    }
    return new Rect(rc.left, rc.top, rc.right, rc.bottom);
}
Rect *ControlBase::ClientRect() {
    RECT rc;
    User32::W32_GetClientRect((HWND)m_hwnd, &rc);
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
void ControlBase::SetAlwaysOnTop(bool b) {
    User32::W32_SetWindowPos(static_cast<HWND>(m_hwnd), b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}
void ControlBase::ShowTop() {
    SetAlwaysOnTop(true);
    SetAlwaysOnTop(false);
    User32::W32_SetForegroundWindow(static_cast<HWND>(m_hwnd));
}
bool ControlBase::ToggleVisible() {
    if (Visible()) Hide();
    else Show();
    return Visible();
}

#endif
