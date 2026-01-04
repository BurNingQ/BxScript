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

#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>

// ============================================================================
// 1. 声明部分 (Declaration)
// ============================================================================

// 前置声明
class ControlBase;
class Font;
class MenuItem;

// 事件回调类型
using EventCallback = std::function<void()>;
using KeyEventCallback = std::function<void(unsigned int)>;

class ControlBase {
protected:
    // 【防火墙】使用 void* 彻底隔离 HWND
    void* m_hwnd = nullptr;

    ControlBase* m_parent = nullptr;
    Font* m_font = nullptr;
    MenuItem* m_contextMenu = nullptr;

    bool m_isForm = false;
    int m_minWidth = 0, m_minHeight = 0;
    int m_maxWidth = 0, m_maxHeight = 0;

    // Invoke 队列
    std::mutex m_invokeMutex;
    std::vector<std::function<void()>> m_dispatchQueue;

public:
    // 事件
    EventCallback OnCreate;
    EventCallback OnClose;
    EventCallback OnPaint;
    EventCallback OnSize;
    KeyEventCallback OnKeyDown;
    KeyEventCallback OnKeyUp;

    virtual ~ControlBase();

    // 初始化
    void InitControl(const std::wstring& className, ControlBase* parent, unsigned int exStyle, unsigned int style);
    void InitWindow(const std::wstring& className, ControlBase* parent, unsigned int exStyle, unsigned int style);

    // 核心属性
    void* GetHandle() const { return m_hwnd; }
    ControlBase* GetParent() const { return m_parent; }
    void SetIsForm(bool isForm) { m_isForm = isForm; }

    // 业务方法
    void SetTheme(const std::wstring& appName);
    void SetText(const std::wstring& text);
    std::wstring GetText();
    void Close();

    // 尺寸与位置
    void SetSize(int w, int h);
    void GetSize(int& w, int& h);
    void SetPos(int x, int y);
    void GetPos(int& x, int& y);

    // 可见性与状态
    bool Visible();
    void Show();
    void Hide();
    bool Enabled();
    void SetEnabled(bool b);
    void SetFocus();

    // Invoke
    bool InvokeRequired();
    void Invoke(std::function<void()> f);
    void ProcessInvokeQueue();

    // 消息处理 (参数使用 void* 隔离 MSG 类型)
    virtual bool PreTranslateMessage(void* msg);

protected:
    // 内部使用的 DPI 辅助
    void GetWindowDPI(unsigned int& dpiX, unsigned int& dpiY);
    int ScaleWithDPI(int val, unsigned int dpi);
    int ScaleToDefaultDPI(int val, unsigned int dpi);

    int ClampWidth(int w);
    int ClampHeight(int h);
};

#endif // BXSCRIPT_CONTROL_BASE_H


// ============================================================================
// 2. 实现部分 (Implementation) - 只有定义了宏才会编译这部分
// ============================================================================

#ifdef BXSCRIPT_IMPLEMENTATION

#include <windows.h>
#include "WindowRegistry.h"
#include "internal/User32.h"
#include "internal/Kernel32.h"
#include "internal/UxTheme.h"
#include "internal/ShCore.h"

// 宏定义
#define HWND_CAST(ptr) static_cast<HWND>(ptr)
#define WM_BX_INVOKE (WM_USER + 1001)

// --- 实现开始 ---

ControlBase::~ControlBase() {
    if (m_hwnd && User32::W32_IsWindow(HWND_CAST(m_hwnd))) {
        WindowRegistry::Unregister(HWND_CAST(m_hwnd));
        User32::W32_DestroyWindow(HWND_CAST(m_hwnd));
    }
}

void ControlBase::InitControl(const std::wstring& className, ControlBase* parent, unsigned int exStyle, unsigned int style) {
    m_parent = parent;
    HWND hParent = parent ? HWND_CAST(parent->GetHandle()) : nullptr;

    HWND hwnd = User32::W32_CreateWindowEx(
        exStyle, className.c_str(), L"", style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hParent, nullptr, GetModuleHandle(nullptr), this
    );

    if (!hwnd) {
        MessageBoxW(nullptr, L"Cannot create window", L"Error", MB_ICONERROR);
        exit(1);
    }

    m_hwnd = hwnd;
    WindowRegistry::Register(hwnd, this);
}

void ControlBase::InitWindow(const std::wstring& className, ControlBase* parent, unsigned int exStyle, unsigned int style) {
    InitControl(className, parent, exStyle, style);
}

void ControlBase::SetTheme(const std::wstring& appName) {
    UxTheme::W32_SetWindowTheme(HWND_CAST(m_hwnd), appName.c_str(), nullptr);
}

void ControlBase::SetText(const std::wstring& text) {
    User32::W32_SetWindowText(HWND_CAST(m_hwnd), text.c_str());
}

std::wstring ControlBase::GetText() {
    HWND hwnd = HWND_CAST(m_hwnd);
    int len = User32::W32_GetWindowTextLength(hwnd);
    if (len <= 0) return L"";
    std::wstring str;
    str.resize(len + 1);
    User32::W32_GetWindowText(hwnd, &str[0], len + 1);
    str.resize(len);
    return str;
}

void ControlBase::Close() {
    WindowRegistry::Unregister(HWND_CAST(m_hwnd));
    User32::W32_DestroyWindow(HWND_CAST(m_hwnd));
}

void ControlBase::GetWindowDPI(unsigned int& dpiX, unsigned int& dpiY) {
    HWND hwnd = HWND_CAST(m_hwnd);
    dpiX = User32::W32_GetDpiForWindow(hwnd);
    if (dpiX == 96 && ShCore::IsAvailable()) {
         HMONITOR hMon = User32::W32_MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
         ShCore::W32_GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
         return;
    }
    dpiY = dpiX;
}

int ControlBase::ScaleWithDPI(int val, unsigned int dpi) {
    return User32::W32_MulDiv(val, dpi, 96);
}

int ControlBase::ScaleToDefaultDPI(int val, unsigned int dpi) {
    return User32::W32_MulDiv(val, 96, dpi);
}

void ControlBase::SetSize(int w, int h) {
    int x = 0, y = 0;
    GetPos(x, y);
    w = ClampWidth(w);
    h = ClampHeight(h);

    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    w = ScaleWithDPI(w, dx);
    h = ScaleWithDPI(h, dy);

    User32::W32_MoveWindow(HWND_CAST(m_hwnd), x, y, w, h, TRUE);
}

void ControlBase::GetSize(int& w, int& h) {
    RECT rc;
    User32::W32_GetWindowRect(HWND_CAST(m_hwnd), &rc);
    w = rc.right - rc.left;
    h = rc.bottom - rc.top;

    unsigned int dx, dy;
    GetWindowDPI(dx, dy);
    w = ScaleToDefaultDPI(w, dx);
    h = ScaleToDefaultDPI(h, dy);
}

void ControlBase::SetPos(int x, int y) {
    User32::W32_SetWindowPos(HWND_CAST(m_hwnd), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void ControlBase::GetPos(int& x, int& y) {
    RECT rc;
    User32::W32_GetWindowRect(HWND_CAST(m_hwnd), &rc);
    x = rc.left;
    y = rc.top;

    if (!m_isForm && m_parent) {
        POINT pt = { (LONG)x, (LONG)y };
        User32::W32_ScreenToClient(HWND_CAST(m_parent->GetHandle()), &pt);
        x = pt.x;
        y = pt.y;
    }
}

bool ControlBase::Visible() { return User32::W32_IsWindowVisible(HWND_CAST(m_hwnd)); }
void ControlBase::Show() { User32::W32_ShowWindow(HWND_CAST(m_hwnd), SW_SHOW); }
void ControlBase::Hide() { User32::W32_ShowWindow(HWND_CAST(m_hwnd), SW_HIDE); }
bool ControlBase::Enabled() { return User32::W32_IsWindowEnabled(HWND_CAST(m_hwnd)); }
void ControlBase::SetEnabled(bool b) { User32::W32_EnableWindow(HWND_CAST(m_hwnd), b); }
void ControlBase::SetFocus() { User32::W32_SetFocus(HWND_CAST(m_hwnd)); }

bool ControlBase::InvokeRequired() {
    if (!m_hwnd) return false;
    return User32::W32_GetWindowThreadProcessId(HWND_CAST(m_hwnd)) != Kernel32::W32_GetCurrentThreadId();
}

void ControlBase::Invoke(std::function<void()> f) {
    if (!InvokeRequired()) {
        f();
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_invokeMutex);
        m_dispatchQueue.push_back(f);
    }
    User32::W32_PostMessage(HWND_CAST(m_hwnd), WM_BX_INVOKE, 0, 0);
}

void ControlBase::ProcessInvokeQueue() {
    std::vector<std::function<void()>> queue;
    {
        std::lock_guard<std::mutex> lock(m_invokeMutex);
        queue.swap(m_dispatchQueue);
    }
    for (auto& f : queue) f();
}

bool ControlBase::PreTranslateMessage(void* msgVoid) {
    MSG* msg = static_cast<MSG*>(msgVoid);
    // 这里可以处理 msg
    return false;
}

int ControlBase::ClampWidth(int w) {
    if (m_minWidth > 0) w = std::max(w, m_minWidth);
    if (m_maxWidth > 0) w = std::min(w, m_maxWidth);
    return w;
}
int ControlBase::ClampHeight(int h) {
    if (m_minHeight > 0) h = std::max(h, m_minHeight);
    if (m_maxHeight > 0) h = std::min(h, m_maxHeight);
    return h;
}

#endif // BXSCRIPT_IMPLEMENTATION
